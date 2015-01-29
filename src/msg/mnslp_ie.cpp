/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_ie.cpp
/// The MNSLP IE Manager
/// ----------------------------------------------------------
/// $Id: mnslp_ie.cpp 2896 2008-02-21 00:54:43Z bless $
/// $HeadURL: https://./src/msg/mnslp_ie.cpp $
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
#include "msg/mnslp_msg.h"
#include "msg/mnslp_response.h"
#include <bitset>


using namespace mnslp::msg;
using namespace protlib::log;

#define LogError(msg) Log(ERROR_LOG, LOG_NORMAL, "MNSLP_IEManager", msg)
#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, "MNSLP_IEManager", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, "MNSLP_IEManager", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, "MNSLP_IEManager", msg)


/**
 * This is where our single MNSLP_IEManager is stored.
 */
MNSLP_IEManager *MNSLP_IEManager::mnslp_inst = NULL;


/**
 * Constructor for child classes.
 *
 * This constructor has been made protected to only allow instantiation
 * via the static instance() method.
 */
MNSLP_IEManager::MNSLP_IEManager() : IEManager() {
	// nothing to do
}


/**
 * Singleton static factory method.
 *
 * Returns the same MNSLP_IEManager object, no matter how often it is called.
 *
 * Note: Calling clear() causes the object to be deleted and the next call
 * to instance() create a new MNSLP_IEManager object.
 *
 * @return always the same MNSLP_IEManager object
 */
MNSLP_IEManager *MNSLP_IEManager::instance() {

	// Instance already exists, so return it.
	if ( mnslp_inst )
		return mnslp_inst;

	// We don't have an instance yet. Create it.
	try {
		mnslp_inst = new MNSLP_IEManager();
		LogDebug("created MNSLP_IEManager singleton: " << mnslp_inst);
	}
	catch ( bad_alloc ) {
		LogError("cannot create MNSLP_IEManager singleton");
		throw IEError(IEError::ERROR_NO_IEMANAGER);
	}

	return mnslp_inst;
}


/**
 * Delete the singleton instance.
 *
 * After calling this, all pointers to or into the object are invalid.
 * The instance() method has to be called before using the MNSLP_IEManager
 * next time.
 */
void MNSLP_IEManager::clear() {
	
	if ( mnslp_inst != NULL ) {
		delete mnslp_inst;
		mnslp_inst = 0;
		LogDebug("deleted MNSLP_IEManager singleton");
	}
}


/**
 * Register all known IEs.
 *
 * This method clears the registry and then registers all IEs known to this
 * implementation. It solely exists for convenience.
 */
void MNSLP_IEManager::register_known_ies() {
	clear();

	MNSLP_IEManager *inst = instance();

	inst->register_ie(new mnslp_configure());
	inst->register_ie(new mnslp_refresh());
	inst->register_ie(new mnslp_options());
	inst->register_ie(new mnslp_response());
	inst->register_ie(new mnslp_notify());

	// TODO: use a yet to implement mnslp_msg_raw!
	//inst->register_ie(new mnslp_msg()); // the catch-all

	inst->register_ie(new session_lifetime());
	inst->register_ie(new information_code());
	inst->register_ie(new message_hop_count());
	inst->register_ie(new selection_metering_entities());
	inst->register_ie(new msg_sequence_number());
	inst->register_ie(new mnslp_ipfix_message());

	// TODO: implement catch-all

	LogDebug("registered known IEs");
}


IE *MNSLP_IEManager::lookup_ie(uint16 category, uint16 type, uint16 subtype) {
	IE *ret = IEManager::lookup_ie(category, type, subtype);

	if ( ret != NULL )
		return ret;

	/*
	 * No IE registered. Return a default IE if possible.
	 */
	switch ( category ) {
	    case cat_mnslp_msg:
		return IEManager::lookup_ie(cat_default_mnslp_msg, 0, 0);

	    case cat_mnslp_object:
		return IEManager::lookup_ie(cat_default_mnslp_object, 0, 0);

	    default:
		return NULL;
	}

	// not reached
}


/**
 * Parse a PDU in NetMsg and return it.
 *
 * This method parses a PDU from the given NetMsg and returns a new
 * IE object representing the PDU. Based on the category parameter,
 * the IE can be a MNSLP Message or MNSLP Object (that is part of a
 * message).
 *
 * All errors encountered during parsing are added to the errorlist object.
 * If no errors occurred, the errorlist will be in the same state as before
 * the call.
 *
 * @param msg a buffer containing the serialized PDU
 * @param category the category the IE belongs to
 * @param coding the protocol version used in msg
 * @param errorlist returns the exceptions caught while parsing the message
 * @param bytes_read returns the number of bytes read from msg
 * @param skip if true, try to ignore errors and continue reading
 * @return the newly created IE, or NULL on error
 */
IE *MNSLP_IEManager::deserialize(NetMsg &msg, uint16 category,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip) {

	// Note: The registered objects decide if they support a given coding.
	
	switch ( category ) {
		case cat_mnslp_msg:
			return deserialize_msg(msg, coding,
					errorlist, bytes_read, skip);

		case cat_mnslp_object:
			return deserialize_object(msg, coding,
					errorlist, bytes_read, skip);

		default:
			LogError("category " << category << " not supported");

			catch_bad_alloc( errorlist.put(
				new IEError(IEError::ERROR_CATEGORY)) );
			return NULL;
	}

	return NULL;	// not reached
}


/**
 * Use a registered natfw_msg instance to deserialize a NetMsg.
 *
 * Helper method for deserialize(). Parameters work like in deserialize().
 *
 * @param msg a buffer containing the serialized PDU
 * @param coding the protocol version used in msg
 * @param errorlist returns the exceptions caught while parsing the message
 * @param bytes_read returns the number of bytes read from msg
 * @param skip if true, try to ignore errors and continue reading
 * @return the newly created IE, or NULL on error
 */
IE *MNSLP_IEManager::deserialize_msg(NetMsg &msg,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip) {

	/*
	 * Peek ahead to find out the message type.
	 */
	uint32 msg_type;
	try {
		uint32 header_raw = msg.decode32(false); // don't move position
		msg_type = mnslp_msg::extract_msg_type(header_raw);
	}
	catch ( NetMsgError ) {
		errorlist.put(new IEMsgTooShort(
				coding, cat_mnslp_msg, msg.get_pos()) );
		return NULL; // fatal error
	}


	IE *ie = new_instance(cat_mnslp_msg, msg_type, 0);

	if ( ie == NULL ) {
		LogError("no mnslp_msg registered for ID " << msg_type);
		errorlist.put(new IEError(IEError::ERROR_WRONG_TYPE));
		return NULL;
	}

	// ie returns iteself on success
	IE *ret = ie->deserialize(msg, coding, errorlist, bytes_read, skip);

	if ( ret == NULL ){
		delete ie;
	}
	
	return ret;	// the deserialized object on success, NULL on error
}


/**
 * Use a registered msnlp_object instance to deserialize a NetMsg.
 *
 * Helper method for deserialize(). Parameters work like in deserialize().
 *
 * @param msg a buffer containing the serialized PDU
 * @param coding the protocol version used in msg
 * @param errorlist returns the exceptions caught while parsing the message
 * @param bytes_read returns the number of bytes read from msg
 * @param skip if true, try to ignore errors and continue reading
 * @return the newly created IE, or NULL on error
 */
IE *MNSLP_IEManager::deserialize_object(NetMsg &msg,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip) {

	/*
	 * Peek ahead to find out the MNSLP Object Type.
	 */
	uint32 object_type;
	try {
		uint32 header_raw = msg.decode32(false); // don't move position
		object_type = mnslp_object::extract_object_type(header_raw);
	}
	catch ( NetMsgError ) {
		errorlist.put(new IEMsgTooShort(
				coding, cat_mnslp_object, msg.get_pos()) );
		return NULL; // fatal error
	}

	IE *ie = new_instance(cat_mnslp_object, object_type, 0);

	if ( ie == NULL ) {
		std::cout << "wrong object_type" << std::endl;
		LogError("no mnslp_object registered for ID " << object_type);
		errorlist.put(new IEError(IEError::ERROR_WRONG_SUBTYPE));
		return NULL;
	}

	// ie returns iteself on success
	IE *ret = ie->deserialize(msg, coding, errorlist, bytes_read, skip);

	if ( ret == NULL )
		delete ie;

	return ret;	// the deserialized object on success, NULL on error
}


// EOF
