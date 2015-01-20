/// ----------------------------------------*- mode: C++; -*--
/// @file message_hop_count.cpp
/// The Message Hop Count Object.
/// ----------------------------------------------------------
/// $Id: message_hop_count.cpp 2558 2014-11-05 15:17:16Z bless $
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

#include "msg/message_hop_count.h"


using namespace mnslp::msg;


const char *const message_hop_count::ie_name = "message_hop_count";


/**
 * Default constructor.
 */
message_hop_count::message_hop_count()
		: mnslp_object(OBJECT_TYPE, tr_mandatory, true), value(0) {

	// nothing to do
}


/**
 * Constructor for manual use.
 *
 * @param msn the message sequence number
 */
message_hop_count::message_hop_count(uint32 mhc, treatment_t treatment, bool _unique)
		: mnslp_object(OBJECT_TYPE, treatment, _unique), value(mhc) {

	// nothing to do
}


message_hop_count::~message_hop_count() {
	// nothing to do
}


message_hop_count *message_hop_count::new_instance() const {
	message_hop_count *q = NULL;
	catch_bad_alloc( q = new message_hop_count() );
	return q;
}


message_hop_count *message_hop_count::copy() const {
	message_hop_count *q = NULL;
	catch_bad_alloc( q = new message_hop_count(*this) );
	return q;
}


bool message_hop_count::deserialize_body(NetMsg &msg, uint16 body_length,
		IEErrorList &err, bool skip) {

	set_value(msg.decode32());

	return true; // success, all values are syntactically valid
}


void message_hop_count::serialize_body(NetMsg &msg) const {
	msg.encode32(get_value());
}


size_t message_hop_count::get_serialized_size(coding_t coding) const {
	return HEADER_LENGTH + 4;
}


// All values are valid.
bool message_hop_count::check_body() const {
	return true;
}


bool message_hop_count::equals_body(const mnslp_object &obj) const {

	const message_hop_count *other
		= dynamic_cast<const message_hop_count *>(&obj);

	return other != NULL && get_value() == other->get_value();
}


const char *message_hop_count::get_ie_name() const {
	return ie_name;
}


ostream &message_hop_count::print_attributes(ostream &os) const {
	return os << ", value=" << get_value();
}


/**
 * Returns the message hop count.
 *
 * @return the message hop count
 */
uint32 message_hop_count::get_value() const {
	return value;
}


/**
 * Set the message hop count.
 *
 * @param mhc the message hop count
 */
void message_hop_count::set_value(uint32 mhc) {
	value = mhc;
}


// EOF
