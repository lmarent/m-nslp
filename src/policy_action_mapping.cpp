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


std::string policy_action_mapping::proc_name_str = "PROC_NAME";
std::string policy_action_mapping::priority_str = "PRIORITY";


policy_action_mapping::policy_action_mapping():
	policy_field_mapping()
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

policy_action_mapping::policy_action_mapping(const policy_action_mapping &rhs)
{
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
        
std::string 
policy_action_mapping::get_proc_name() const
{
	return proc_name;
}
    
int 
policy_action_mapping::get_priority()
{
	return priority;
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

	return policy_field_mapping::operator ==(rhs);
	

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

	policy_field_mapping::operator<<(out);
}    

std::string
policy_action_mapping::to_string()
{
	ostringstream temp;
	temp << "proc_name:" << proc_name << " metering_application:" 
		<< metering_application << " priority:" << priority << std::endl;

	policy_field_mapping::operator<<(temp);	
	return  temp.str();
}    

   
} // namespace mnslp
