/// ----------------------------------------*- mode: C++; -*--
/// @file ntlp_msg.cpp
/// An NTLP Message.
/// ----------------------------------------------------------
/// $Id: ntlp_msg.cpp 2558 2014-11-05 18:24:00Z amarentes $
/// $HeadURL: https://./src/msg/ntlp_msg.cpp $
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
#include "msg/ntlp_msg.h"

using namespace mnslp::msg;
using namespace protlib::log;


/**
 * Constructor.
 */
ntlp_msg::ntlp_msg(const session_id &id, mnslp_msg *body,
		ntlp::mri *m, uint32 sii)
		: sid(id), routing_info(m), sii_handle(sii), msg(body) {

	assert( routing_info != NULL );
	// Note: body may be NULL if deserialize() failed!
}


/**
 * Destructor.
 */
ntlp_msg::~ntlp_msg() {
	delete routing_info;
	delete msg;
}


/**
 * Create an exact copy of an ntlp_msg.
 */
ntlp_msg *ntlp_msg::copy() const {
	return new ntlp_msg(
		get_session_id(), get_mnslp_msg()->copy(),
		get_mri()->copy(), get_sii_handle()
	);
}


/**
 * Create a copy of an ntlp_msg suitable for forwarding.
 *
 * This creates an almost exact copy, but the SII handle is set to zero.
 * This is needed for forwarding a message because the incoming message's
 * SII handle won't be valid for the outgoing link.
 *
 * If the mri_pc parameter is not NULL, it is used instead of the original
 * message's MRI.
 */
ntlp_msg *ntlp_msg::copy_for_forwarding(ntlp::mri *new_mri) const {

	if ( new_mri == NULL )
		new_mri = get_mri()->copy();

	return new ntlp_msg(
		get_session_id(), get_mnslp_msg()->copy(), new_mri, 0);
}


/**
 * Creates an error MNSLP RESPONSE with the given severity and response code.
 */
ntlp_msg *ntlp_msg::create_response(uint8 severity, uint8 response_code) const {

	mnslp_msg *msg = get_mnslp_msg();
	uint16 object_type = 0;

	// invert the MRI
	ntlp::mri *new_mri = get_mri()->copy();
	new_mri->invertDirection();

	mnslp_response *resp = new mnslp_response();
	resp->set_information_code(severity, response_code, object_type);

	if ( msg != NULL && msg->has_msg_sequence_number() )
		resp->set_msg_sequence_number(msg->get_msg_sequence_number());

	return new ntlp_msg(get_session_id(), resp, new_mri, 0);
}


/**
 * Creates an error MNSLP RESPONSE based on the given request_error.
 */
ntlp_msg *ntlp_msg::create_error_response(const request_error &e) const {
	return create_response(e.get_severity(), e.get_response_code());
}


/**
 * Create a MNSLP RESPONSE indicating success.
 */
ntlp_msg *ntlp_msg::create_success_response(
		uint32 session_lifetime) const {

	ntlp_msg *msg = create_response(information_code::sc_success,
		information_code::suc_successfully_processed);

	mnslp_response *resp
		= dynamic_cast<mnslp_response *>(msg->get_mnslp_msg());
	assert( resp != NULL );

	resp->set_session_lifetime(session_lifetime);

	return msg;
}


/**
 * Checks if an MSN is greater than an other one, as per RFC-1982.
 *
 * Note that this algorithm takes integer wrap around into account.
 *
 * @return true if s1 is greater than s2
 */
bool mnslp::msg::is_greater_than(uint32 s1, uint32 s2) {
	const uint32 LIMIT = 2147483648u;		// 2^(32-1)

	return ( s1 != s2 ) 
		&& ( (s1 < s2 && s2-s1 > LIMIT) || (s1 > s2 && s1-s2 < LIMIT) );
}




std::ostream &mnslp::msg::operator<<(std::ostream &out, const ntlp_msg &s) {
	out << "[ntlp_msg: ";

	out << std::endl << "sii = " << s.get_sii_handle() << std::endl;

	if ( s.get_mnslp_msg() != NULL )
		out << *(s.get_mnslp_msg());
	else
		out << "[no body]";

	if ( s.get_mri() != NULL )
		out << std::endl << "mri = " << *(s.get_mri());
	else
		out << std::endl << "mri = [none]";

	return out << "]";
}


// EOF
