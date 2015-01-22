/// ----------------------------------------*- mode: C++; -*--
/// @file policy_rule.h
/// The policy rule classes.
/// ----------------------------------------------------------
/// $Id: policy_rule.h 2558 2014-11-10 09:55:00 amarentes $
/// $HeadURL: https://./include/policy_rule.h $
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
#ifndef MNSLP__POLICY_RULE_H
#define MNSLP__POLICY_RULE_H

#include <map>

#include "protlib_types.h"
#include "address.h"
#include "mspec_rule_key.h"
#include "msg/mnslp_mspec_object.h"


namespace mnslp {
    using protlib::uint8;
    using protlib::uint16;
    using protlib::uint32;
    using protlib::hostaddress;

/**
 * A metering policy rule.
 *
 * This consists of a filter and an action part. The filter part has 
 * addresses, ports, etc., and the action part decides what to do with
 * packages matching the filter part.
 *
 * The ports and protocol may be 0. In this case they are wildcarded and
 * data traffic from any port or protocol is metered (depending on
 * the action).
 *
 * A policy rule is just an abstraction. It has to be mapped to a concrete
 * metering rule, which is vendor-dependent.
 */
class mt_policy_rule {

  public:

	/**
	* Constructor.
	*/
	mt_policy_rule();

	/**
	* Copy constructor.
	*/
	mt_policy_rule(const mt_policy_rule &rhs);
  
	~mt_policy_rule() { }

    /**
    * Create a deep copy of this object.
    */
	mt_policy_rule *copy() const;

    /**
    * Add a mspec objet to the rule. This creates the key to share with the
    * metering application. 
    */
    void set_object(msg::mnslp_mspec_object &obj);
		
	typedef std::map<mspec_rule_key, msg::mnslp_mspec_object>::const_iterator const_iterator;

	const_iterator begin() const throw() { return objects.begin(); }
	const_iterator end() const throw() { return objects.end(); }
	

  private:
	
	/// Map for all mspec objects belonging to the rule to be used in the 
	/// metering application.
	std::map<mspec_rule_key,msg::mnslp_mspec_object> objects;
	
};

std::ostream &operator<<(std::ostream &out, const mt_policy_rule &mpr);


} // namespace mnslp

#endif // MNSLP__POLICY_RULE_H
