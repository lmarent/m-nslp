/// ----------------------------------------*- mode: C++; -*--
/// @file netmate_ipfix_policy_action_mapping.h
/// The netmate_ipfix_policy_action_mapping class.
/// ----------------------------------------------------------
/// $Id: netmate_ipfix_policy_action_mapping.h 2558 2015-01-16 13:34:00 amarentes $
/// $HeadURL: https://./include/netmate_ipfix_policy_action_mapping.h $
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
#ifndef MNSLP__NETMATE_IPFIX_POLICY_ACTION_MAPPING_H
#define MNSLP__NETMATE_IPFIX_POLICY_ACTION_MAPPING_H

#include <map>
#include <string>
#include <libxml/xmlreader.h>

#include "msg/mnslp_field.h"
#include "mnslp_xml_node_reader.h"
#include "policy_field_mapping.h"
#include "policy_action_mapping.h"
#include "netmate_ipfix_metering_config.h"


namespace mnslp {



/**
 * This class helps to maintain the mapping configuration for the export 
 * and action processes.
 *
 */
class netmate_ipfix_policy_action_mapping : public policy_action_mapping {


  public:
	
	netmate_ipfix_policy_action_mapping();
		
	netmate_ipfix_policy_action_mapping(const netmate_ipfix_policy_action_mapping &rhs);
	
	virtual ~netmate_ipfix_policy_action_mapping();
	
	virtual netmate_ipfix_policy_action_mapping *copy() const;
	
	virtual int read_from_xml(int level, xmlTextReaderPtr node);
	
	virtual std::string get_key() const;
		    
    static std::string metering_proc_name_str;
    static std::string priority_str;
    static std::string export_proc_name_str;
    static std::string export_directory_str;
    static std::string export_report_interval_str;
  
  protected:
	
	void processNode(int level, xmlTextReaderPtr reader, 
					 netmate_ipfix_metering_config *meter_conf); 
			
};



} // namespace mnslp

#endif // MNSLP__POLICY_ACTION_MAPPING_H
