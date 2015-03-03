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
	
	// Copy rules keys.
	const_iterator_commands it_rules;
	for ( it_rules = rhs.rule_keys.begin(); it_rules != rhs.rule_keys.end(); it_rules++ )
	{
		const mspec_rule_key id = it_rules->first;
		std::vector<std::string> rules = it_rules->second;
		set_commands(id, rules);
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

mspec_rule_key 
mt_policy_rule::set_object(msg::mnslp_mspec_object *obj)
{
	mspec_rule_key key;
	objects.insert(std::pair<mspec_rule_key, msg::mnslp_mspec_object *> (key, obj));
	return key;
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
	
	// All objects have to be identical.
	for ( const_iterator i = objects.begin(); i != objects.end(); i++ ) {
		const_iterator j = rhs.objects.find(i->first);
		if ( j == rhs.objects.end() ){
			return false;
		}
			
		if ( (i->second)->notEqual(*(j->second))  ){
			return false;
		}
	}	

	// Verify rule keys.
	if (rule_keys.size() != rhs.rule_keys.size())
		return false;
	
	for ( const_iterator_commands it = rule_keys.begin(); it != rule_keys.end(); it++ ) {
		const_iterator_commands ij = rhs.rule_keys.find(it->first);
		if ( ij == rhs.rule_keys.end() )
			return false;
		
		if ( are_equal(it->second, ij->second) == false )
			return false;	

	}
	return true;
}

bool 
mt_policy_rule::are_equal(std::vector<std::string> left, std::vector<std::string> right)
{
	std::sort (left.begin(), left.end());
	std::sort (right.begin(), right.end());
	for ( int index = 0; index < rule_keys.size(); index ++ ) {
				
		if ( left[index].compare(right[index]) != 0 ){
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

mt_policy_rule & 
mt_policy_rule::operator=(const mt_policy_rule &rhs)
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
	
	const_iterator_commands it_rule_keys;
	for ( it_rule_keys = rhs.rule_keys.begin(); it_rule_keys != rhs.rule_keys.end(); it_rule_keys++)
	{
		mspec_rule_key id = it_rule_keys->first;
		std::vector<std::string> list_keys = it_rule_keys->second;
		set_commands(id, list_keys);
	}
	
	return *this;
}

void mt_policy_rule::set_commands(mspec_rule_key key, std::vector<std::string> commands)
{
	rule_keys.insert(std::pair<mspec_rule_key, std::vector<std::string> > (key, commands));

}

void mt_policy_rule::clear_commands()
{
	rule_keys.clear();
}

size_t mt_policy_rule::get_number_rule_keys()
{
	return rule_keys.size();
}

size_t mt_policy_rule::get_number_rule_keys() const
{
	return rule_keys.size();
}


}

// EOF
