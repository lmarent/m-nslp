/// ----------------------------------------*- mode: C++; -*--
/// @file policy_rule_installer.h
/// The policy_rule_installer classes.
/// ----------------------------------------------------------
/// $Id: policy_rule_installer.h 2558 2014-11-10 10:03:00 amarentes $
/// $HeadURL: https://./include/policy_rule_installer.h $
// ===========================================================
//                      
// Copyright (C) 2012-2014, all rights reserved by
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
#ifndef MNSLP__POLICY_RULE_INSTALLER_H
#define MNSLP__POLICY_RULE_INSTALLER_H


#include "session.h"
#include "mnslp_config.h"

#include "policy_rule.h"
#include "policy_action_container.h"
#include "policy_application_configuration_container.h"


namespace mnslp {

/**
 * An exception to be thrown if the policy_rule_installer failed.
 */
class policy_rule_installer_error : public request_error {
	
  public:
	
	policy_rule_installer_error(const std::string &msg,
		uint8 severity=0, uint8 response_code=0) throw ()
		: request_error(msg, severity, response_code) { }
		
	virtual ~policy_rule_installer_error() throw () { }
};

inline std::ostream &operator<<(
		std::ostream &out, const policy_rule_installer_error &e) {

	return out << e.get_msg();
}


/**
 * An abstract interface to the operating system's Metering package.
 *
 * Callers pass Metering policy rules to install() and this class
 * maps it to operating system specific rules. The installed rules can later
 * be removed using remove().
 *
 * Subclasses have to implement locking if necessary.
 */
class policy_rule_installer {

  public:
  
	policy_rule_installer(mnslp_config *conf) throw () : 
		config(conf), action_container(NULL), app_container(NULL) { }
	
	virtual ~policy_rule_installer() throw ();

	/**
	 * Prepare the external METERING package for usage.
	 */
	virtual void setup() throw (policy_rule_installer_error);

	/**
	 * Check if the given policy rule is correct and supported.
	 *
	 * If they are not, an exception is thrown.
	 */
	virtual void check(const msg::mnslp_mspec_object *object)
		throw (policy_rule_installer_error) = 0;

	/**
	 * Install the given policy rule.
	 * 
	 * Subclasses have to use operating system dependent code to access the
	 * local metering package. After calling this method, the rules are
	 * in effect and data traffic will be treated as requested.
	 */
	virtual mt_policy_rule * install(const mt_policy_rule *mt_object) = 0;

	/**
	 * Remove the given policy rule.
	 *
	 * After calling this method, the previously installed rules are no
	 * longer in effect.
	 *
	 * Note that this is the reverse operation to install(). Only previously
	 * installed policy rule may be deleted!. 
	 */
	virtual mt_policy_rule * remove(const mt_policy_rule * mt_object) = 0;

	virtual bool remove_all() = 0;

			
	const policy_action_container * get_action_container() const;	
	
	const policy_application_configuration_container * get_application_configuration_container() const;

	std::string get_metering_application(){ return config->get_metering_application(); }

	std::string get_metering_application() const { return config->get_metering_application(); }
		
	std::string get_user() const { return config->get_user(); }
	
	std::string get_password() const { return config->get_password(); } 
	
	std::string get_metering_server() const { return config->get_metering_server(); } 
	
	std::string get_metering_xsl() const { return config->get_metering_xsl(); } 
	
	uint32 get_metering_port() const { return config->get_metering_port(); } 
	
	std::string get_export_directory() const { return config->get_export_directory(); }
	
	std::string to_string() const;
  
  private:
  
	mnslp_config *config;
	policy_action_container * action_container;
	policy_application_configuration_container * app_container;

	/**
	* parse_export_definition_file:
	* @filename: the file name to parse
	*
	* Parse, validate and load information about the export configuration.
	*/
	void parse_export_definition_file(const char *filename) throw (policy_rule_installer_error);

	/**
	* parse_filter_definition_file:
	* @filename: the file name to parse
	*
	* Parse, validate and load information about the filter configuration.
	*/
	void parse_configuration_definition_file(const char *filename) throw (policy_rule_installer_error);
	
	
	
};

} // namespace mnslp

#include "nop_policy_rule_installer.h"
#include "netmate_ipfix_policy_rule_installer.h"


#endif // MNSLP__POLICY_RULE_INSTALLER_H
