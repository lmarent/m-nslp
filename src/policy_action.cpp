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

#include <libxml/xmlreader.h>
#include <iostream>
#include <vector>
#include <sstream>

#include "policy_action.h"
#include "msg/information_code.h"
#include "policy_rule_installer.h"


namespace mnslp {

std::string policy_action::id_str = "ID";
std::string policy_action::priority_str = "PRIORITY";
std::string policy_action::action_str = "POLICY_ACTION_APP";
std::string policy_action::directory_str = "DIRECTORY";
std::string policy_action::metering_procedure_str = "METERING_PROCEDURE";
std::string policy_action::export_procedure_str = "EXPORT_PROCEDURE";
std::string policy_action::interval_str = "INTERVAL";



policy_action::policy_action()
{
	// Nothing to do.
}
	
policy_action::policy_action(const policy_action &rhs)
{
	
	action = rhs.action;
	priority = rhs.priority;

	std::map<std::string, policy_action_mapping *>::const_iterator it;
	for ( it = rhs.action_mappings.begin(); it != rhs.action_mappings.end(); it++ )
	{
		const std::string id = it->first;
		const policy_action_mapping *obj = it->second;
		if (obj){
			set_action_mapping(id, obj->copy());
		}
	}
}

policy_action& 
policy_action::operator=(policy_action const &rhs)
{
	action = rhs.action;
	priority = rhs.priority;

	std::map<std::string, policy_action_mapping *>::const_iterator it;
	for ( it = rhs.action_mappings.begin(); it != rhs.action_mappings.end(); it++ )
	{
		const std::string id = it->first;
		const policy_action_mapping *obj = it->second;
		if (obj){
			set_action_mapping(id, obj->copy());
		}
	}
	return *this;
}
	
policy_action::~policy_action()
{
	std::map<std::string, policy_action_mapping *>::iterator i;
	for ( i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		delete i->second;
		action_mappings.erase(i);
	}

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
			name = xmlTextReaderName(reader);
			
			if ((xmlTextReaderDepth(reader) == level) and 
				(xmlTextReaderNodeType(reader) ==  XML_READER_TYPE_ELEMENT ) ){
					
				if (xmlStrEqual( name, 
						(const xmlChar *) policy_action::action_str.c_str()) == 1 )
				{

					std::string metering_application = 
						processAttribute(reader, policy_action::id_str);
						
					policy_action_mapping *action_mapping;
									
					action_mapping = policy_action_mapping::make(metering_application);
				
					action_mapping->read_from_xml(level, reader);
								
					set_action_mapping(action_mapping->get_metering_application(), 
							   action_mapping);
				}
				else if (xmlStrEqual( name, 
						(const xmlChar *) policy_action::priority_str.c_str()) == 1)
				{	
					set_priority(atoi((processTextNode(level + 1, reader)).c_str()));
				}	
			}
			else{
			
				if (xmlTextReaderDepth(reader) < level){
					return ret;
				}
				else
				{
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
policy_action::set_action_mapping(std::string app, policy_action_mapping *_mapping)
{

	if ( _mapping == NULL )
		return;

	policy_action_mapping *old = action_mappings[app];

	if ( old )
		delete old;

	action_mappings[app] = _mapping;
	
}

bool
policy_action::operator ==(const policy_action & rhs) const
{
	if ((action.compare(rhs.action)) != 0 )
		return false;
		
	if (priority != rhs.priority )
		return false;	
	
	// All entries have to be identical.
	for ( const_iterator i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		const std::string key = i->first; 
		
		const_iterator j = rhs.action_mappings.find(key);
		if ( j != rhs.action_mappings.end() ){
			if ( (i->second)->not_equal( *(j->second)))
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
policy_action::get_action() const
{
	return action;
}

int policy_action::get_priority() const
{
	return priority;
}

void 
policy_action::set_action(std::string _action)
{
	action = _action;
}

void 
policy_action::set_priority(int _priority)
{
	priority = _priority;
}

bool 
policy_action::check_field_availability(std::string app, msg::mnslp_field &field) const
{
	
	std::vector< std::pair<int, std::string> > map_to_be_ordered;
	
	// Do the search using the priority defined.
	for ( const_iterator i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		std::pair<int, std::string> pair_tmp((i->second)->get_priority(), i->first);
		map_to_be_ordered.push_back( pair_tmp);
	}
	
	std::sort (map_to_be_ordered.begin(), map_to_be_ordered.end());
	
	for (std::vector< std::pair<int, std::string> >::iterator it=map_to_be_ordered.begin(); 
				it!=map_to_be_ordered.end(); ++it){

		std::map<std::string, policy_action_mapping *>::const_iterator it_map;
		it_map = action_mappings.find((*it).second);
		
		if (app.compare( (it_map->second)->get_metering_application() ) == 0){
			return (it_map->second)->check_field_availability(field);
		}
	}
	return false;
}

std::string 
policy_action::get_field_traslate(std::string app, msg::mnslp_field &field) const
{
	std::string val_return = "";
	std::vector< std::pair<int, std::string> > map_to_be_ordered;
	// Do the search using the priority defined.
	for ( const_iterator i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		std::pair<int, std::string> pair_tmp((i->second)->get_priority(), i->first);
		map_to_be_ordered.push_back( pair_tmp);
	}
	
	std::sort (map_to_be_ordered.begin(), map_to_be_ordered.end());
	
	for (std::vector< std::pair<int, std::string> >::iterator it=map_to_be_ordered.begin(); 
				it!=map_to_be_ordered.end(); ++it){
						
		std::map<std::string, policy_action_mapping *>::const_iterator it_map;
		it_map = action_mappings.find((*it).second);
		
		if (app.compare( (it_map->second)->get_metering_application() ) == 0){
			return (it_map->second)->get_field_traslate(field);
		}
	}
	return val_return;
		
}

const metering_config *
policy_action::get_package(std::string app, msg::mnslp_field &field) const
{

	metering_config * val_return = NULL;
	std::vector< std::pair<int, std::string> > map_to_be_ordered;
	// Do the search using the priority defined.
	for ( const_iterator i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		std::pair<int, std::string> pair_tmp((i->second)->get_priority(), i->first);
		map_to_be_ordered.push_back(pair_tmp);
	}
	
	std::sort (map_to_be_ordered.begin(), map_to_be_ordered.end());
	
	for (std::vector< std::pair<int, std::string> >::iterator it=map_to_be_ordered.begin(); 
				it!=map_to_be_ordered.end(); ++it){

		std::map<std::string, policy_action_mapping *>::const_iterator it_map;
		it_map = action_mappings.find((*it).second);
		
		if (app.compare( (it_map->second)->get_metering_application() ) == 0){
			if ( (it_map->second)->check_field_availability(field) ){
				return (it_map->second)->get_metering_configuration();
			}
		}
	}
	return val_return;

}

std::string 
policy_action::to_string() const
{
	ostringstream temp;
	temp << "action:" << action << " priority:"  << priority << std::endl;
	for ( const_iterator i = action_mappings.begin(); i != action_mappings.end(); i++ ) {
		temp << (i->second)->to_string() << std::endl;
	}
	
	return  temp.str();

}

} // namespace mnslp
