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
#include <limits>
#include <libxml/xmlreader.h>

#include "msg/mnslp_field.h"
#include "mnslp_xml_node_reader.h"
#include "policy_field_mapping.h"

namespace mnslp {

/**
 * This class represents the configuration established for an metering application
 **/
class metering_config
{
	public: 
		metering_config():priority(std::numeric_limits<int>::max()){}
		virtual ~metering_config() = 0;
		
		virtual metering_config *copy() const = 0;
		
		virtual bool is_equal(const metering_config &rhs) = 0;
		virtual bool not_equal(const metering_config &rhs) = 0;

		virtual void set_priority(int _priority) = 0;

		virtual int get_priority() const = 0;

		virtual int get_priority() = 0;

		virtual std::ostream &operator<<(std::ostream &out) const = 0;
	
		virtual std::string to_string() const = 0;


	protected:
		/// An application can have more than a procedure to meter a specific 
		/// export field. This value decides which is the prefered metering 
		/// procedure when that happens.
		int priority; 		
};

inline metering_config::~metering_config()
{
   // Nothing to do.
}

/**
 * This class helps to maintain the mapping configuration for the export 
 * and action processes.
 *
 */
class policy_action_mapping : public policy_field_mapping {


  public:
	
	policy_action_mapping();
			
	virtual ~policy_action_mapping() = 0;
	
	static policy_action_mapping *make(std::string app);
	
	virtual policy_action_mapping *copy() const = 0;
	
	virtual int read_from_xml(int level, xmlTextReaderPtr node) = 0;
    
    bool is_equal(const policy_action_mapping &rhs) const;
    
    bool not_equal(const policy_action_mapping &rhs) const;
    	
	std::ostream &operator<<(std::ostream &out);
	
	std::string to_string() const;
	
	void set_metering_configuration(metering_config *met_conf);
	
	const metering_config * get_metering_configuration() const;
	
	virtual std::string get_key() const = 0;
	
	int get_priority() const;
		       
  protected:

	metering_config *meter_config;
					
};

} // namespace mnslp

#endif // MNSLP__POLICY_ACTION_MAPPING_H
