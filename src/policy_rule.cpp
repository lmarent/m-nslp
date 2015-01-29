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

namespace mnslp {

/*****************************************************************************
 *
 * The mt_policy_rule class.
 *
 *****************************************************************************/

/**
 * Constructor.
 */
mt_policy_rule::mt_policy_rule()
{
	// nothing to do
}

mt_policy_rule::~mt_policy_rule()
{
	std::map<mspec_rule_key, msg::mnslp_mspec_object *>::iterator it;
	for ( it = objects.begin(); it != objects.end(); it++)
	{
		delete(it->second);
		objects.erase(it);
	}
}

/**
 * Copy constructor.
 */
mt_policy_rule::mt_policy_rule(const mt_policy_rule &rhs)
{
	std::map<mspec_rule_key, msg::mnslp_mspec_object *>::const_iterator it;
	for ( it = rhs.objects.begin(); it != rhs.objects.end(); it++ )
	{
		const mspec_rule_key id = it->first;
		const msg::mnslp_mspec_object *obj = it->second;
		if (obj){
			set_object(id, obj->copy());
		}
	}
}

/**
 * Create a deep copy of this object.
 */
mt_policy_rule *mt_policy_rule::copy() const 
{
	return new mt_policy_rule(*this);
}

void 
mt_policy_rule::set_object(mspec_rule_key key, msg::mnslp_mspec_object *obj)
{
	if ( obj == NULL )
		return;

	msg::mnslp_mspec_object *old = objects[key];

	if ( old )
		delete old;

	objects[key] = obj;
}

/**
 * Add a mspec objet to the rule. This creates the key to share with the
 * metering application. 
*/
std::string 
mt_policy_rule::set_object(msg::mnslp_mspec_object *obj)
{
	mspec_rule_key key;
	objects.insert(std::pair<mspec_rule_key, msg::mnslp_mspec_object *> (key, obj));
	return key.to_string();
}

size_t
mt_policy_rule::get_number_mspec_objects()
{
	return objects.size();
}

bool
mt_policy_rule::operator==(const mt_policy_rule &rhs)
{
	if (objects.size() != rhs.objects.size()){
		return false;
	}

	// All entries have to be identical.
	for ( const_iterator i = objects.begin(); i != objects.end(); i++ ) {
		const_iterator j = rhs.objects.find(i->first);
		if ( j == rhs.objects.end() ){
			return false;
		}
			
		if ( (i->second)->notEqual(*(j->second))  ){
			return false;
		}
	}	
	
	return true;

}

bool
mt_policy_rule::operator!=(const mt_policy_rule &rhs) 
{
	return !(operator==(rhs));
}

std::ostream &
operator<<(std::ostream &out, const mt_policy_rule &r) 
{
	// TODO AM: implement this function.
}

}

// EOF
