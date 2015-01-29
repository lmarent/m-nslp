/// ----------------------------------------*- mode: C++; -*--
/// @file policy_action_container.cpp
/// The policy_action_container class.
/// ----------------------------------------------------------
/// $Id: policy_action_container.cpp 2558 2015-01-16 15:15:00 amarentes $
/// $HeadURL: https://./include/policy_action_container.cpp $
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

#include <new>          // std::nothrow
#include "policy_action_container.h"
#include "msg/information_code.h"
#include "policy_rule_installer.h"


namespace mnslp {


policy_action_container::policy_action_container()
{
	// Nothing to do.
}
		
policy_action_container::policy_action_container(const policy_action_container &rhs)
{
	actions = rhs.actions;
}
	
policy_action_container::~policy_action_container()
{
	// Nothing to do.
}

void 
policy_action_container::read_from_xml(xmlTextReaderPtr reader)
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
			policy_action action;
			ret = action.read_from_xml(reader);
			set_policy_action(action.get_action(), action);
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
policy_action_container::set_policy_action(std::string key, policy_action &_action)
{
	std::cout << "size found" << _action.get_number_mappings() << std::endl;
	actions[key] = _action;
}
	
bool 
policy_action_container::operator==(const policy_action_container &rhs) const
{
	// All entries have to be identical.
	for ( const_iterator i = actions.begin(); i != actions.end(); i++ ) {
		const std::string key = i->first; 
		
		const_iterator j = rhs.actions.find(key);
		if ( j != rhs.actions.end() ){
			if ( i->second != j->second)
				return false;
		}
		else
			return false;
	}	
	
	return true;

}
	
bool 
policy_action_container::operator!=(const policy_action_container &rhs) const
{
	return !(operator==(rhs));
}    

bool 
policy_action_container::check_field_availability(std::string app, 
												  msg::mnslp_field &field) const
{

	std::vector< std::pair<int, std::string> > map_to_be_ordered;
	// Do the search using the priority defined.
	for ( const_iterator i = actions.begin(); i != actions.end(); i++ ) {
		std::pair<int, std::string> pair_tmp((i->second).get_priority(), i->first);
		map_to_be_ordered.push_back( pair_tmp);
	}
	
	std::sort (map_to_be_ordered.begin(), map_to_be_ordered.end());
	
	for (std::vector< std::pair<int, std::string> >::iterator it=map_to_be_ordered.begin(); 
				it!=map_to_be_ordered.end(); ++it){

		const_iterator it_map;
		it_map = actions.find((*it).second);
		
		if ((it_map->second).check_field_availability(app,field) )
			return true;
		
	}
	
	return false;
	
}

std::string 
policy_action_container::get_field_traslate( std::string app, msg::mnslp_field &field ) const
{
	
	std::string val_return = "";

	std::vector< std::pair<int, std::string> > map_to_be_ordered;
	// Do the search using the priority defined.
	for ( const_iterator i = actions.begin(); i != actions.end(); i++ ) {
		std::pair<int, std::string> pair_tmp((i->second).get_priority(), i->first);
		map_to_be_ordered.push_back( pair_tmp);
	}
	
	std::sort (map_to_be_ordered.begin(), map_to_be_ordered.end());
	
	for (std::vector< std::pair<int, std::string> >::iterator it=map_to_be_ordered.begin(); 
				it!=map_to_be_ordered.end(); ++it){

		const_iterator it_map;
		it_map = actions.find((*it).second);
		
		val_return = ((it_map->second).get_field_traslate(app,field) );
		if (val_return.length() > 0)
			break;
		
	}
	
	return val_return;
}

const metering_config *
policy_action_container::get_package( std::string app, msg::mnslp_field &field ) const
{
	
	const metering_config *val_return = NULL;
	
	std::vector< std::pair<int, std::string> > map_to_be_ordered;
	// Do the search using the priority defined.
	for ( const_iterator i = actions.begin(); i != actions.end(); i++ ) {
		std::pair<int, std::string> pair_tmp((i->second).get_priority(), i->first);
		map_to_be_ordered.push_back( pair_tmp);
	}
	
	std::sort (map_to_be_ordered.begin(), map_to_be_ordered.end());
	
	for (std::vector< std::pair<int, std::string> >::iterator it=map_to_be_ordered.begin(); 
				it!=map_to_be_ordered.end(); ++it){

		const_iterator it_map;
		it_map = actions.find((*it).second);
		
		val_return = ((it_map->second).get_package(app,field) );
		if (val_return != NULL)
			break;
		
	}
	
	return val_return;
}
	
} // namespace mnslp
