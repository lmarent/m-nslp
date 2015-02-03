/// ----------------------------------------*- mode: C++; -*--
/// @file policy_field_mapping.cpp
/// The policy_field_mapping class.
/// ----------------------------------------------------------
/// $Id: policy_field_mapping.cpp 2558 2015-01-16 13:49:00 amarentes $
/// $HeadURL: https://./src/policy_field_mapping.cpp $
// ===========================================================
//                      
// Copyright (C) 2014-2015, all rights reserved by
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

#include "policy_field_mapping.h"
#include "msg/information_code.h"
#include "policy_rule_installer.h"
#include <sstream>
#include <iostream>


namespace mnslp {


std::string policy_field_mapping::field_str = "FIELD";
std::string policy_field_mapping::eno_str = "ENO";
std::string policy_field_mapping::field_type_str = "FIELDTYPE";
std::string policy_field_mapping::field_name_str = "NAME";
std::string policy_field_mapping::configuration_str = "CONFIGURATION";
std::string policy_field_mapping::filter_str = "FILTER";
std::string policy_field_mapping::export_str = "EXPORT";

application_field_mapping::application_field_mapping(std::string configuration, 
									std::string filter, std::string exporting, 
									std::string application_key)
{									
	if (configuration.compare("true") == 0 )
		is_configuration_field = true;
	else
		is_configuration_field = false;

    if (filter.compare("true") == 0 )
		is_filter_field = true;
	else 
		is_filter_field = false;

	if (exporting.compare("true") == 0 )		
		is_export_field = true;
	else 
	    is_export_field = false; 
	
	if ((is_configuration_field == true) 
	    and (is_filter_field == false)
	    and (is_export_field == false)){
		application_field_key = "";
	}
	else
	{
	   if (application_key.compare("NoTraslation") != 0 )
		  application_field_key = application_key;
	   else
		  throw policy_rule_installer_error("Field bad configured",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);
	}
}

application_field_mapping::application_field_mapping(const application_field_mapping &rhs )
{
	is_configuration_field = rhs.is_configuration_field;
	is_filter_field = rhs.is_filter_field;
	is_export_field = rhs.is_export_field;
	application_field_key = rhs.application_field_key;
}

bool 
application_field_mapping::operator==(const application_field_mapping &rhs) const
{
	if (is_configuration_field != rhs.is_configuration_field)
		return false;

	if (is_filter_field != rhs.is_filter_field)
		return false;

	if (is_export_field != rhs.is_export_field)
		return false;

	if (application_field_key.compare(rhs.application_field_key) != 0 )
		return false;
	
	return true;
		
}

bool 
application_field_mapping::operator!=(const application_field_mapping &rhs) const
{
	return !(operator ==(rhs));
}

std::string 
application_field_mapping::to_string() const
{

	ostringstream temp;

	temp << "is to configure:" << is_configuration_field 
		 << " is filter:" << is_filter_field 
		 << " is export:" << is_export_field 
		 << " application field key:" << application_field_key 
		 << std::endl;
	
	return  temp.str();
	
}

policy_field_mapping::policy_field_mapping()
{
	// NOTHING TO DO.
}


void policy_field_mapping::processNode(int level, xmlTextReaderPtr reader) 
{
    int ret;
    int eno = 0;
    int field_type = 0;
    std::string name_field, configuration, filter, exp;
    std::string field_map;
    
    xmlChar *name, *value;

    name = xmlTextReaderName(reader);
            
    if (name){
		if (xmlStrEqual( name,  (xmlChar *) 
						 policy_field_mapping::field_str.c_str()) == 1 )
		{
			eno = atoi( (processAttribute( reader, 
						 policy_field_mapping::eno_str)).c_str());
							
			field_type = atoi( (processAttribute( reader, 
						 policy_field_mapping::field_type_str)).c_str()); 
							
			name_field = processAttribute( reader, 
						 policy_field_mapping::field_name_str);

			configuration = processAttribute( reader, 
						 policy_field_mapping::configuration_str);

			filter = processAttribute( reader, 
						 policy_field_mapping::filter_str);

			exp = processAttribute( reader, 
						 policy_field_mapping::export_str);
			
			// Bring the definition of field from the container.
			msg::mnslp_ipfix_field_key *key = new msg::mnslp_ipfix_field_key(eno,field_type); 

			// Bring the mapping between el ipfix field container and the
			// metering application.
			field_map = processTextNode(level + 1, reader);

			set_field(key,
				application_field_mapping(configuration,filter,exp,field_map));
		}

		xmlFree(name);
	}
	 
}

int policy_field_mapping::read_from_xml(int level,  xmlTextReaderPtr reader)
{
	int ret;
	std::string metering_app;
	const xmlChar *name, *value;
		
	if (xmlTextReaderDepth(reader) == level)
	{
        // Read the metering application.
        metering_app = processAttribute(reader, "ID");
        metering_application = metering_app;
                
        ret = xmlTextReaderRead(reader);
		level++;
        while (ret == 1) 
        {
			if ((xmlTextReaderDepth(reader) >= level) 
			     and (xmlTextReaderNodeType(reader) == 1) )
			{
				processNode(level, reader);
				ret = xmlTextReaderRead(reader);				
            }
            else
				if (xmlTextReaderDepth(reader) < level)
					// Correspond to another policy_action_mapping or blank space.
					return ret;
				else
					ret = xmlTextReaderRead(reader);
				
        }
	}
	else
	{
		throw policy_rule_installer_error("Configuration file does not parse",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);	
	}
}

policy_field_mapping::policy_field_mapping(const policy_field_mapping &rhs)
{
	metering_application=rhs.metering_application;

	std::map<msg::mnslp_field_key *, application_field_mapping>::const_iterator i;
	for ( i = rhs.fields.begin(); i != rhs.fields.end(); i++ ) {
		msg::mnslp_field_key *key = (*(i->first)).copy();
		fields.insert(std::pair<msg::mnslp_field_key *, application_field_mapping> (key, i->second));
	}
	
	
}
	
policy_field_mapping::~policy_field_mapping()
{

	std::map<msg::mnslp_field_key *, application_field_mapping>::iterator i;
	for ( i = fields.begin(); i != fields.end(); i++ ) {
		delete i->first;
		fields.erase(i);
	}

}

// If the field already exists, the method does not replace its definition.        
void policy_field_mapping::set_field(msg::mnslp_field_key *_ipfixfield, 
									 application_field_mapping fieldMetering)
{
	try{
	   get_field(_ipfixfield);
	}
	catch(policy_rule_installer_error &e){
		fields.insert(std::pair<msg::mnslp_field_key *, 
						 application_field_mapping>(_ipfixfield, fieldMetering));
	}
}

application_field_mapping
policy_field_mapping::get_field(msg::mnslp_field_key *_ipfixfield) const
{
	const_iterator i = fields.find(_ipfixfield);
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		if (*(i->first) == *_ipfixfield)
			return i->second;
	}

	throw policy_rule_installer_error("Application field mapping not found",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);	
}

void 
policy_field_mapping::set_metering_application(std::string metering_app)
{
	metering_application = metering_app;
}
        
bool
policy_field_mapping::operator==(const policy_field_mapping &rhs) const
{
	
	if (metering_application.compare(rhs.metering_application) != 0)
		return false;
	
	try 
	{ 
		// All entries have to be identical.
		for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
				if ( i->second != rhs.get_field(i->first) )
					return false;
		}
	}	
	catch(policy_rule_installer_error &e)
	{
		return false;
	}		
	
	return true;

}

bool
policy_field_mapping::operator!=(const policy_field_mapping &rhs) const
{
	return ! operator==(rhs);
}

std::ostream &
policy_field_mapping::operator<<(std::ostream &out)
{
	
	// Print all entries.
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		out << "Key:" << (i->first)->to_string() << " Value:" << (i->second).to_string() << std::endl;
	}
	
}    

std::string
policy_field_mapping::to_string() const
{
	ostringstream temp;

	// Print all entries.
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		temp << "Key:" << (i->first)->to_string() << " Value:" << (i->second).to_string() << std::endl;
	}
	
	return  temp.str();
}    

bool
policy_field_mapping::check_field_availability(msg::mnslp_field &field)
{
	msg::mnslp_field_key *key = field.get_field_key();
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) 
	{
		if (*key == *(i->first)){
			delete key;
			return true;
		}
	}
	delete key;
	return false;
}

bool
policy_field_mapping::check_field_availability(msg::mnslp_field &field) const
{
	msg::mnslp_field_key *key = field.get_field_key();
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ )
	{
		if (*key == *(i->first)){
			delete key;
			return true;
		}
	}
	delete key;
	return false;
}

std::string
policy_field_mapping::get_field_traslate(msg::mnslp_field &field)
{
	std::string val_return = "";
	
	msg::mnslp_field_key *key = field.get_field_key();
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		if (*key == *(i->first)){
			delete key;
			return (i->second).get_field_traslate();
		}
	}
	delete key;
	return val_return;
}

std::string
policy_field_mapping::get_field_traslate(msg::mnslp_field &field) const
{
	std::string val_return = "";
	
	msg::mnslp_field_key *key = field.get_field_key();
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		if (*key == *(i->first)){
			delete key;
			return (i->second).get_field_traslate();
		}
	}
	delete key;
	return val_return;
}

bool
policy_field_mapping::is_configuration_field(msg::mnslp_field &field) const
{
	bool val_return = false;
	
	msg::mnslp_field_key *key = field.get_field_key();
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		if (*key == *(i->first)){
			delete key;
			return (i->second).is_configuration();
		}
	}
	delete key;
	return val_return;
}

bool
policy_field_mapping::is_filter_field(msg::mnslp_field &field) const
{
	bool val_return = false;
	
	msg::mnslp_field_key *key = field.get_field_key();
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		if (*key == *(i->first)){
			delete key;
			return (i->second).is_filter();
		}
	}
	delete key;
	return val_return;
}

bool
policy_field_mapping::is_export_field(msg::mnslp_field &field) const
{
	bool val_return = false;
	msg::mnslp_field_key *key = field.get_field_key();
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		if (*key == *(i->first)){
			delete key;
			return (i->second).is_export();
		}
	}
	delete key;
	return val_return;
}
   
} // namespace mnslp
