/// ----------------------------------------*- mode: C++; -*--
/// @file netmate_ipfix_policy_rule_installer.h
/// The policy_rule_installer for netmate meter classes.
/// ----------------------------------------------------------
/// $Id: netmate_ipfix_policy_rule_installer.h 2558 2015-01-15 11:16:00 amarentes $
/// $HeadURL: https://./include/netmate_ipfix_policy_rule_installer.h $
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
#ifndef MNSLP__NETMATE_IPFIX_POLICY_RULE_INSTALLER_H
#define MNSLP__NETMATE_IPFIX_POLICY_RULE_INSTALLER_H


#include "session.h"
#include "msg/mnslp_mspec_object.h"
#include "mnslp_config.h"
#include "msg/mnslp_ipfix_message.h"
#include "msg/mnslp_ipfix_template.h"
#include "policy_rule_installer.h"

// curl includes
#include <curl/curl.h>


namespace mnslp {

/**
 * A policy rule installer for netmate.
 *
 * This implementation is Linux-specific and uses the netmate library to
 * install and remove policy rules.
 *
 * This class is currently stateless, so no locking is required. Library calls
 * are done using the HTTP-enabled API interface, so multithreaded calls to the
 * library are safe.
 */
class netmate_ipfix_policy_rule_installer : public policy_rule_installer 
{
  public:
  
	netmate_ipfix_policy_rule_installer(mnslp_config *conf) throw ();
	
	virtual ~netmate_ipfix_policy_rule_installer() throw ();

	void setup() throw (policy_rule_installer_error);

	/**
	 * This method verifies whether a metering spec object might be deployed in this 
	 * metering application or not. 
	 */
	virtual void check(const msg::mnslp_mspec_object *object) 
		    throw (policy_rule_installer_error);
	/**
	 * This method installs a group of metering spec objects into the metering
	 * application, it assumes that every object included has been checked
	 * to be installable in the metering application. 
	 * 
	 * Creates a new mt_policy_rule that constains mspec objects installed.
	 */
	virtual mt_policy_rule * install(const mt_policy_rule *rule);

	/**
	 * This method removes a group of metering spec objects into the metering
	 * application, it assumes that every object included has been checked
	 * to be installable in the metering application. 
	 */
	virtual mt_policy_rule * remove(const mt_policy_rule *rule);

	virtual bool remove_all();


  protected:

    const msg::mnslp_ipfix_message * get_ipfix_message(const msg::mnslp_mspec_object *object);
    
    msg::mnslp_ipfix_template * get_filter_template(const msg::mnslp_ipfix_message *mes) const;
    
    msg::mnslp_ipfix_template * get_export_template(const msg::mnslp_ipfix_message *mess) const;
    
    std::map<std::string, std::string> 
    build_command_export_fields( const mspec_rule_key &key, 
	 							 const msg::mnslp_ipfix_message *mess, 
								 msg::mnslp_ipfix_template *templ,
								 std::string filter_def) const;

	std::string build_command_filter_fields(const msg::mnslp_ipfix_message *mess, 
									msg::mnslp_ipfix_template *templ) const;

	void print_filter_values(msg::mnslp_ipfix_field &field,
							 std::list<std::string> &values,
							 std::map<std::string, std::string> &filter_fields) const;

	std::string create_action_command(const msg::mnslp_ipfix_message *object) 
			const throw ();

	std::map<std::string, std::string> 
	create_postfield_command(const mspec_rule_key &key, 
							 const msg::mnslp_ipfix_message *mess) const throw ();
			
	bool execute_command(std::string action, std::string post_fields);
	
	/**
	 * This function verifies that all options fields included in the option template
	 * are also included as data fields. Returns true if the condition is
	 * found, false otherwise.
	 */ 
	bool include_all_data_fields(const msg::mnslp_ipfix_message *mess, 
							     msg::mnslp_ipfix_template *templ);
	
	/**
	 * This function verifies that all export fields included in the template
	 * can be processed by the metering application. 
	 * Returns true if the condition is found, false otherwise.
	 */ 
	bool handle_export_fields(const msg::mnslp_ipfix_message *mess, 
							  msg::mnslp_ipfix_template *templ);
	
	/**
	 * This function verifies that all filter fields included in the 
	 * option template can be processed by the metering application. 
	 * Returns true if the condition is found, false otherwise.
	 */ 
	bool handle_filter_fields(const msg::mnslp_ipfix_message *mess, 
						      msg::mnslp_ipfix_template *templ);
	
	/** 
	 * Extract errno out of err string generated by curl and translate errno
	 * to string by using strerror
	 */
	std::string getErr(char *e);
	
	static std::string rule_group; 
	
};


} // namespace mnslp

#endif // MNSLP__NETMATE_POLICY_RULE_INSTALLER_H
