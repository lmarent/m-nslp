/// ----------------------------------------*- mode: C++; -*--
/// @file policy_action.cpp
/// The policy_action class.
/// ----------------------------------------------------------
/// $Id: policy_action.h 2558 2015-01-16 13:49:00 amarentes $
/// $HeadURL: https://./src/policy_action.cpp $
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

#include "policy_action_mapping.h"
#include "msg/information_code.h"
#include "policy_rule_installer.h"
#include <sstream>


namespace mnslp {


std::string policy_action_mapping::export_field_str = "EXPORT_FIELD";
std::string policy_action_mapping::proc_name_str = "PROC_NAME";
std::string policy_action_mapping::priority_str = "PRIORITY";
std::string policy_action_mapping::eno_str = "ENO";
std::string policy_action_mapping::field_type_str = "FIELDTYPE";
std::string policy_action_mapping::field_name_str = "NAME";


policy_action_mapping::policy_action_mapping()
{
	// NOTHING TO DO.
}


void policy_action_mapping::processNode(int level, xmlTextReaderPtr reader) 
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
						 policy_action_mapping::export_field_str.c_str()) == 1 )
		{
			eno = atoi( (processAttribute( reader, 
							policy_action_mapping::eno_str)).c_str());
							
			field_type = atoi( (processAttribute( reader, 
							policy_action_mapping::field_type_str)).c_str()); 
							
			name_field = processAttribute( reader, 
							policy_action_mapping::field_name_str);
			
			// Bring the definition of field from the container.
			msg::mnslp_ipfix_field_key key(eno,field_type); 

			// Bring the mapping between el ipfix field container and the
			// metering application.
			field_map = processTextNode(level + 1, reader);

			set_export_field(key,field_map);
		}
		else
		{ 
			
			if (xmlStrEqual( name, 
			    (const xmlChar *) policy_action_mapping::proc_name_str.c_str()) == 1 )
			{
				set_proc_name(processTextNode(level + 1, reader));
			}
			else if (xmlStrEqual( name, 
				 (const xmlChar *) policy_action_mapping::priority_str.c_str()) == 1 )
			{
				set_priority(atoi((processTextNode(level + 1, reader)).c_str()));
			}
		}
		xmlFree(name);
	}
	 
}

int policy_action_mapping::read_from_xml(xmlTextReaderPtr reader)
{
	int ret;
	std::string metering_app;
	const xmlChar *name, *value;
	int level;
		
	// Level 0 is the whole set of rules
	// Level 1 corresponds to action objects.
	// Verifies that the pointer's level is 2.
	if (xmlTextReaderDepth(reader) == 2)
	{
        // Read the metering application.
        metering_app = processAttribute(reader, "ID");
        metering_application = metering_app;
                
        // Read Export_field, proc_name and priority.
        ret = xmlTextReaderRead(reader);
		level = 3;
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

policy_action_mapping::policy_action_mapping(const policy_action_mapping &rhs)
{
	export_fields = rhs.export_fields;
	priority = rhs.priority;
	metering_application = rhs.metering_application;
	proc_name = rhs.proc_name;

}

	
policy_action_mapping::~policy_action_mapping()
{
	// NOTHING TO DO.
}

void policy_action_mapping::set_proc_name(std::string _proc_name)
{
	proc_name = _proc_name;
}
    
void policy_action_mapping::set_priority(int _priority)
{
	priority = _priority;
}
    
void policy_action_mapping::set_export_field(msg::mnslp_ipfix_field_key _ipfixfield, 
											 std::string idFieldMetering)
{
	export_fields[_ipfixfield] = idFieldMetering;
}

std::string 
policy_action_mapping::get_field_metering(msg::mnslp_ipfix_field_key _ipfixfield) const
{
	std::string str;
	const_iterator i = export_fields.find(_ipfixfield);
	
	if ( i != export_fields.end() )
		return i->second;
	else
		return str;
}
    
std::string 
policy_action_mapping::get_proc_name()
{
	return proc_name;
}
    
int 
policy_action_mapping::get_priority()
{
	return priority;
}

std::string
policy_action_mapping::get_application()
{
	return metering_application;
}

bool
policy_action_mapping::operator==(const policy_action_mapping &rhs) const
{
	
	if (priority != rhs.priority)
		return false;
		
	if (metering_application.compare(rhs.metering_application))
		return false;
		
	if (proc_name.compare(rhs.proc_name)) 
		return false;

	// All entries have to be identical.
	for ( const_iterator i = export_fields.begin(); i != export_fields.end(); i++ ) {
		msg::mnslp_ipfix_field_key key = i->first; 
		const std::string str = i->second;
		if ( str.compare(rhs.get_field_metering(key)) != 0)
			return false;
	}
	
	return true;

}

bool
policy_action_mapping::operator!=(const policy_action_mapping &rhs) const
{
	return ! operator==(rhs);
}

std::ostream &
policy_action_mapping::operator<<(std::ostream &out)
{
	out << "proc_name:" << proc_name << " metering_application:" 
		<< metering_application << " priority:" << priority << std::endl;

	// Print all entries.
	for ( const_iterator i = export_fields.begin(); i != export_fields.end(); i++ ) {
		msg::mnslp_ipfix_field_key key = i->first; 
		out << "Key:" << key.to_string() << " Value:" << i->second << std::endl;
	}
	
}    

std::string
policy_action_mapping::to_string()
{
	ostringstream temp;
	temp << "proc_name:" << proc_name << " metering_application:" 
		<< metering_application << " priority:" << priority << std::endl;

	// Print all entries.
	for ( const_iterator i = export_fields.begin(); i != export_fields.end(); i++ ) {
		msg::mnslp_ipfix_field_key key = i->first; 
		temp << "Key:" << key.to_string() << " Value:" << i->second << std::endl;
	}
	
	return  temp.str();
}    

   
} // namespace mnslp
