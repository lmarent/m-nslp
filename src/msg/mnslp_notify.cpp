/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_notify.cpp
/// Implementation of the MNSLP NOTIFY Message.
/// ----------------------------------------------------------
/// $Id: mnslp_notify.cpp 2558 2014-11-05 $
/// $HeadURL: https://./src/msg/mnslp_notify.cpp $
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

#include "msg/mnslp_ie.h"
#include "msg/ie_object_key.h"
#include "msg/mnslp_notify.h"
#include "msg/mnslp_object.h"


using namespace mnslp::msg;
using namespace protlib::log;


/**
 * Constructor.
 *
 * Only basic initialization is done. No MNSLP objects exist yet. All other
 * attributes are set to default values.
 */
mnslp_notify::mnslp_notify() : mnslp_msg(MSG_TYPE) {
	// nothing to do
}


/**
 * Copy constructor.
 *
 * Makes a deep copy of the object passed as an argument.
 *
 * @param other the object to copy
 */
mnslp_notify::mnslp_notify(const mnslp_notify &other)
		: mnslp_msg(other) {
	
	// nothing else to do
}


/**
 * Destructor.
 *
 * Deletes all objects this message contains.
 */
mnslp_notify::~mnslp_notify() {
	// Nothing to do, parent class handles this.
}


mnslp_notify *mnslp_notify::new_instance() const {
	mnslp_notify *inst = NULL;
	catch_bad_alloc(inst = new mnslp_notify());
	return inst;
}


mnslp_notify *mnslp_notify::copy() const {
	mnslp_notify *copy = NULL;
	catch_bad_alloc(copy = new mnslp_notify(*this));
	return copy;
}


void mnslp_notify::serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError) {

	
	const IE *obj;
	uint32 obj_bytes_written;
	uint32 start_pos = msg.get_pos();
	/* 
	 * Write the header.
	 */
	mnslp_msg::serialize(msg, coding, bytes_written);
	
	/*
	 * Write the body: Serialize each object.
	 */
	ie_object_key key_inf(information_code::OBJECT_TYPE, 1);
	bytes_written += serialize_object(key_inf, msg, coding);
	

	// this would be an implementation error
	if ( bytes_written != msg.get_pos() - start_pos )
		Log(ERROR_LOG, LOG_CRIT, "mnslp_msg",
				"serialize(): byte count mismatch");
	
}

uint32 
mnslp_notify::serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const
{
	uint32 obj_bytes_written = 0;
	mnslp_object *obj = get_object(key);
	if (obj != NULL){
		obj->serialize(msg, coding, obj_bytes_written);
		return obj_bytes_written;
	}
	else{
		// Throwns an exception.
	}
}



bool mnslp_notify::check() const {
	
	// Verifies that information code exists.
	ie_object_key key_inf(information_code::OBJECT_TYPE, 1);
	mnslp_object *obj= get_object(key_inf);
	if (obj == NULL)
		return false;
	
	if (get_num_objects() != 1 )
		return false;
	
	return true;

}

void mnslp_notify::register_ie(IEManager *iem) const {
	iem->register_ie(cat_mnslp_msg, get_msg_type(), 0, this);
}


/**
 * Sets the information code.
 *
 * This is used to indicate success or various degrees of failure.
 *
 * @param severity the severity class (4 bit)
 * @param response_code a response code, depending on the severity class (8 bit)
 * @param object_type the object type the response is about (16 bit)
 */
void mnslp_notify::set_information_code(uint8 severity, uint8 response_code,
                uint16 object_type) {

	set_object(new information_code(severity, response_code, object_type,
		mnslp_object::tr_mandatory, true));
}


/**
 * Return the object type that triggered this response message.
 *
 * @return a MNSLP object type (16 bit)
 */
uint16 mnslp_notify::get_object_type() const {
	
	ie_object_key key (information_code::OBJECT_TYPE, 1);
	information_code *ic = dynamic_cast<information_code *>(
		get_object(key));

	if ( ic == NULL )
		return 0; // TODO: use a default constant here?
	else
		return ic->get_object_type();
}


/**
 * Return the severity class code.
 *
 * @return the severity class as defined in information_code
 */
uint8 mnslp_notify::get_severity_class() const {
	
	ie_object_key key (information_code::OBJECT_TYPE, 1);
	information_code *ic = dynamic_cast<information_code *>(
		get_object(key));

	if ( ic == NULL )
		return 0; // TODO: use a default constant here?
	else
		return ic->get_severity_class();
}

/**
 * Return the response code.
 *
 * @return the response code as defined in information_code
 */
uint8 mnslp_notify::get_response_code() const {
	
	ie_object_key key (information_code::OBJECT_TYPE, 1);
	information_code *ic = dynamic_cast<information_code *>(
		get_object(key));

	if ( ic == NULL )
		return 0; // TODO: use a default constant here?
	else
		return ic->get_response_code();
}

/**
 * Return the response object type.
 *
 * @return the response object type as defined in information_code
 */
uint16 
mnslp_notify::get_response_object_type() const {
	
	ie_object_key key (information_code::OBJECT_TYPE, 1);
	information_code *ic = dynamic_cast<information_code *>(
		get_object(key));

	if ( ic == NULL )
		return 0; // TODO: use a default constant here?
	else
		return ic->get_response_object_type();
}

// EOF
