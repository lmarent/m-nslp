/// ----------------------------------------*- mode: C++; -*--
/// @file policy_action.h
/// The policy_action class.
/// ----------------------------------------------------------
/// $Id: policy_action.h 2558 2015-01-16 13:34:00 amarentes $
/// $HeadURL: https://./include/policy_action.h $
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
#ifndef MNSLP__POLICY_ACTION_MAPPING_H
#define MNSLP__POLICY_ACTION_MAPPING_H

#include <map>
#include <string>
#include <libxml/xmlreader.h>

#include "msg/mnslp_ipfix_field.h"
#include "mnslp_xml_node_reader.h"

namespace mnslp {


/**
 * This class helps to maintain the mapping configuration for the export 
 * and action processes.
 *
 */
class policy_action_mapping : public mnslp_xml_node_reader {

  public:
	
	policy_action_mapping();
		
	policy_action_mapping(const policy_action_mapping &rhs);
	
	virtual ~policy_action_mapping();

	/**
	 * This function assumes that the pointer is at the beginning of a 
	 * policy action mapping. When the function ends the pointer can be
	 * at the beginning of the next object or at the end of the document
	 */ 
	int read_from_xml(xmlTextReaderPtr node);

    void set_proc_name(std::string proc_name);
    
    void set_priority(int priority);
    
    void set_export_field(msg::mnslp_ipfix_field_key ipfixfield, 
						  std::string );
    
    std::string get_field_metering(msg::mnslp_ipfix_field_key _ipfixfield) const;
    
    std::string get_proc_name();
    
    int get_priority();
    
    std::string get_application();

    bool operator==(const policy_action_mapping &rhs) const;
    
    bool operator!=(const policy_action_mapping &rhs) const;
    
	typedef std::map<msg::mnslp_ipfix_field_key, std::string>::const_iterator const_iterator;
	const_iterator begin() const throw() { return export_fields.begin(); }
	const_iterator end() const throw() { return export_fields.end(); }
	
	std::ostream &operator<<(std::ostream &out);
	std::string to_string();
    
  private:
	std::map<msg::mnslp_ipfix_field_key, std::string> export_fields;
	int priority;
	std::string metering_application;
	std::string proc_name;
	
	void processNode(int level, xmlTextReaderPtr reader); 
	
    static std::string export_field_str;
    static std::string proc_name_str;
    static std::string priority_str;
    static std::string eno_str;
    static std::string field_type_str;
    static std::string field_name_str;
	
	
};



} // namespace mnslp

#endif // MNSLP__POLICY_ACTION_MAPPING_H
