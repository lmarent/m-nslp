/// ----------------------------------------*- mode: C++; -*--
/// @file mspec_rule_key.cpp
/// Keys used to control the metering application interface for 
/// NSIS metering objects.
/// ----------------------------------------------------------
/// $Id: mspec_rule_key.cpp 2558 2015-01-22  $
/// $HeadURL: https://./src/mspec_rule_key.cpp $
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

#include <sstream>
#include <iostream>
#include "mspec_rule_key.h"


namespace mnslp 
{

/// Constructor of the field key
mspec_rule_key::mspec_rule_key()
{
	uuid_generate_time_safe(uuid);
}

/// Copy constructor of the field key
mspec_rule_key::mspec_rule_key(const mspec_rule_key &rhs)
{
	//std::cout << "constructor mspec_rule_key" << std::endl;
	uuid_copy(uuid, rhs.uuid);
}

	
/// Destructor of the field key
mspec_rule_key::~mspec_rule_key()
{
   uuid_clear(uuid);
}
		
/**
 *  Equals to operator. It is equal when they have the same uuids.
 */
bool 
mspec_rule_key::operator ==(const mspec_rule_key &rhs) const
{
	int rv;
	rv = uuid_compare(uuid, rhs.uuid);
	if (rv == 0)
		return true;
	else
		return false;
}

/** 
 * less operator. 
 */ 
bool 
mspec_rule_key::operator< (const mspec_rule_key& rhs) const
{
	int rv;
	rv = uuid_compare(uuid, rhs.uuid);
	if (rv < 0)
		return true;
	else
		return false;
	
}

/**
 * Return the key represented as string. 
 */
std::string 
mspec_rule_key::to_string() const
{
	char uuid_str[37]; 
	uuid_unparse_lower(uuid, uuid_str); 
	std::string val_return (uuid_str);
	return val_return;
}	


}
