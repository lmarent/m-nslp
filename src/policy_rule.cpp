/// ----------------------------------------*- mode: C++; -*--
/// @file policy_rule.cpp
/// The policy rule classes.
/// ----------------------------------------------------------
/// $Id: policy_rule.cpp 3166 2014-11-10 09:59:00 amarentes $
/// $HeadURL: https://./src/policy_rule.cpp $
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
#include <assert.h>

#include "logfile.h"

#include "policy_rule.h"

using namespace mnslp;


/*****************************************************************************
 *
 * The mt_policy_rule class.
 *
 *****************************************************************************/

/**
 * Constructor.
 */
mt_policy_rule::mt_policy_rule(msg::mnslp_mspec_object &obj):
object(obj) 
{

	// nothing to do
}


/**
 * Create a deep copy of this object.
 */
mt_policy_rule *mt_policy_rule::copy() const {
	return new mt_policy_rule(*this);
}


/**
 * Create a deep copy of this object.
 */
msg::mnslp_mspec_object * mt_policy_rule::get_object() const {
	return &object;
}


std::ostream &
mnslp::operator<<(std::ostream &out, const mt_policy_rule &mpr)
{

	//TODO AM implement operator<< in msnlp_mspec_object
	//return object.operator <<(out);
	
	return out;
	
}

// EOF
