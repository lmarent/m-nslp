/// ----------------------------------------*- mode: C; -*--
/// @file mspec_rule_key.h
/// Tools for processing NSIS metering.
/// ----------------------------------------------------------
/// $Id: mspec_rule_key.h 2558 2014-12-26 7:30:00 amarentes $
/// $HeadURL: https://./include/mspec_rule_key.h $
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

#ifndef MNSLP__MSPEC_RULE_KEY_H
#define MNSLP__MSPEC_RULE_KEY_H

#include <uuid/uuid.h>

namespace mnslp 
{

/**
 * \class mspec_rule_key
 *
 *
 * \brief This class respresents the key of a mnspec object that is 
 * 		  going to be installed in a metering application (MA). 
 * 		  This value is used whenever we refer to this policy on the MA. Therefore, 
 *        we assume that is shared with the MA.
 *
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2015/01/22 7:30:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class mspec_rule_key
{

private:

	// typedef unsigned char uuid_t[16];
	uuid_t uuid;

public:
	
	/// Constructor of the field key
	mspec_rule_key();
	
	/// Copy contructor - Constructs a copy of mspec_rule_key.
	mspec_rule_key(const mspec_rule_key &rul_key);
	
	/// Destructor of the field key
	~mspec_rule_key();
		
	/**
	 *  Equals to operator. It is equal when they have the same uuids.
	 */
	bool operator ==(const mspec_rule_key &rhs) const;

	/** 
	 * less operator. 
	 */ 
	bool operator< (const mspec_rule_key& rhs) const;

	/** 
	 * Assignment operator. 
	*/ 
	inline mspec_rule_key& operator= (const mspec_rule_key& param)
	{
		uuid_copy(uuid, param.uuid); 
		return *this;
	}

	/**
	 * Return the key represented as string. 
	 */
	std::string get_string_key() const;
	
	/** 
	 * Not equal to operator. 
	*/ 
	inline bool operator != (const mspec_rule_key &rhs) const
	{
		return !(operator ==(rhs));
	}
	
};

} // namespace mnslp

#endif // MNSLP__MSPEC_RULE_KEY_H
