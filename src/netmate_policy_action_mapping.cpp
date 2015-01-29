/// ----------------------------------------*- mode: C++; -*--
/// @file netmate_policy_action_mapping.cpp
/// The netmate_policy_action_mapping class.
/// ----------------------------------------------------------
/// $Id: netmate_policy_action_mapping.h 2558 2015-01-16 13:49:00 amarentes $
/// $HeadURL: https://./src/netmate_policy_action_mapping.cpp $
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

#include "netmate_policy_action_mapping.h"
#include "msg/information_code.h"
#include "policy_rule_installer.h"
#include <sstream>


namespace mnslp {


std::string netmate_policy_action_mapping::metering_proc_name_str = "METERING_PROCEDURE";
std::string netmate_policy_action_mapping::priority_str = "PRIORITY";
std::string netmate_policy_action_mapping::export_proc_name_str = "EXPORT_PROCEDURE";
std::string netmate_policy_action_mapping::export_directory_str = "DIRECTORY";
std::string netmate_policy_action_mapping::export_report_interval_str = "INTERVAL";



netmate_policy_action_mapping::netmate_policy_action_mapping():
	policy_action_mapping()
{
	// NOTHING TO DO.
}

netmate_policy_action_mapping::~netmate_policy_action_mapping()
{
	// NOTHING TO DO.
}

netmate_policy_action_mapping::netmate_policy_action_mapping(const netmate_policy_action_mapping &rhs):
  policy_action_mapping(rhs)
{
	if (meter_config != NULL){
		netmate_metering_config *meter_configuration = dynamic_cast< netmate_metering_config* >( meter_config );
		meter_config = meter_configuration->copy();
	}
	else {
		meter_config = NULL;
	}
}

netmate_policy_action_mapping *
netmate_policy_action_mapping::copy() const
{
	netmate_policy_action_mapping *q = NULL;
	q = new netmate_policy_action_mapping(*this);
	return q;
}

void netmate_policy_action_mapping::processNode(int level, xmlTextReaderPtr reader, 
						netmate_metering_config *meter_conf) 
{
    int ret;
    int eno = 0;
    int field_type = 0;
    std::string name_field;
    std::string field_map;
    
    xmlChar *name, *value;
    name = xmlTextReaderName(reader);
        
    if ((name) and (meter_conf != NULL)){
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

			set_field(key,
			  application_field_mapping("false","false","true",field_map));
		}
		else
		{ 
			
			if (xmlStrEqual( name, 
				 (const xmlChar *) netmate_policy_action_mapping::priority_str.c_str()) == 1 )
			{
				meter_conf->set_priority(atoi((processTextNode(level + 1, reader)).c_str()));
			}
			else if (xmlStrEqual( name, 
			    (const xmlChar *) netmate_policy_action_mapping::metering_proc_name_str.c_str()) == 1 )
			{
				meter_conf->set_metering_procedure(processTextNode(level + 1, reader));
			}
			else if (xmlStrEqual( name, 
				 (const xmlChar *) netmate_policy_action_mapping::export_proc_name_str.c_str()) == 1 )
			{
				meter_conf->set_export_procedure(processTextNode(level + 1, reader));
			}
			else if (xmlStrEqual( name, 
				 (const xmlChar *) netmate_policy_action_mapping::export_directory_str.c_str()) == 1 )
			{
				meter_conf->set_export_directory(processTextNode(level + 1, reader));
			}
			else if (xmlStrEqual( name, 
				 (const xmlChar *) netmate_policy_action_mapping::export_report_interval_str.c_str()) == 1 )
			{
				meter_conf->set_export_interval(atof((processTextNode(level + 1, reader)).c_str()));
			}
		}
		xmlFree(name);
	}
	else
	{
		throw policy_rule_installer_error("Error parsing the export configuration",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);	
	} 
}


int 
netmate_policy_action_mapping::read_from_xml(int level,  xmlTextReaderPtr reader)
{
	int ret;
	std::string metering_app;
	const xmlChar *name, *value;
	netmate_metering_config *meter_configuration = new netmate_metering_config();
		
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
				processNode(level, reader, meter_configuration);
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

std::string 
netmate_policy_action_mapping::get_key() const
{
	if (meter_config != NULL){
		netmate_metering_config *meter_configuration = dynamic_cast< netmate_metering_config* >( meter_config );
		if (meter_configuration != NULL)
		{
			return meter_configuration->get_metering_procedure();
		}
		else
		{
			throw policy_rule_installer_error("Error creating a key for the application mapping",
				msg::information_code::sc_permanent_failure,
				msg::information_code::fail_configuration_failed);	
		}
	}
	else{
		throw policy_rule_installer_error("Error creating a key for the application mapping",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);	
	}
		
}

   
} // namespace mnslp
