/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_field.cpp
/// mnslp_field.cpp - Implementation of the abstract mnslp_field class
/// ----------------------------------------------------------
/// $Id: mnslp_field.cpp 2558 2015-01-20 amarentes $
/// $HeadURL: https://src/msg/mnslp_field.cpp $
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

#include "msg/mnslp_field.h"


using namespace mnslp::msg;

/**
 * Standard constructor.
 *
 * This may leave an instance in an uninitialized state. Use deserialize()
 * to initialize it from a NetMsg.
 *
 * The treatment is set to mandatory.
 */
mnslp_field::mnslp_field()
		: field_type(mnslp_ipfix_type) {

	// nothing to do
}


/**
 * Constructor for manual use.
 *
 * @param field_type, we created three field types for each reporting 
 *        technology. The values used do not follow the standard
 * 		  because this part is not commented.
 */
mnslp_field::mnslp_field(msnlp_field_type_t field_type)
		: field_type(field_type) {

	// nothing to do
}


mnslp_field::~mnslp_field() {
	// nothing to do
}


uint16
mnslp_field::get_field_type(){
	return field_type;
}


// EOF
