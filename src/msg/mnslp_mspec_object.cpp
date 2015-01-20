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


const char *const mnslp_mspec_object::ie_name = "mnslp_ipfix_mspec";

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

mnslp_mspec_object::mnslp_mspec_object(const mnslp_mspec_object &rhs):
	mnslp_object(rhs.get_object_type(), rhs.get_treatment(),  rhs.is_unique())
{
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

mnslp_mspec_object&
mnslp_mspec_object::operator=(const mnslp_mspec_object &rhs)
{
	set_object_type(rhs.get_object_type());
	set_treatment(rhs.get_treatment());
	set_unique(rhs.is_unique());
}


mnslp_mspec_object *
mnslp_mspec_object::new_instance() const {
	mnslp_mspec_object *q = NULL;
	catch_bad_alloc( q = new mnslp_mspec_object() );
	return q;
}

mnslp_mspec_object *
mnslp_mspec_object::copy() const {
	mnslp_mspec_object *q = NULL;
	
	catch_bad_alloc( q = new mnslp_mspec_object(*this) );
	return q;
}

size_t 
mnslp_mspec_object::get_serialized_size(coding_t coding) const 
{
	return HEADER_LENGTH;
}

bool 
mnslp_mspec_object::deserialize_body(NetMsg &msg, uint16 body_length,
		IEErrorList &err, bool skip) {

	return true; // success
	
}

bool 
mnslp_mspec_object::check_body() const
{
	return true;
}

bool mnslp_mspec_object::equals_body(const mnslp_object &obj) const {

	const mnslp_mspec_object *other
		= dynamic_cast<const mnslp_mspec_object *>(&obj);

	return other != NULL;
}

const char *mnslp_mspec_object::get_ie_name() const {
	return ie_name;
}


void mnslp_mspec_object::serialize_body(NetMsg &msg) const {

  // Nothing to do

}


// EOF
