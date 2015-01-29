/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_mspec_object.cpp
/// mnslp_object.cpp - Implementation of the abstract mnslp_mspec_object class
/// ----------------------------------------------------------
/// $Id: mnslp_mspec_object.cpp 2558 2014-11-05 amarentes $
/// $HeadURL: https://src/msg/mnslp_mspec_object.cpp $
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

#include "logfile.h"

#include "msg/mnslp_ie.h"
#include "msg/mnslp_mspec_object.h"


using namespace mnslp::msg;
using namespace protlib::log;


/**
 * Standard constructor.
 *
 * This may leave an instance in an uninitialized state. Use deserialize()
 * to initialize it from a NetMsg.
 *
 * The treatment is set to mandatory.
 */
mnslp_mspec_object::mnslp_mspec_object()
		: mnslp_object() {

	// nothing to do
}

/**
 * Constructor for manual use.
 *
 * The obj_type value must not be larger than 12 bit. Note that only a small
 * subset is valid according to the MNSLP standard and registered with IANA.
 *
 * @param object_type the MNSLP Object Type (12 bit)
 */
mnslp_mspec_object::mnslp_mspec_object(uint16 obj_type, treatment_t tr, bool _unique)
		: mnslp_object(obj_type, tr, _unique) {

	// nothing to do
}


mnslp_mspec_object::~mnslp_mspec_object() {
	// nothing to do
}



// EOF
