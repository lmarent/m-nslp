/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_field_key.cpp
/// mnslp_field_key.cpp - Implementation of the abstract mnslp_field_key class
/// ----------------------------------------------------------
/// $Id: mnslp_field_key.cpp 2558 2015-01-20 amarentes $
/// $HeadURL: https://src/msg/mnslp_field_key.cpp $
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
#include <iomanip>	// for setw()
#include <cstring>
#include <limits>

#include "msg/mnslp_field_key.h"


using namespace mnslp::msg;

/**
 * Standard constructor.
 *
 * This may leave an instance in an uninitialized state. Use deserialize()
 * to initialize it from a NetMsg.
 *
 * The treatment is set to mandatory.
 */
mnslp_field_key::mnslp_field_key()
		: field_key_type(mnslp_ipfix_key) {

	// nothing to do
}


/**
 * Constructor for manual use.
 *
 * @param field_type, we created three field types for each reporting 
 *        technology. The values used do not follow the standard
 * 		  because this part is not commented.
 */
mnslp_field_key::mnslp_field_key(msnlp_field_key_type_t field_key_type)
		: field_key_type(field_key_type) {

	// nothing to do
}


mnslp_field_key::~mnslp_field_key() {
	// nothing to do
}


uint16
mnslp_field_key::get_field_key_type() {
	return field_key_type;
}


// EOF
