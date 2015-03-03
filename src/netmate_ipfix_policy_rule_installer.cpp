/// ----------------------------------------*- mode: C++; -*--
/// @file netmate_policy_rule_installer.cpp
/// The netmate_policy_rule_installer class.
/// ----------------------------------------------------------
/// $Id: netmate_policy_rule_installer.cpp 2558 2015-01-16 11:32:00 amarentes $
/// $HeadURL: https://./src/netmate_policy_rule_installer.cpp $
// ===========================================================
//                      
// Copyright (C) 2005-2007, all rights reserved by
// - System and Computing Engineering, Universidad de los Andes
//
// More information and contact:
// https://www.uniandes.edu.co/
//                      
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ===========================================================
#include <sstream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <set>

#include "address.h"
#include "logfile.h"

#include "policy_rule_installer.h"
#include "netmate_ipfix_metering_config.h"

// curl includes
#include <curl/curl.h>
#include <list>

// xml includes
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>


using namespace mnslp;
using mnslp::msg::information_code;
using namespace protlib::log;


#define LogError(msg) Log(ERROR_LOG, LOG_NORMAL, \
	"netmate_ipfix_policy_rule_installer", msg)
#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, \
	"netmate_ipfix_policy_rule_installer", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, \
	"netmate_ipfix_policy_rule_installer", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, \
	"netmate_ipfix_policy_rule_installer", msg)
#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, \
	"netmate_ipfix_policy_rule_installer", \
	msg << " at " << __FILE__ << ":" << __LINE__)


std::string netmate_ipfix_policy_rule_installer::rule_group = "NETMATE_IPFIX";

netmate_ipfix_policy_rule_installer::netmate_ipfix_policy_rule_installer(
		mnslp_config *conf) throw () 
		: policy_rule_installer(conf) {

	// nothing to do
}


netmate_ipfix_policy_rule_installer::~netmate_ipfix_policy_rule_installer() throw () {
	// nothing to do
}


void netmate_ipfix_policy_rule_installer::setup()
		throw (policy_rule_installer_error) {

	/*
	 * Establishes the ip address, user and password for metering connection.
	 * It eliminates all metering activities previously configurated.
	 * It checks if the metering is working on this node, if not put the
	 * configuration as not posible to do metering. 
	 */
	policy_rule_installer::setup();
    
}


void netmate_ipfix_policy_rule_installer::check(const msg::mnslp_mspec_object *object)
		throw (policy_rule_installer_error) {

	
	LogDebug("check(): ");

	const msg::mnslp_ipfix_message *mess = get_ipfix_message(object);
	msg::mnslp_ipfix_template *templ = NULL;
	
	// An ipfix configuration message has to have a template and an option
	// template.
	if ( mess->get_num_templates() == 2 ){
		
		templ = get_export_template(mess);
		if (templ== NULL){
			throw policy_rule_installer_error("Invalid ipfix message",
				msg::information_code::sc_signaling_session_failures,
				msg::information_code::sigfail_wrong_conf_message); 
		}
		else{ 
			if (handle_export_fields(mess, templ) == false) 
				throw policy_rule_installer_error("Invalid ipfix message",
					msg::information_code::sc_signaling_session_failures,
					msg::information_code::sigfail_rule_action_not_applicable);
		}
						
		templ = get_filter_template(mess);
		if (templ== NULL)
		{
			throw policy_rule_installer_error("Invalid ipfix message",
				msg::information_code::sc_signaling_session_failures,
				msg::information_code::sigfail_wrong_conf_message); 
		}
		else
		{
			if (include_all_data_fields(mess, templ) == false)
				throw policy_rule_installer_error("Invalid ipfix message",
					msg::information_code::sc_signaling_session_failures,
					msg::information_code::sigfail_wrong_conf_message);
				
			// Check if filter fields can be handled by the metering app. 
			if (!handle_filter_fields(mess, templ))
				throw policy_rule_installer_error("Invalid ipfix message",
					msg::information_code::sc_signaling_session_failures,
					msg::information_code::sigfail_filter_action_not_applicable);
		}					
		
	}		
	else
	{
		throw policy_rule_installer_error("Invalid ipfix message",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_wrong_conf_message);
	}

}


mt_policy_rule * netmate_ipfix_policy_rule_installer::install(const mt_policy_rule *rule )
{

	bool checked = true;
	LogDebug("install(): ");
	
	mt_policy_rule *rule_return = new mt_policy_rule();
	mt_policy_rule::const_iterator i;
	std::map<std::string, std::string> commands;
	std::map<std::string, std::string>::iterator it_commands;
	
	
	std::cout << "before exec command" << std::endl;
	
	for ( i = rule->begin(); i != rule->end(); i++){
		
		std::vector<std::string> rule_keys;
		const msg::mnslp_ipfix_message *mess = get_ipfix_message(i->second);

		// Create configuration message.
		std::string action = create_action_command(mess);
		commands = create_postfield_command(i->first, mess);
		
		for (it_commands = commands.begin(); it_commands != commands.end(); ++it_commands){
			// Create the http command to send for configuration
			try{
				execute_command(action, it_commands->second);
				rule_keys.push_back(it_commands->first);
			}
			catch(policy_rule_installer_error &e){
				std::cout << "It cannot install the object" << std::endl;
			}
		}
		// If we can install at least a rule, we insert the metering object in the policy
		// rule to return.
		if (rule_keys.size() > 0 ){
			rule_return->set_object(i->first, (i->second)->copy());
			rule_return->set_commands(i->first, rule_keys);
		}
	}
	return rule_return;
}


mt_policy_rule * 
netmate_ipfix_policy_rule_installer::remove(const mt_policy_rule *rule) {

	LogDebug("remove(): " );
	std::string action = "/rm_task";
	mt_policy_rule *rule_return = new mt_policy_rule(*rule);
	rule_return->clear_commands();
	std::vector<std::string> return_keys;

	mt_policy_rule::const_iterator_commands i;
	for ( i = rule->begin_commands(); i != rule->end_commands(); i++){
		std::vector<std::string> keys = i->second;
		for ( int index = 0; index < keys.size(); index ++ ) {
			try
			{
				std::string postfields = "RuleID=";
				postfields.append(netmate_ipfix_policy_rule_installer::rule_group);
				postfields.append(".");
				postfields.append(keys[index]);
				execute_command(action, postfields);
				return_keys.push_back(postfields);
			}
			catch (policy_rule_installer_error &e)
			{
				// TODO AM: Implement behavior for different errors.
			}
		}
		rule_return->set_commands(i->first, return_keys);
	}
	
	return rule_return;
}


bool netmate_ipfix_policy_rule_installer::remove_all() {

	LogDebug("NOP: removing all metering policy rules ");
	std::string action = "/rm_task";
	std::string postfields = "RuleID=";
	postfields.append(netmate_ipfix_policy_rule_installer::rule_group);
	try
	{
		execute_command(action, postfields);
		return true;
	}
	catch (policy_rule_installer_error &e) 
	{
		return false;
	}

}


const msg::mnslp_ipfix_message *
netmate_ipfix_policy_rule_installer::get_ipfix_message(const msg::mnslp_mspec_object *object)
{
	const msg::mnslp_ipfix_message *mess = dynamic_cast<const msg::mnslp_ipfix_message *>(object);
	assert( mess != NULL );
	return mess;
}

msg::mnslp_ipfix_template * 
netmate_ipfix_policy_rule_installer::get_filter_template(const msg::mnslp_ipfix_message *mess) const
{
	
 	
 	msg::mnslp_ipfix_template *templ = NULL;
 	std::list<int> templates = mess->get_template_list();
	for (std::list<int>::iterator it = templates.begin(); it != templates.end(); it++)
	{
		templ =  mess->get_template( *it );
		if (templ->get_type() == msg::OPTION_TEMPLATE)
			break;
		
	}
	return templ;

}

msg::mnslp_ipfix_template * 
netmate_ipfix_policy_rule_installer::get_export_template(const msg::mnslp_ipfix_message *mess) const
{

 	msg::mnslp_ipfix_template *templ = NULL;
 	std::list<int> templates = mess->get_template_list();
		
	for (std::list<int>::iterator it = templates.begin(); it != templates.end(); it++){
		templ =  mess->get_template( *it );
		if (templ->get_type() == msg::DATA_TEMPLATE){
			break;
		}
	}
	return templ;

}

std::string  
netmate_ipfix_policy_rule_installer::create_action_command(
					const msg::mnslp_ipfix_message *message) const
		throw () {

	LogDebug("create_action_command(): " );
	
	std::string action = "/add_task";
	return action;
}

std::map<std::string, std::string> 
netmate_ipfix_policy_rule_installer::create_postfield_command(
					const mspec_rule_key &key, 
					const msg::mnslp_ipfix_message *message) const throw () 
{

	msg::mnslp_ipfix_template *templ = NULL;
	LogDebug("create_filter_command(): ");
	std::string postfields;
	std::map<std::string, std::string> val_return;
	
	std::cout << "create string with filter fields" << std::endl;
	// Add all filters specified. 
	templ = get_filter_template(message);
	if (templ!= NULL){
		postfields.append(" ");	
		postfields.append( build_command_filter_fields(message, templ) );
	}
	else{
		// TODO AM: Put in log.
		std::cout << "Option template not found" << std::endl;
	}
	
	std::cout << "create string with export fields" << std::endl;
	
	// Add all export fields ( for now it just add the package for metering )
	templ = get_export_template(message);
	if (templ!= NULL){
		val_return = build_command_export_fields(key, message, templ, postfields);
	}
	else{
		// TODO AM: Put in log.
		std::cout << "Export template not found" << std::endl;
	}
	
	return val_return;
}

bool
netmate_ipfix_policy_rule_installer::include_all_data_fields( const msg::mnslp_ipfix_message *mess, 
								msg::mnslp_ipfix_template *templ)
{
	if (mess != NULL){
		return mess->include_all_data_fields(templ);
	}
	else
		return false;
}

bool
netmate_ipfix_policy_rule_installer::handle_export_fields(const msg::mnslp_ipfix_message *mess, 
									msg::mnslp_ipfix_template *templ)
{
	for (int i = 0; i < templ->get_numfields(); i++ ){
		msg::mnslp_ipfix_field field = templ->get_field(i).elem;
		
		// Verify  the field as export configured.
		if ( get_application_configuration_container()->is_export_field( 
				get_metering_application(),	field) == false ){
			return false;
		}
		
		// Verify the field as a member of an action configured.
		if (!(get_action_container()->check_field_availability(
						get_metering_application(),	field))){
			return false;
		}
	}
	return true;
}	

std::map<std::string, std::string>
netmate_ipfix_policy_rule_installer::build_command_export_fields( const mspec_rule_key &key, 
									const msg::mnslp_ipfix_message *mess, 
									msg::mnslp_ipfix_template *templ, 
									std::string filter_def) const
{
	std::set<std::string> export_fields;
	
	std::map<std::string, std::set<string> > exports;
	std::map<std::string, std::set<string> >::iterator it_export;
	const netmate_ipfix_metering_config * met_conf;
		
	for (int i = 0; i < templ->get_numfields(); i++ )
	{
		msg::mnslp_ipfix_field field = templ->get_field(i).elem;
		if ( get_application_configuration_container()->
				is_export_field( get_metering_application(), field) )
		{
			export_fields.insert( get_action_container()->get_field_traslate( 
									get_metering_application(), field) );
																		
			met_conf = dynamic_cast< const netmate_ipfix_metering_config* >( get_action_container()->get_package( 
									get_metering_application(), field) );
			
			if (met_conf != NULL)
			{
				
				std::string export_str;
				// Establish the target for the export process.
				if (met_conf->get_export_procedure().compare("file") == 0){
					export_str.append("file:");
					export_str.append(met_conf->get_export_directory());
					export_str.append(key.to_string());
				}
				else if (met_conf->get_export_procedure().compare("ipfix") == 0){					
					export_str.append("ipfix:");
				}					
				else{
					export_str.append("file:");
					export_str.append(get_export_directory());
					export_str.append(key.to_string());					
				}
				
				it_export = exports.find(export_str);
				if (it_export == exports.end()){
					std::set<std::string> export_commands;
					export_commands.insert(met_conf->get_metering_procedure());
					exports[export_str] = export_commands;
				}
				else{
					(it_export->second).insert( met_conf->get_metering_procedure());
				}
			}	
		}
	}
	
	std::map<std::string, std::string> list_commands;
	int sequence = 1;
	for (it_export=exports.begin(); it_export!= exports.end(); ++it_export){
		ostringstream command;
		ostringstream comp_key;
		comp_key << key.to_string() << "_" << sequence;
		command << "Rule=" << netmate_ipfix_policy_rule_installer::rule_group 
						   << "." << key.to_string() << "_" 
					       << sequence << " " <<  filter_def << " -a ";
		
		std::set<string>::iterator it_packages;
		for (it_packages = (it_export->second).begin(); 
				it_packages != (it_export->second).end(); ++it_packages){
			command << *it_packages << " ";
		}	

		command << "-e target=" << it_export->first; 
		list_commands.insert(std::pair<std::string, std::string>(comp_key.str(), command.str()));
		sequence = sequence + 1;
	}	
  
	return list_commands;
}	

std::string
netmate_ipfix_policy_rule_installer::build_command_filter_fields( const msg::mnslp_ipfix_message *mess, 
									msg::mnslp_ipfix_template *templ) const
{
	std::map<std::string, std::string> filter_fields;
	std::string val_return = "-r";
	
	
	for (int i = 0; i < templ->get_numfields(); i++ ){
		
		msg::mnslp_ipfix_field field = templ->get_field(i).elem;
		if ( get_application_configuration_container()->
				is_filter_field( get_metering_application(), field) )
		{
			std::list<std::string> values = mess->get_field_data_values(templ,field);
			print_filter_values(field, values, filter_fields);
		}
	}
		
	std::map<std::string, std::string>::iterator it;
	for (it=filter_fields.begin(); it!=filter_fields.end(); ++it){
		val_return.append(" ");
		val_return.append(it->first);
		val_return.append("=");
		val_return.append(it->second);
	}	
		
	return val_return;
}	

void 
netmate_ipfix_policy_rule_installer::print_filter_values(msg::mnslp_ipfix_field &field, 
					std::list<std::string> &values,
					std::map<std::string, std::string> &filter_fields) const
{
	std::list<std::string>::iterator it;
	std::string comma = ",";
	for (it = values.begin(); it != values.end(); ++it)
	{
		if (it == values.begin())
		{
			filter_fields.insert( std::pair<std::string,std::string>
								( get_application_configuration_container()->get_field_traslate( 
									get_metering_application(), field),
								   *it ) );
		}
		else
		{ 	
			filter_fields.insert( std::pair<std::string,std::string>
								( get_application_configuration_container()->get_field_traslate( 
									get_metering_application(), field),
								   comma.append(*it) ) );
		}					
	}
}

bool 
netmate_ipfix_policy_rule_installer::handle_filter_fields(const msg::mnslp_ipfix_message *mess, 
									msg::mnslp_ipfix_template *templ)
{
	for (int i = 0; i < templ->get_numfields(); i++ ){
		msg::mnslp_ipfix_field field = templ->get_field(i).elem;
		if (!(get_application_configuration_container()->check_field_availability(
					get_metering_application(),field)))
			return false;
	}
	return true;
}

/* ------------------- curl callbacks ---------------- */

size_t writedata( void *ptr, size_t size, size_t nmemb, void  *stream)
{
    string *s = (string *) stream;

    s->append((char *)ptr, size*nmemb);

    return size*nmemb;
}


std::string netmate_ipfix_policy_rule_installer::getErr(char *e)
{
    string err = e;

    int p = err.find_last_of(":");

    if (p > 0) {
        return err.substr(0,p) + ": " + 
          string(strerror(atoi(err.substr(p+1, err.length()-p).c_str())));
    }

    return "";
}

bool 
netmate_ipfix_policy_rule_installer::execute_command(std::string action, std::string post_fields)
{

    char cebuf[CURL_ERROR_SIZE], *ctype;
	char *post_body = NULL;
	string userpwd;
	string server = get_metering_server();
	string response;
	string input, input2;
	string stylesheet = get_metering_xsl();
	int port = get_metering_port();
	unsigned long rcode;
	bool val_return = true;
#ifdef USE_SSL
    int use_ssl = 0;
#endif
	CURL *curl;
	CURLcode res;
	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, out;
    
    std::cout << "The action given is:" << action << std::endl;
	// initialize libcurl
	curl = curl_easy_init();
	if (curl == NULL) {
		throw policy_rule_installer_error("Error during policy installation",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_metering_connection_broken);
	}

	memset(cebuf, 0, sizeof(cebuf));
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
    cur = xsltParseStylesheetFile((const xmlChar *)stylesheet.c_str());

#ifdef USE_SSL
    use_ssl = 1;
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLCERT, CERT_FILE.c_str());
    curl_easy_setopt(curl, CURLOPT_SSLKEYPASSWD, SSL_PASSWD);
    curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLKEY, CERT_FILE.c_str());
    /* do not validate server's cert because its self signed */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    /* do not verify host */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
#endif

    // debug
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, cebuf);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writedata);
   
    userpwd = get_user() + ":" + get_password();
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd.c_str());
  
    ostringstream url;
          
    // build URL
#ifdef USE_SSL
    if (use_ssl) {
       url << "https://";
    } else {
#endif
       url << "http://";
#ifdef USE_SSL
    }
#endif
    url << server << ":" << port;
    url << action;
                       
    char *_url = strdup(url.str().c_str());
    curl_easy_setopt(curl, CURLOPT_URL, _url);
    post_body =  curl_escape(post_fields.c_str(), post_fields.length());	
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
       response = "";
       free(_url);
#ifdef HAVE_CURL_FREE
       curl_free(post_body);
#else
       free(post_body);
#endif
       std::cout << "Here 0" << std::endl;
	   curl_easy_cleanup(curl);
	   xsltFreeStylesheet(cur);
	   xsltCleanupGlobals();
	   xmlCleanupParser();
	   throw policy_rule_installer_error(getErr(cebuf),
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_metering_connection_broken);       
    }

    res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
    if (res != CURLE_OK) {
       response = "";
       free(_url);
#ifdef HAVE_CURL_FREE
       curl_free(post_body);
#else
       free(post_body);
#endif

	   std::cout << "Here 4 " << std::endl;
      
	   curl_easy_cleanup(curl);
	   xsltFreeStylesheet(cur);
	   xsltCleanupGlobals();
	   xmlCleanupParser();
	   throw policy_rule_installer_error(getErr(cebuf),
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_metering_connection_broken);
    }

    res = curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &rcode);
    if (res != CURLE_OK) {

       response = "";
       free(_url);
#ifdef HAVE_CURL_FREE
       curl_free(post_body);
#else
       free(post_body);
#endif

	   std::cout << "Here 5 " << std::endl;
      
	   curl_easy_cleanup(curl);
	   xsltFreeStylesheet(cur);
	   xsltCleanupGlobals();
	   xmlCleanupParser();
	   throw policy_rule_installer_error(getErr(cebuf),
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_metering_connection_broken);
    }

    if (!strcmp(ctype, "text/xml")) {
       // translate
       doc = xmlParseMemory(response.c_str(), response.length());
       out = xsltApplyStylesheet(cur, doc, NULL);
       xsltSaveResultToFile(stdout, out, cur);         
       xmlFreeDoc(out);
       xmlFreeDoc(doc);
    } 
    else {
       // just dump
       cout << response << endl;
    }

    response = "";
    free(_url);
#ifdef HAVE_CURL_FREE
    curl_free(post_body);
#else
    free(post_body);
#endif

    std::cout << "Here 6 " << std::endl;
     
    curl_easy_cleanup(curl);
	xsltFreeStylesheet(cur);
	xsltCleanupGlobals();
	xmlCleanupParser();

}



// EOF
