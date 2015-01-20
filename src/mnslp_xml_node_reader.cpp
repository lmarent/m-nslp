/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_xml_node_reader.cpp
/// The mnslp_xml_node_reader class.
/// ----------------------------------------------------------
/// $Id: mnslp_xml_node_reader.h 2558 2015-01-16 13:49:00 amarentes $
/// $HeadURL: https://./src/mnslp_xml_node_reader.cpp $
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

#include "mnslp_xml_node_reader.h"
#include "msg/information_code.h"
#include "policy_rule_installer.h"


namespace mnslp {

std::string 
mnslp_xml_node_reader::processTextNode(int level,  xmlTextReaderPtr reader)
{
    int ret;
    xmlChar *name, *value;
    
	// remove blank lines that the user could introduce.
	ret = xmlTextReaderRead(reader);
	while (ret == 1)
	{
		if ((xmlTextReaderDepth(reader) >= level) 
		     and (xmlTextReaderNodeType(reader) == 3) )
		{
			// We are in a node of type text and with the level required.
			value = xmlTextReaderValue(reader);
			if (value){
				std::string val_return(reinterpret_cast<char*>(value)) ;
				xmlFree(value);
				return val_return;
			}
		}
		
		if (xmlTextReaderDepth(reader) < level)
			// parse error, we expect a text here.
			throw policy_rule_installer_error("Export configuration file does not parse",
				msg::information_code::sc_permanent_failure,
				msg::information_code::fail_configuration_failed);	
		else
			ret = xmlTextReaderRead(reader);

	}

	// Something goes wrong and could not find the text node.
	throw policy_rule_installer_error("Configuration file does not parse",
				msg::information_code::sc_permanent_failure,
				msg::information_code::fail_configuration_failed);	
} 

std::string mnslp_xml_node_reader::processAttribute(xmlTextReaderPtr reader, 
											std::string atrribute)
{
    xmlChar *attrVal;

	attrVal = xmlTextReaderGetAttribute(reader, (xmlChar *) atrribute.c_str());    

    if (attrVal){
	   std::string val_return(reinterpret_cast<char*>(attrVal)) ;
	   xmlFree(attrVal);
	   return val_return;
	}
	throw policy_rule_installer_error("Configuration file does not parse",
				msg::information_code::sc_permanent_failure,
				msg::information_code::fail_configuration_failed);	
}


   
} // namespace mnslp
