/// ----------------------------------------*- mode: C; -*--
/// @file ie_object_key.h
/// Tools for processing NSIS metering.
/// ----------------------------------------------------------
/// $Id: IE_object_key.h 2558 2014-12-26 7:30:00 amarentes $
/// $HeadURL: https://./include/IE_object_key.h $
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

#ifndef PROTLIB__IE_OBJECT_KEY_H
#define PROTLIB__IE_OBJECT_KEY_H

#include "ie.h"

namespace protlib 
{

/**
 * \class ie_object_key
 *
 *
 * \brief This respresents the key of an object to be exchanged.
 *
 * This class is used to represent the key of an object that is going to be exchange.
 * The class is the key used on the map of object values inside the mnslp message.
 *
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2014/12/26 7:30:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class ie_object_key
{

private:

	uint32 object_type;  		///< Object type assigned
	uint32 seq_nbr;             ///< Sequence number for messages of metering protocols.
	                            ///< for all other objects it is zero.

public:
	
	/// Constructor of the field key
	inline ie_object_key(uint32 _object_type, uint32 _seq_nbr): 
			object_type(_object_type), seq_nbr(_seq_nbr){}
	
	/// Destructor of the field key
	inline ~ie_object_key(){}
		
	/**
	 *  Equals to operator. It is equal when it has the same objetc type and sequence.
	 */
	inline bool operator ==(const ie_object_key &rhs) const
	{ 
		return ((object_type == rhs.object_type) && (seq_nbr == rhs.seq_nbr)); 
	}

	/** less operator. a key field is less than other when the sum of its 
	*    attributes is less that the same sum for the key field given as 
	*    parameter.
	*/ 
	inline bool operator< (const ie_object_key& rhs) const
	{
		return (object_type + seq_nbr) < (rhs.object_type + rhs.seq_nbr ); 
	}

	/** 
	 * Assignment operator. 
	*/ 
	inline ie_object_key& operator= (const ie_object_key& param)
	{
		object_type = param.object_type;
		seq_nbr = param.seq_nbr;
		return *this;
	}
	
	inline uint32 get_object_type() const
	{
		return object_type;
	}
	
	inline uint32 get_sequence_number() const
	{
		return seq_nbr;
	}
	
	/** 
	 * Not equal to operator. 
	*/ 
	inline bool operator != (const ie_object_key &rhs) const
	{
		return ((object_type != rhs.object_type) || (seq_nbr != rhs.seq_nbr)); 
	}
	
	/** Convert the key field in a string.
	*/ 
	std::string to_string() const;

};

} // namespace protlib

#endif // PROTLIB__IE_OBJECT_KEY_H
