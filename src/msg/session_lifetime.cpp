/// ----------------------------------------*- mode: C++; -*--
/// @file session_lifetime.cpp
/// The Session Lifetime Object.
/// ----------------------------------------------------------
/// $Id: session_lifetime.cpp 2558 2014-11-05 bless $
/// $HeadURL: https://./src/msg/session_lifetime.cpp $
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
#include "logfile.h"

#include "msg/session_lifetime.h"


using namespace mnslp::msg;


const char *const session_lifetime::ie_name = "session_lifetime";


/**
 * Default constructor.
 */
session_lifetime::session_lifetime()
		: mnslp_object(OBJECT_TYPE, tr_mandatory, true), slt(0) {

	// nothing to do
}


/**
 * Constructor for manual use.
 *
 * @param value the session lifetime in seconds
 */
session_lifetime::session_lifetime(uint32 _slt, treatment_t treatment, bool _unique)
		: mnslp_object(OBJECT_TYPE, treatment, _unique), slt(_slt) {

	// nothing to do
}

/**
 * Class destructor 
 */
session_lifetime::~session_lifetime() {
	// nothing to do
}


session_lifetime *session_lifetime::new_instance() const {
	session_lifetime *q = NULL;
	catch_bad_alloc( q = new session_lifetime() );
	return q;
}


session_lifetime *session_lifetime::copy() const {
	session_lifetime *q = NULL;
	catch_bad_alloc( q = new session_lifetime(*this) );
	return q;
}


bool session_lifetime::deserialize_body(NetMsg &msg, uint16 body_length,
		IEErrorList &err, bool skip) {

	set_value(msg.decode32());

	return true; // success, all values are syntactically valid
}


void session_lifetime::serialize_body(NetMsg &msg) const {
	msg.encode32(get_value());
}


size_t session_lifetime::get_serialized_size(coding_t coding) const {
	return HEADER_LENGTH + 4;
}


// All values are valid.
bool session_lifetime::check_body() const {
	return true;
}


bool session_lifetime::equals_body(const mnslp_object &obj) const {

	const session_lifetime *other
		= dynamic_cast<const session_lifetime *>(&obj);

	return other != NULL && get_value() == other->get_value();
}


const char *session_lifetime::get_ie_name() const {
	return ie_name;
}


ostream &session_lifetime::print_attributes(ostream &os) const {
	return os << ", value=" << get_value();
}


/**
 * Returns the session lifetime in seconds.
 *
 * @return the session lifetime in seconds.
 */
uint32 session_lifetime::get_value() const {
	return slt;
}


/**
 * Set the session lifetime in seconds.
 *
 * @param val the session lifetime in seconds.
 */
void session_lifetime::set_value(uint32 _slt) {
	slt = _slt;
}


// EOF
