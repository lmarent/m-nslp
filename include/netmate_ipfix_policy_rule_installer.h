/// ----------------------------------------*- mode: C++; -*--
/// @file netmate_policy_rule_installer.h
/// The policy_rule_installer for netmate meter classes.
/// ----------------------------------------------------------
/// $Id: netmate_policy_rule_installer.h 2558 2015-01-15 11:16:00 amarentes $
/// $HeadURL: https://./include/netmate_policy_rule_installer.h $
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

	virtual void check(const mt_policy_rule *rule) 
		    throw (policy_rule_installer_error);

	virtual void install(const mt_policy_rule *rule)
		throw (policy_rule_installer_error);

	virtual void remove(const mt_policy_rule *rule)
		throw (policy_rule_installer_error);

	virtual void remove_all() throw (policy_rule_installer_error);

  private:

	std::string create_connection_command(void) 
			const throw ();

	std::string create_action_command(const msg::mnslp_mspec_object *object) 
			const throw ();

	std::string create_filter_command(const msg::mnslp_mspec_object *object) 
			const throw ();
	
};


} // namespace mnslp

#endif // MNSLP__NETMATE_POLICY_RULE_INSTALLER_H
