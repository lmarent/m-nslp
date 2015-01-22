/// ----------------------------------------*- mode: C++; -*--
/// @file policy_action.cpp
/// The policy_action class.
/// ----------------------------------------------------------
/// $Id: policy_action.cpp 2558 2015-01-16 15:50:00 amarentes $
/// $HeadURL: https://./include/policy_action.cpp $
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

#include "policy_action.h"
#include <libxml/xmlreader.h>
#include <iostream>
#include "msg/information_code.h"
#include "policy_rule_installer.h"


namespace mnslp {

std::string policy_action::id_str = "ID";


policy_action::policy_action()
{
	// Nothing to do.
}
	
policy_action::policy_action(const policy_action &rhs)
{
	action = rhs.action;
	action_mappings = rhs.action_mappings;
}
	
policy_action::~policy_action()
{
	// Nothing to do.
}

int policy_action::read_from_xml(xmlTextReaderPtr reader)
{

	int ret;
	xmlChar *attribVal = NULL; 
	const xmlChar *name, *value;
	
	// Level 0 is the whole set of rules
	// Level 1 corresponds to action objects.
	int level = 1;
	if (xmlTextReaderDepth(reader) == level)
	{
        
        // Read the metering application.
        action = processAttribute( reader, policy_action::id_str );
		
		// Read an application mapping
		ret = xmlTextReaderRead(reader);
		
		level++;
		while (ret == 1) 
		{
			if ((xmlTextReaderDepth(reader) == level) and 
				(xmlTextReaderNodeType(reader) == 1) )
			{
				policy_action_mapping action_mapping;
				ret = action_mapping.read_from_xml(level, reader);
				set_action_mapping(action_mapping.get_application(), 
							   action_mapping);
					
			}
			else{
				if (xmlTextReaderDepth(reader) < level){
					return ret;
				}
				else{
					ret = xmlTextReaderRead(reader);
				}
			}
		}		
	}
	else
	{
		std::cout << "level found: " << xmlTextReaderDepth(reader) << std::endl;
		throw policy_rule_installer_error("Export configuration file does not parse",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);		
	}
}

	
void
policy_action::set_action_mapping(std::string app, policy_action_mapping _mapping)
{
	action_mappings[app] = _mapping;
}

bool
policy_action::operator ==(const policy_action & rhs) const
{
	if ((action.compare(rhs.action)) != 0 )
		return false;
	
	// All entries have to be identical.
	for ( const_iterator i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		const std::string key = i->first; 
		
		const_iterator j = rhs.action_mappings.find(key);
		if ( j != rhs.action_mappings.end() ){
			if ( i->second != j->second)
				return false;
		}
		else
			return false;
	}	
	
	return true;
}		 

bool
policy_action::operator !=(const policy_action & rhs) const
{
	return !(operator==(rhs));
}

std::string 
policy_action::get_name() const
{
	return action;
}

bool 
policy_action::check_field_availability(std::string app, msg::mnslp_field &field) const
{
	for ( const_iterator i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		if (app.compare( (i->second).get_application() ) == 0) 
			return (i->second).check_field_availability(field);
	}
	return false;
}

std::string 
policy_action::get_field_traslate(std::string app, msg::mnslp_field &field) const
{
	std::string val_return = "";
	for ( const_iterator i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		if (app.compare( (i->second).get_application() ) == 0) 
			return (i->second).get_field_traslate(field);
	}
	return val_return;
}

std::string 
policy_action::get_package(std::string app, msg::mnslp_field &field) const
{
	std::string val_return = "";
	for ( const_iterator i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		if (app.compare( (i->second).get_application() ) == 0) 
			if ( (i->second).check_field_availability(field) )
				return (i->second).get_proc_name();
	}
	return val_return;
}

} // namespace mnslp
