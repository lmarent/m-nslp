/// ----------------------------------------*- mode: C++; -*--
/// @file policy_field_mapping.h
/// The policy_field_mapping class.
/// ----------------------------------------------------------
/// $Id: policy_field_mapping.h 2558 2015-01-16 13:34:00 amarentes $
/// $HeadURL: https://./include/policy_field_mapping.h $
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
#ifndef MNSLP__POLICY_FIELD_MAPPING_H
#define MNSLP__POLICY_FIELD_MAPPING_H

#include <map>
#include <string>
#include <libxml/xmlreader.h>

#include "msg/mnslp_field.h"
#include "mnslp_xml_node_reader.h"

namespace mnslp {

class application_field_mapping
{

	public: 
		application_field_mapping(std::string configuration, std::string filter, 
									std::string exporting, std::string application_key);
		
		~application_field_mapping() { }
		
		application_field_mapping(const application_field_mapping &rhs );

		bool operator==(const application_field_mapping &rhs) const;
    
		bool operator!=(const application_field_mapping &rhs) const;
		
		bool is_to_configure() { return is_configuration_field; }
		
		bool is_for_exporting() { return is_export_field; }
		
		bool is_for_filtering() { return is_filter_field; }
		
		std::string get_field_traslate() { return application_field_key; }
		
		std::string get_field_traslate() const { return application_field_key; }

		bool is_configuration() const { return is_configuration_field; }
	
		bool is_filter() const { return is_filter_field; }
	
		bool is_export() const { return is_export_field; }
				
		std::string to_string() const; 
					

	private:
		bool is_configuration_field;
		bool is_filter_field;
		bool is_export_field;
		std::string application_field_key;
		
};


/**
 * This class helps to maintain the mapping configuration for the export 
 * and action processes.
 *
 */
class policy_field_mapping : public mnslp_xml_node_reader {


  public:
	
	policy_field_mapping();
		
	policy_field_mapping(const policy_field_mapping &rhs);
	
	virtual ~policy_field_mapping();

	/**
	 * This function assumes that the pointer is at the beginning of a 
	 * policy action mapping. When the function ends the pointer can be
	 * at the beginning of the next object or at the end of the document
	 */ 
	int read_from_xml(int level, xmlTextReaderPtr node);
    
    void set_field(msg::mnslp_field_key *ipfixfield, 
			  	   application_field_mapping fieldMetering);

    void set_metering_application(std::string metering_app);
            
    bool operator==(const policy_field_mapping &rhs) const;
    
    bool operator!=(const policy_field_mapping &rhs) const;
    
	typedef std::map<msg::mnslp_field_key *, application_field_mapping>::const_iterator const_iterator;
	const_iterator begin() const throw() { return fields.begin(); }
	const_iterator end() const throw() { return fields.end(); }
	
	std::ostream &operator<<(std::ostream &out);
	std::string to_string();
	
	bool check_field_availability(msg::mnslp_field &field);

	bool check_field_availability(msg::mnslp_field &field) const;
	
	std::string get_field_traslate(msg::mnslp_field &field);
	
	std::string get_field_traslate(msg::mnslp_field &field) const;

	bool is_configuration_field( msg::mnslp_field &field) const;
	
	bool is_filter_field( msg::mnslp_field &field) const;
	
	bool is_export_field( msg::mnslp_field &field) const;

    static std::string field_str;
    static std::string eno_str;
    static std::string field_type_str;
    static std::string field_name_str;
    static std::string configuration_str;
    static std::string filter_str;
    static std::string export_str;
    
    std::string get_metering_application(){ return metering_application; }
    std::string get_metering_application() const { return metering_application; }

    
  protected:
	std::map<msg::mnslp_field_key *, application_field_mapping> fields;
	std::string metering_application;
	
	void processNode(int level, xmlTextReaderPtr reader); 	

	application_field_mapping get_field(msg::mnslp_field_key *_ipfixfield) const;
	
};



} // namespace mnslp

#endif // MNSLP__POLICY_FIELD_MAPPING_H
