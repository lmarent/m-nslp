/// ----------------------------------------*- mode: C++; -*--
/// @file policy_filter_mapping.cpp
/// The policy_filter_mapping class.
/// ----------------------------------------------------------
/// $Id: policy_filter_mapping.h 2558 2015-01-16 13:49:00 amarentes $
/// $HeadURL: https://./src/policy_filter_mapping.cpp $
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

#include "policy_filter_mapping.h"
#include "msg/information_code.h"
#include "policy_rule_installer.h"
#include <sstream>


namespace mnslp {


std::string policy_filter_mapping::filter_field_str = "FILTER_FIELD";
std::string policy_filter_mapping::eno_str = "ENO";
std::string policy_filter_mapping::field_type_str = "FIELDTYPE";
std::string policy_filter_mapping::field_name_str = "NAME";


policy_filter_mapping::policy_filter_mapping()
{
	// NOTHING TO DO.
}


void policy_filter_mapping::processNode(int level, xmlTextReaderPtr reader) 
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
						 policy_filter_mapping::filter_field_str.c_str()) == 1 )
		{
			eno = atoi( (processAttribute( reader, 
							policy_filter_mapping::eno_str)).c_str());
							
			field_type = atoi( (processAttribute( reader, 
							policy_filter_mapping::field_type_str)).c_str()); 
							
			name_field = processAttribute( reader, 
							policy_filter_mapping::field_name_str);
			
			// Bring the definition of field from the container.
			msg::mnslp_ipfix_field_key key(eno,field_type); 

			// Bring the mapping between el ipfix field container and the
			// metering application.
			field_map = processTextNode(level + 1, reader);

			set_filter_field(key,field_map);
		}
		xmlFree(name);
	}
	 
}

int policy_filter_mapping::read_from_xml(xmlTextReaderPtr reader)
{
	int ret;
	std::string metering_app;
	const xmlChar *name, *value;
	int level;
		
	// Level 0 is the whole set of rules
	// Level 1 corresponds to action objects.
	// Verifies that the pointer's level is 2.
	level = 1;
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
		throw policy_rule_installer_error("Export configuration file does not parse",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);	
	}
}

policy_filter_mapping::policy_filter_mapping(const policy_filter_mapping &rhs)
{
	filter_fields = rhs.filter_fields;
	metering_application = rhs.metering_application;

}

	
policy_filter_mapping::~policy_filter_mapping()
{
	// NOTHING TO DO.
}

    
    
void policy_filter_mapping::set_filter_field(msg::mnslp_ipfix_field_key _ipfixfield, 
												std::string idFieldMetering)
{
	filter_fields[_ipfixfield] = idFieldMetering;
}

std::string 
policy_filter_mapping::get_field_metering(msg::mnslp_ipfix_field_key _ipfixfield) const
{
	std::string str;
	const_iterator i = filter_fields.find(_ipfixfield);
	
	if ( i != filter_fields.end() )
		return i->second;
	else
		return str;
}
    
std::string
policy_filter_mapping::get_application()
{
	return metering_application;
}

bool
policy_filter_mapping::operator==(const policy_filter_mapping &rhs) const
{
			
	if (metering_application.compare(rhs.metering_application))
		return false;
		
	// All entries have to be identical.
	for ( const_iterator i = filter_fields.begin(); i != filter_fields.end(); i++ ) {
		msg::mnslp_ipfix_field_key key = i->first; 
		const std::string str = i->second;
		if ( str.compare(rhs.get_field_metering(key)) != 0)
			return false;
	}
	
	return true;

}

bool
policy_filter_mapping::operator!=(const policy_filter_mapping &rhs) const
{
	return ! operator==(rhs);
}

std::ostream &
policy_filter_mapping::operator<<(std::ostream &out)
{
	out << " metering_application:" 
		<< metering_application << std::endl;

	// Print all entries.
	for ( const_iterator i = filter_fields.begin(); i != filter_fields.end(); i++ ) {
		msg::mnslp_ipfix_field_key key = i->first; 
		out << "Key:" << key.to_string() << " Value:" << i->second << std::endl;
	}
	
}    

std::string
policy_filter_mapping::to_string()
{
	ostringstream temp;
	temp << " metering_application:" << metering_application << std::endl;

	// Print all entries.
	for ( const_iterator i = filter_fields.begin(); i != filter_fields.end(); i++ ) {
		msg::mnslp_ipfix_field_key key = i->first; 
		temp << "Key:" << key.to_string() << " Value:" << i->second << std::endl;
	}
	
	return  temp.str();
}    

   
} // namespace mnslp
