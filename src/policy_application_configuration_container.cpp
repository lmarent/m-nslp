/// ----------------------------------------*- mode: C++; -*--
/// @file policy_application_configuration_container.cpp
/// The policy_application_configuration_container class.
/// ----------------------------------------------------------
/// $Id: policy_application_configuration_container.cpp 2558 2015-01-16 15:15:00 amarentes $
/// $HeadURL: https://./include/policy_application_configuration_container.cpp $
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

#include "msg/information_code.h"
#include "policy_rule_installer.h"
#include "policy_application_configuration_container.h"


namespace mnslp {


policy_application_configuration_container::policy_application_configuration_container()
{
	// Nothing to do.
}
		
policy_application_configuration_container::policy_application_configuration_container(const policy_application_configuration_container &rhs)
{
	applications = rhs.applications;
}
	
policy_application_configuration_container::~policy_application_configuration_container()
{
	// Nothing to do.
}

void
policy_application_configuration_container::read_from_xml(xmlTextReaderPtr reader)
{
	int ret;
	int count = 0;
	const xmlChar *name, *value;
	
    // Read an application mapping
    ret = xmlTextReaderRead(reader);    
    while (ret == 1) 
    {
		if ((xmlTextReaderDepth(reader) == 1) and 
		      (xmlTextReaderNodeType(reader) == 1) )
		{
			int level = 1;
			policy_field_mapping mapping;
			ret = mapping.read_from_xml(level, reader);
			set_application(mapping.get_metering_application(), mapping);
		}
		else{
			ret = xmlTextReaderRead(reader);
		}
    }

	/*
	* Once the document has been fully parsed check the validation results
	*/
	if (xmlTextReaderIsValid(reader) != 1)
		throw policy_rule_installer_error("Export configuration file does not validate",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);
	
	xmlFreeTextReader(reader);
        
    if (ret != 0)
		throw policy_rule_installer_error("Export configuration file does not parse",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);
	
}
	
void 
policy_application_configuration_container::set_application(std::string key, policy_field_mapping &_mapping)
{
	applications.insert(std::pair<std::string, policy_field_mapping>(key,_mapping));
}
	
bool 
policy_application_configuration_container::operator==(const policy_application_configuration_container &rhs) const
{
	// All entries have to be identical.
	for ( const_iterator i = applications.begin(); i != applications.end(); i++ ) {
		const std::string key = i->first; 
		
		const_iterator j = rhs.applications.find(key);
		if ( j != rhs.applications.end() ){
			if ( i->second != j->second)
				return false;
		}
		else
			return false;
	}	
	
	return true;

}
	
bool 
policy_application_configuration_container::operator!=(const policy_application_configuration_container &rhs) const
{
	return !(operator==(rhs));
}    

bool
policy_application_configuration_container::check_field_availability(std::string app, 
						msg::mnslp_field &field) const
{

	// Look for the application and then the field on those mapping.
	for ( const_iterator i = applications.begin(); i != applications.end(); i++ ) {
				
		if ( app.compare(i->first) == 0 )
			return (i->second).check_field_availability(field);
	}	
	
	return false;

}


std::string
policy_application_configuration_container::get_field_traslate(std::string app, 
						msg::mnslp_field &field) const
{

	std::string val_return = "";
	// Look for the application and then the field on those mapping.
	for ( const_iterator i = applications.begin(); i != applications.end(); i++ ) {
				
		if ( app.compare(i->first) == 0 )
			return (i->second).get_field_traslate(field);
	}	
	
	return val_return;

}

bool
policy_application_configuration_container::is_configuration_field(std::string app, 
						msg::mnslp_field &field) const
{

	bool val_return = false;
	// Look for the application and then the field on those mapping.
	for ( const_iterator i = applications.begin(); i != applications.end(); i++ ) {
				
		if ( app.compare(i->first) == 0 )
			return (i->second).is_configuration_field(field);
	}	
	
	return val_return;

}

bool
policy_application_configuration_container::is_filter_field(std::string app, 
						msg::mnslp_field &field) const
{

	bool val_return = false;
	// Look for the application and then the field on those mapping.
	for ( const_iterator i = applications.begin(); i != applications.end(); i++ ) {
				
		if ( app.compare(i->first) == 0 )
			return (i->second).is_filter_field(field);
	}	
	
	return val_return;
}

bool
policy_application_configuration_container::is_export_field(std::string app, 
						msg::mnslp_field &field) const
{

	std::cout << "in is_export_field" << app << std::endl;
	bool val_return = false;
	// Look for the application and then the field on those mapping.
	std::cout << "number of applications:" << applications.size() << std::endl;
	for ( const_iterator i = applications.begin(); i != applications.end(); i++ ) {
		std::cout << "app:" << i->first << std::endl;
		if ( app.compare(i->first) == 0 ){
			std::cout << "we found the application for ask for export field" << std::endl;
			return (i->second).is_export_field(field);
		}
	}	
	
	return val_return;
}

std::string
policy_application_configuration_container::to_string()
{
    std::string val_return;
	for ( const_iterator i = applications.begin(); i != applications.end(); i++ ) {
		val_return.append("\n");
		val_return.append(i->first);
		val_return.append(":");
		val_return.append((i->second).to_string());
	}
}

	
} // namespace mnslp
