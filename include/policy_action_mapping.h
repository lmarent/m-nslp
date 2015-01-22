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

#include "msg/mnslp_field.h"
#include "mnslp_xml_node_reader.h"
#include "policy_field_mapping.h"

namespace mnslp {


/**
 * This class helps to maintain the mapping configuration for the export 
 * and action processes.
 *
 */
class policy_action_mapping : public policy_field_mapping {

  public:
	
	policy_action_mapping();
		
	policy_action_mapping(const policy_action_mapping &rhs);
	
	virtual ~policy_action_mapping();

    void set_proc_name(std::string proc_name);
    
    void set_priority(int priority);
            
    std::string get_proc_name() const;
    
    int get_priority();
    
    bool operator==(const policy_action_mapping &rhs) const;
    
    bool operator!=(const policy_action_mapping &rhs) const;
    	
	std::ostream &operator<<(std::ostream &out);
	
	std::string to_string();
		    
    static std::string proc_name_str;
    static std::string priority_str;
	    
  private:

	int priority;
	std::string proc_name;
	
	void processNode(int level, xmlTextReaderPtr reader); 
			
};



} // namespace mnslp

#endif // MNSLP__POLICY_ACTION_MAPPING_H
