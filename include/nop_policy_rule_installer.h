/// ----------------------------------------*- mode: C++; -*--
/// @file nop_policy_rule_installer.h
/// The fake policy_rule_installer classes.
/// ----------------------------------------------------------
/// $Id: nop_policy_rule_installer.h 2558 2015-01-16 10:55:00 amarentes $
/// $HeadURL: https://./include/nop_policy_rule_installer.h $
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
#ifndef MNSLP__NOP_POLICY_RULE_INSTALLER_H
#define MNSLP__NOP_POLICY_RULE_INSTALLER_H


#include "policy_rule_installer.h"

namespace mnslp {

/**
 * A policy rule installer which does nothing.
 *
 * Basically, all its methods are no-ops, but the requested actions are logged.
 */
class nop_policy_rule_installer : public policy_rule_installer 
{
	
  public:

	nop_policy_rule_installer(mnslp_config *conf) throw ();

	virtual ~nop_policy_rule_installer() throw ();

	void setup() throw (policy_rule_installer_error);

	virtual void check(const mt_policy_rule *mt_object)
		throw (policy_rule_installer_error);

	virtual void install(const mt_policy_rule *mt_object)
		throw (policy_rule_installer_error);

	virtual void remove(const mt_policy_rule *mt_object)
		throw (policy_rule_installer_error);

	virtual void remove_all() throw (policy_rule_installer_error);
};

} // namespace mnslp


#endif // MNSLP__NOP_POLICY_RULE_INSTALLER_H
