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


policy_field_mapping::policy_field_mapping()
{
	// NOTHING TO DO.
}


void policy_field_mapping::processNode(int level, xmlTextReaderPtr reader) 
{
    int ret;
    int eno = 0;
    int field_type = 0;
    std::string name_field;
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
			
			// Bring the definition of field from the container.
			msg::mnslp_ipfix_field_key *key = new msg::mnslp_ipfix_field_key(eno,field_type); 

			// Bring the mapping between el ipfix field container and the
			// metering application.
			field_map = processTextNode(level + 1, reader);

			set_field(key,field_map);
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
                
        // Read Export_field, proc_name and priority.
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
	fields = rhs.fields;
}

	
policy_field_mapping::~policy_field_mapping()
{

	std::map<msg::mnslp_field_key *, std::string>::iterator i;
	for ( i = fields.begin(); i != fields.end(); i++ ) {
		delete i->first;
		fields.erase(i);
	}

}
        
void policy_field_mapping::set_field(msg::mnslp_field_key *_ipfixfield, 
									 std::string idFieldMetering)
{
	fields[_ipfixfield] = idFieldMetering;
}

std::string 
policy_field_mapping::get_field(msg::mnslp_field_key *_ipfixfield) const
{
	std::string str;
	const_iterator i = fields.find(_ipfixfield);
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		if (*(i->first) == *_ipfixfield)
			return i->second;
	}
	
	// field not found
	return str;
}
        
bool
policy_field_mapping::operator==(const policy_field_mapping &rhs) const
{
	
	// All entries have to be identical.
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		const std::string str = i->second;
		if ( str.compare(rhs.get_field(i->first)) != 0)
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
		out << "Key:" << (i->first)->to_string() << " Value:" << i->second << std::endl;
	}
	
}    

std::string
policy_field_mapping::to_string()
{
	ostringstream temp;

	// Print all entries.
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
		temp << "Key:" << (i->first)->to_string() << " Value:" << i->second << std::endl;
	}
	
	return  temp.str();
}    

bool
policy_field_mapping::check_field_availability(msg::mnslp_field &field)
{
	msg::mnslp_field_key *key = field.get_field_key();
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
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
	for ( const_iterator i = fields.begin(); i != fields.end(); i++ ) {
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
			return i->second;
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
			return i->second;
		}
	}
	delete key;
	return val_return;
}

   
} // namespace mnslp
