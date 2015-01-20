/// ----------------------------------------*- mode: C++; -*--
/// @file msg_sequence_number.cpp
/// The Message Sequence Number Object.
/// ----------------------------------------------------------
/// $Id: msg_sequence_number.cpp 2558 2014-11-05 15:17:16Z bless $
/// $HeadURL:  $
// ===========================================================
//                      
// Copyright (C) 2012-2014, all rights reserved by
// - System and Computing Engineering, Universidad de los Andes
//
// More information and contact:
// https://www.uniandes.edu.co
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

#include "msg/msg_sequence_number.h"


using namespace mnslp::msg;


const char *const msg_sequence_number::ie_name = "msg_sequence_number";


/**
 * Default constructor.
 */
msg_sequence_number::msg_sequence_number()
		: mnslp_object(OBJECT_TYPE, tr_mandatory, true), value(0) {

	// nothing to do
}


/**
 * Constructor for manual use.
 *
 * @param msn the message sequence number
 */
msg_sequence_number::msg_sequence_number(uint32 msn, treatment_t treatment, bool _unique)
		: mnslp_object(OBJECT_TYPE, treatment, _unique), value(msn) {

	// nothing to do
}


msg_sequence_number::~msg_sequence_number() {
	// nothing to do
}


msg_sequence_number *msg_sequence_number::new_instance() const {
	msg_sequence_number *q = NULL;
	catch_bad_alloc( q = new msg_sequence_number() );
	return q;
}


msg_sequence_number *msg_sequence_number::copy() const {
	msg_sequence_number *q = NULL;
	catch_bad_alloc( q = new msg_sequence_number(*this) );
	return q;
}


bool msg_sequence_number::deserialize_body(NetMsg &msg, uint16 body_length,
		IEErrorList &err, bool skip) {

	set_value(msg.decode32());

	return true; // success, all values are syntactically valid
}


void msg_sequence_number::serialize_body(NetMsg &msg) const {
	msg.encode32(get_value());
}


size_t msg_sequence_number::get_serialized_size(coding_t coding) const {
	return HEADER_LENGTH + 4;
}


// All values are valid.
bool msg_sequence_number::check_body() const {
	return true;
}


bool msg_sequence_number::equals_body(const mnslp_object &obj) const {

	const msg_sequence_number *other
		= dynamic_cast<const msg_sequence_number *>(&obj);

	return other != NULL && get_value() == other->get_value();
}


const char *msg_sequence_number::get_ie_name() const {
	return ie_name;
}


ostream &msg_sequence_number::print_attributes(ostream &os) const {
	return os << ", value=" << get_value();
}


/**
 * Returns the message sequence number.
 *
 * @return the message sequence number
 */
uint32 msg_sequence_number::get_value() const {
	return value;
}


/**
 * Set the message sequence number.
 *
 * @param msn the message sequence number
 */
void msg_sequence_number::set_value(uint32 msn) {
	value = msn;
}


// EOF
