/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_configure.cpp
/// Implementation of the MNSLP CONFIGURE Message.
/// ----------------------------------------------------------
/// $Id: mnslp_configure.cpp 2896 2014-11-05 $
/// $HeadURL: https://./src/msg/mnslp_configure.cpp $
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
#include <openssl/rand.h>

#include "logfile.h"

#include "msg/mnslp_ie.h"
#include "msg/ie_object_key.h"
#include "msg/mnslp_configure.h"
#include "msg/mnslp_object.h"


using namespace mnslp::msg;
using namespace protlib::log;


/**
 * Constructor.
 *
 * Only basic initialization is done. No MNSLP objects exist yet. All other
 * attributes are set to default values.
 *
 * @param proxy_mode set to true, if proxy mode is enabled
 */
mnslp_configure::mnslp_configure()
		: mnslp_msg(mnslp_configure::MSG_TYPE) {

	// nothing to do
}


/**
 * Copy constructor.
 *
 * Makes a deep copy of the object passed as an argument.
 *
 * @param other the object to copy
 */
mnslp_configure::mnslp_configure(const mnslp_configure &other)
		: mnslp_msg(other) {
	
	// nothing else to do
}


/**
 * Destructor.
 *
 * Deletes all objects this message contains.
 */
mnslp_configure::~mnslp_configure() {
	// Nothing to do, parent class handles this.
}


mnslp_configure *mnslp_configure::new_instance() const {
	mnslp_configure *inst = NULL;
	catch_bad_alloc(inst = new mnslp_configure());
	return inst;
}


mnslp_configure *mnslp_configure::copy() const {
	mnslp_configure *copy = NULL;
	catch_bad_alloc(copy = new mnslp_configure(*this));
	return copy;
}

void mnslp_configure::serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError) {

	
	if ( check() ){ 
		const IE *obj;
		uint32 obj_bytes_written;
		uint32 start_pos = msg.get_pos();
		/* 
		 * Write the header.
		 */
		mnslp_msg::serialize(msg, coding, bytes_written);
		
		uint32 start_pos2 = msg.get_pos();

		/*
		 * Write the body: Serialize each object. According with the RFC
		 * Order of object is important. So we give the correct order here.
		 */
		ie_object_key key_msn(msg_sequence_number::OBJECT_TYPE, 0);
		bytes_written += serialize_object(key_msn, msg, coding);
		ie_object_key key_slf(session_lifetime::OBJECT_TYPE, 0);
		bytes_written += serialize_object(key_slf, msg, coding);
		ie_object_key key_sme(selection_metering_entities::OBJECT_TYPE, 0);
		bytes_written += serialize_object(key_sme, msg, coding);
		ie_object_key key_mhc(message_hop_count::OBJECT_TYPE, 0);
		bytes_written += serialize_object(key_mhc, msg, coding);
		
		/*
		 * Write the body: Serialize ipfix message.
		 */
		uint32 max_seq_nbr = objects.getMaxSequence(mnslp_ipfix_message::OBJECT_TYPE);
		
		for ( uint32 i = 0; i <= max_seq_nbr; i++ ) {
			ie_object_key key_ipfix(mnslp_ipfix_message::OBJECT_TYPE, i);
			bytes_written += serialize_object(key_ipfix, msg, coding);
		}
        
		// this would be an implementation error
		if ( bytes_written != msg.get_pos() - start_pos )
			Log(ERROR_LOG, LOG_CRIT, "mnslp_msg",
					"serialize(): byte count mismatch");
	}
	else
	{
		
		throw IEError(IEError::ERROR_OBJ_SET_FAILED);
		Log(ERROR_LOG, LOG_CRIT, "mnslp_msg",
					"serialize(): Data missing for the message");
	}
	
}

uint32 
mnslp_configure::serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const
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

bool mnslp_configure::check() const {
	
	bool message_included = false;
	bool sequence_included = false;
	bool sel_meter_ent = false;
	bool session_lt = false;
	bool msg_hop_count = false;
	
	// Check all objects for errors.
	for ( obj_iter i = objects.begin(); i != objects.end(); i++ ) {
		ie_object_key key = i->first;

		if (key.get_object_type() == msg_sequence_number::OBJECT_TYPE ){ 
			sequence_included = true;
		}
		if (key.get_object_type() == selection_metering_entities::OBJECT_TYPE ){
			sel_meter_ent = true;
		}
		if (key.get_object_type() == session_lifetime::OBJECT_TYPE ){
			session_lt = true;
		}
		if (key.get_object_type() == message_hop_count::OBJECT_TYPE ){
			msg_hop_count = true;
		}
		
	    if ( key.get_object_type() == mnslp_ipfix_message::OBJECT_TYPE ){
			message_included = true;
		}
	}
				  
	return message_included & sequence_included & sel_meter_ent & session_lt & msg_hop_count; // no error found
}

void mnslp_configure::register_ie(IEManager *iem) const {
	iem->register_ie(cat_mnslp_msg, get_msg_type(), 0, this);
}


/**
 * Set the desired session lifetime.
 *
 * @param seconds the session lifetime in milliseconds
 */
void mnslp_configure::set_session_lifetime(uint32 milliseconds) {
	set_object(new session_lifetime(milliseconds, mnslp_object::tr_mandatory, true));
}


/**
 * Return the desired session lifetime.
 *
 * @return the session lifetime in milliseconds
 */
uint32 mnslp_configure::get_session_lifetime() const {
	
	ie_object_key key(session_lifetime::OBJECT_TYPE, 0);
	
	session_lifetime *lt = dynamic_cast<session_lifetime *>(
		get_object(key));

	if ( lt == NULL )
		return 0; // TODO: use a default constant here?
	else
		return lt->get_value();
}




/**
 * Sets the message sequence number.
 *
 * See the msg_sequence_number object for more information.
 *
 * @param msn the value of the message sequence number
 */
void mnslp_configure::set_msg_sequence_number(uint32 msn) {
	set_object(new msg_sequence_number(msn, mnslp_object::tr_mandatory, true));
}

/**
 * Return the desired message sequence number.
 *
 * @return the message sequence number
 */
uint32 mnslp_configure::get_msg_sequence_number() const {
	
	ie_object_key key(msg_sequence_number::OBJECT_TYPE, 0);
	
	msg_sequence_number *lt = dynamic_cast<msg_sequence_number *>(
		get_object(key));

	if ( lt == NULL )
		return 0; // TODO: use a default constant here?
	else
		return lt->get_value();
}


/**
 * Set the selection metering entities.
 * *
 * @param value the selection metering entities value
 */
void mnslp_configure::set_selection_metering_entities(uint32 value) {
	set_object(new selection_metering_entities(value, mnslp_object::tr_mandatory, true));
}

/**
 * Return the selection metering entities.
 *
 *
 * @return the selection metering entities's value
 */
uint32 mnslp_configure::get_selection_metering_entities() const {
	
	ie_object_key key(selection_metering_entities::OBJECT_TYPE, 0);
	
	selection_metering_entities *sme = dynamic_cast<selection_metering_entities *>(
		get_object(key));

	if ( sme == NULL )
		return 0; // TODO: use a default constant here?
	else
		return sme->get_value();
}


/**
 * Set the message hop count.
 * *
 * @param value the message hop count value
 */
void mnslp_configure::set_message_hop_count(uint32 value) {
	set_object(new message_hop_count(value, mnslp_object::tr_mandatory, true));
}

/**
 * Return the message hop count.
 *
 *
 * @return the message hop count's value
 */
uint32 mnslp_configure::get_message_hop_count() const {
	
	ie_object_key key(message_hop_count::OBJECT_TYPE, 0);
	
	message_hop_count *mhc = dynamic_cast<message_hop_count *>(
		get_object(key));

	if ( mhc == NULL )
		return 0; // TODO: use a default constant here?
	else
		return mhc->get_value();
}

/**
 * Set an ipfix message for the configuration message. According with the RFC 
 * it is important the order of the message, so the order is given by the order in which
 * it is inserted. In Other words, the first message inserted is the number one message and so on.
 * *
 * @param message object to be inserted.
 */
void mnslp_configure::set_mspec_object(mnslp_mspec_object *message)
{
	set_object(message);
}

void mnslp_configure::get_mspec_objects(std::vector<mnslp_mspec_object *> &list_return)
{
	for ( obj_iter i = objects.begin(); i != objects.end(); i++ ) {
		const ie_object_key key = i->first;
		const mnslp_mspec_object *obj = dynamic_cast<const mnslp_mspec_object *>( i->second);
		
		if ((key.get_object_type() != message_hop_count::OBJECT_TYPE) 
		    and (key.get_object_type() != message_hop_count::OBJECT_TYPE)
		    and (key.get_object_type() != selection_metering_entities::OBJECT_TYPE) 
		    and (key.get_object_type() != msg_sequence_number::OBJECT_TYPE)
		    and (key.get_object_type() != session_lifetime::OBJECT_TYPE)){
				
			list_return.push_back(obj->copy());
		}
	}
}


// EOF
