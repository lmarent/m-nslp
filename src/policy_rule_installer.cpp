/// ----------------------------------------*- mode: C++; -*--
/// @file policy_rule_installer.cpp
/// The policy_rule_installer class.
/// ----------------------------------------------------------
/// $Id: policy_rule_installer.cpp 2558 2015-01-16 11:32:00 amarentes $
/// $HeadURL: https://./src/policy_rule_installer.cpp $
// ===========================================================
//                      
// Copyright (C) 2005-2007, all rights reserved by
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

#include "policy_rule_installer.h"
#include "msg/information_code.h"


namespace mnslp {

policy_rule_installer::~policy_rule_installer() throw()
{
	if (action_container != NULL)
		delete action_container;
	
	if (app_container != NULL)
		delete app_container;
}

void
policy_rule_installer::setup() throw (policy_rule_installer_error)
{
	
	if ( config->is_ms_meter()){
		
		try
		{
			// Parse and load the filter configuration.
			parse_configuration_definition_file(config->get_configuration_file().c_str());
			
			// Parse and load the export configuration.
			parse_export_definition_file(config->get_export_config_file().c_str());
		}
		catch(policy_rule_installer_error &e)
		{
			std::cout << "Node cannot be configured as metering" << std::endl;
			config->setpar(mnslpconf_ms_is_meter, false);
		}
			
	}
}

void policy_rule_installer::parse_export_definition_file(const char *filename) 
		throw (policy_rule_installer_error)
{

    xmlTextReaderPtr reader;
    int ret;

    /*
     * Pass some special parsing options to activate DTD attribute defaulting,
     * entities substitution and DTD validation
     */
    reader = xmlReaderForFile(filename, NULL,
                 XML_PARSE_DTDATTR |  /* default DTD attributes */
				 XML_PARSE_NOENT |    /* substitute entities */
				 XML_PARSE_DTDVALID); /* validate with the DTD */

    if (reader != NULL){
		action_container = new policy_action_container();
		action_container->read_from_xml(reader);
	}
	else{
		throw policy_rule_installer_error("Export configuration file does not open",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);
	}
	
    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
	
}

void policy_rule_installer::parse_configuration_definition_file(const char *filename) 
		throw (policy_rule_installer_error)
{

    xmlTextReaderPtr reader;
    int ret;

    /*
     * Pass some special parsing options to activate DTD attribute defaulting,
     * entities substitution and DTD validation
     */
    reader = xmlReaderForFile(filename, NULL,
                 XML_PARSE_DTDATTR |  /* default DTD attributes */
				 XML_PARSE_NOENT |    /* substitute entities */
				 XML_PARSE_DTDVALID); /* validate with the DTD */

    if (reader != NULL){
		app_container = new policy_application_configuration_container();
		app_container->read_from_xml(reader);
		
		policy_application_configuration_container::const_iterator it;
		for (it = app_container->begin(); it != app_container->end(); it++)
			std::cout << "number of applications:" << std::endl;
		 
	}
	else{
		throw policy_rule_installer_error("Export configuration file does not open",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);
	}
	
}

const policy_action_container *
policy_rule_installer::get_action_container() const
{
	if (action_container != NULL)
		return action_container;
	else
		throw policy_rule_installer_error("action configuration not defined",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);
}

const policy_application_configuration_container *
policy_rule_installer::get_application_configuration_container() const
{
	if (app_container != NULL)
		return app_container;
	else
		throw policy_rule_installer_error("filter configuration not defined",
			msg::information_code::sc_permanent_failure,
			msg::information_code::fail_configuration_failed);
}

	
} // mnslp
