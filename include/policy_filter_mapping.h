/// ----------------------------------------*- mode: C++; -*--
/// @file policy_filter_mapping.h
/// The policy_filter_mapping class.
/// ----------------------------------------------------------
/// $Id: policy_action.h 2558 2015-01-16 13:34:00 amarentes $
/// $HeadURL: https://./include/policy_filter_mapping.h $
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
#ifndef MNSLP__POLICY_FILTER_MAPPING_H
#define MNSLP__POLICY_FILTER_MAPPING_H

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
class policy_filter_mapping : public mnslp_xml_node_reader {

  public:
	
	policy_filter_mapping();
		
	policy_filter_mapping(const policy_filter_mapping &rhs);
	
	virtual ~policy_filter_mapping();

	/**
	 * This function assumes that the pointer is at the beginning of a 
	 * policy filter mapping. When the function ends the pointer can be
	 * at the beginning of the next object or at the end of the document
	 */ 
	int read_from_xml(xmlTextReaderPtr node);
    
    void set_filter_field(msg::mnslp_ipfix_field_key ipfixfield, 
						  std::string );
    
    void set_application(std::string app);
    
    std::string get_field_metering(msg::mnslp_ipfix_field_key _ipfixfield) const;
         
    std::string get_application();

    bool operator==(const policy_filter_mapping &rhs) const;
    
    bool operator!=(const policy_filter_mapping &rhs) const;
    
	typedef std::map<msg::mnslp_ipfix_field_key, std::string>::const_iterator const_iterator;
	const_iterator begin() const throw() { return filter_fields.begin(); }
	const_iterator end() const throw() { return filter_fields.end(); }
	
	std::ostream &operator<<(std::ostream &out);
	std::string to_string();
    
  private:
	std::map<msg::mnslp_ipfix_field_key, std::string> filter_fields;
	std::string metering_application;
	
	void processNode(int level, xmlTextReaderPtr reader); 
	
    static std::string filter_field_str;
    static std::string eno_str;
    static std::string field_type_str;
    static std::string field_name_str;
	
	
};

} // namespace mnslp

#endif // MNSLP__POLICY_FILTER_MAPPING_H
