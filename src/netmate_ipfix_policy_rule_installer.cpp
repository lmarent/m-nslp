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
			if (!handle_export_fields(mess, templ)) 
				throw policy_rule_installer_error("Invalid ipfix message",
					msg::information_code::sc_signaling_session_failures,
					msg::information_code::sigfail_rule_action_not_applicable);
		}
				
		templ = get_filter_template(mess);
		if (templ== NULL){
			throw policy_rule_installer_error("Invalid ipfix message",
				msg::information_code::sc_signaling_session_failures,
				msg::information_code::sigfail_wrong_conf_message); 
		}
		else{
			if (!include_all_data_fields(mess, templ));
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


void netmate_ipfix_policy_rule_installer::install(const mt_policy_rule *rule )
		throw (policy_rule_installer_error) {

	bool checked = true;
	LogDebug("install(): ");
	
	
	mt_policy_rule::const_iterator i;
	for ( i = rule->begin(); i != rule->end(); i++){
		const msg::mnslp_ipfix_message *mess = get_ipfix_message(&(i->second));

		// Create configuration message.
		std::string action = create_action_command(mess);
		std::string post_fields = create_postfield_command(i->first, mess);
	
		// Create the http command to send for configuration
		execute_command(action, post_fields);
	}
}


void netmate_ipfix_policy_rule_installer::remove(const mt_policy_rule *object)
		throw (policy_rule_installer_error) {

	LogDebug("remove(): " );


}


void netmate_ipfix_policy_rule_installer::remove_all()
		throw (policy_rule_installer_error) {

	LogDebug("NOP: removing all metering policy rules ");
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
		
	for (std::list<int>::iterator it = templates.begin(); it != templates.end(); it++){
		msg::mnslp_ipfix_template *templ =  mess->get_template( *it );
		if (templ->get_type() == msg::OPTION_TEMPLATE){
			break;
		}
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
	
	std:string action = "/add_task";
	return action;
}

std::string  
netmate_ipfix_policy_rule_installer::create_postfield_command(
					const mspec_rule_key &key, const msg::mnslp_ipfix_message *message) const
		throw () {

	msg::mnslp_ipfix_template *templ = NULL;
	LogDebug("create_filter_command(): ");
	std:string postfields =	"Rule=" + key.get_string_key();
 
	// Add all filters specified. 
	templ = get_filter_template(message);
	if (templ!= NULL){
		postfields.append(" ");	
		postfields.append( build_command_filter_fields(message, templ) );
	}
	
	// Add all export fields ( for now it just add the package for metering )
	templ = get_export_template(message);
	if (templ!= NULL){
		postfields.append(" ");
		postfields.append( build_command_export_fields(message, templ) );
	}
	
	
	
}

bool
netmate_ipfix_policy_rule_installer::include_all_data_fields(const msg::mnslp_ipfix_message *mess, 
								msg::mnslp_ipfix_template *templ)
{
	if (mess != NULL)
		return mess->include_all_data_fields(templ);
	else
		return false;
}

bool
netmate_ipfix_policy_rule_installer::handle_export_fields(const msg::mnslp_ipfix_message *mess, 
									msg::mnslp_ipfix_template *templ)
{
	for (int i = 0; i < templ->get_numfields(); i++ ){
		msg::mnslp_ipfix_field field = templ->get_field(i).elem;
		if (!(get_action_container().check_field_availability(
						get_metering_application(),	field)))
			return false;
	}
	return true;
}	

std::string
netmate_ipfix_policy_rule_installer::build_command_export_fields(const msg::mnslp_ipfix_message *mess, 
									msg::mnslp_ipfix_template *templ) const
{
	std::set<std::string> packages;
	std::set<std::string> export_fields;
	std::string val_return = "-a ";
	
	for (int i = 0; i < templ->get_numfields(); i++ ){
		msg::mnslp_ipfix_field field = templ->get_field(i).elem;
		export_fields.insert( get_action_container().get_field_traslate( 
									get_metering_application(), field) );
									
		packages.insert( get_action_container().get_package( 
									get_metering_application(), field) );
	}
	
	std::set<string>::iterator it;
	for (it=packages.begin(); it!=packages.end(); ++it){
		val_return.append(*it); 
		val_return.append(" ");
	}	
  
	return val_return;
}	

std::string
netmate_ipfix_policy_rule_installer::build_command_filter_fields(const msg::mnslp_ipfix_message *mess, 
									msg::mnslp_ipfix_template *templ) const
{
	std::map<std::string, std::string> filter_fields;
	std::string val_return = "-r ";
	
	
	for (int i = 0; i < templ->get_numfields(); i++ ){
		msg::mnslp_ipfix_field field = templ->get_field(i).elem;
		
		std::list<std::string> values = mess->get_field_data_values(templ,field);
		std::list<std::string>::iterator it;
		for (it=values.begin(); it!=values.end(); ++it){
			if (it == values.begin()){
				filter_fields.insert( std::pair<std::string,std::string>
									( get_filter_container().get_field_traslate( 
										get_metering_application(), field),
									   *it ) );
			}
			else{ 
				std::string comma = ",";
				filter_fields.insert( std::pair<std::string,std::string>
									( get_filter_container().get_field_traslate( 
										get_metering_application(), field),
									   comma.append(*it) ) );
			}					
		}
	}
	
	std::map<std::string, std::string>::iterator it;
	for (it=filter_fields.begin(); it!=filter_fields.end(); ++it){
		val_return.append(it->first);
		val_return.append(":");
		val_return.append(it->second);
	}	
  
	return val_return;
}	


bool 
netmate_ipfix_policy_rule_installer::handle_filter_fields(const msg::mnslp_ipfix_message *mess, 
									msg::mnslp_ipfix_template *templ)
{
	for (int i = 0; i < templ->get_numfields(); i++ ){
		msg::mnslp_ipfix_field field = templ->get_field(i).elem;
		if (!(get_filter_container().check_field_availability(
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
	int port = atoi(get_metering_port().c_str());
	unsigned long rcode;
	bool val_return = true;
#ifdef USE_SSL
    int use_ssl = 0;
#endif
	CURL *curl;
	CURLcode res;
	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, out;
    
	// initialize libcurl
	curl = curl_easy_init();
	if (!curl) {
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
     
    curl_easy_cleanup(curl);
	xsltFreeStylesheet(cur);
	xsltCleanupGlobals();
	xmlCleanupParser();

}



// EOF
