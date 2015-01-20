/// ----------------------------------------*- mode: C++; -*--
/// @file gistka_mapper.cpp
/// Utilities for mapping between GISTka/protlib and M-NSLP.
/// ----------------------------------------------------------
/// $Id: gistka_mapper.cpp 3093 2014-11-07 16:50:00Z bless $
/// $HeadURL: https://./src/gistka_mapper.cpp $
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
#include "timer_module.h"
#include "logfile.h"

#include "apimessage.h"		// from NTLP

#include "mnslp_config.h"
#include "msg/mnslp_ie.h"
#include "events.h"
#include "gistka_mapper.h"
#include "benchmark_journal.h"


using namespace mnslp;
using namespace mnslp::msg;
using namespace protlib::log;


#define LogError(msg) ERRLog("gistka_mapper", msg)
#define LogWarn(msg) WLog("gistka_mapper", msg)
#define LogInfo(msg) ILog("gistka_mapper", msg)
#define LogDebug(msg) DLog("gistka_mapper", msg)

#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, "gistka_mapper", \
	msg << " at " << __FILE__ << ":" << __LINE__)

#ifdef BENCHMARK
  extern benchmark_journal journal;
#endif


/*
 * Create an NTLP APIMsg from our ntlp_msg.
 */
ntlp::APIMsg *gistka_mapper::create_api_msg(msg::ntlp_msg *msg) const throw () {
	using ntlp::nslpdata;

	MP(benchmark_journal::PRE_SERIALIZE);

	/*
	 * Construct the NSLP payload (the NTLP's body).
	 */
	const mnslp_msg *m = msg->get_mnslp_msg();
	NetMsg payload( m->get_serialized_size(IE::protocol_v1) );

	try {
		uint32 bytes_written;
		m->serialize(payload, IE::protocol_v1, bytes_written);
	}
	catch ( IEError &e ) {
		LogError("serializing M-NSLP message failed");
		assert( false ); // this would be a programming error
	}

	// Note: The nslpdata constructor copies the buffer.
	nslpdata *data = new nslpdata(payload.get_buffer(), payload.get_size());


	/*
	 * Gather all data the NTLP needs for sending our message.
	 */
	uint32 msg_handle = 0;		// NSLP message handle
	uint32 nslp_id = mnslp_config::NSLP_ID;

	ntlp::sessionid *sid = create_ntlp_session_id(msg->get_session_id());

	ntlp::mri *nslp_mri = msg->get_mri()->copy();
	assert( nslp_mri != NULL );

	uint32 sii_handle = msg->get_sii_handle();

	// reliable, but not secure; final hop flag is irrelevant
	ntlp::tx_attr_t attr;
	attr.reliable= true;
	attr.secure= false;

	uint32 timeout = mnslp_config::SEND_MSG_TIMEOUT;
	uint16 ip_ttl = mnslp_config::SEND_MSG_IP_TTL;
	uint32 ntlp_hop_count = mnslp_config::SEND_MSG_GIST_HOP_COUNT;

	/*
	 * Now construct a message from the data above and hand it off to the
	 * NTLP.
	 */
	ntlp::APIMsg *apimsg = new ntlp::APIMsg();
	apimsg->set_source(mnslp_config::INPUT_QUEUE_ADDRESS);
	apimsg->set_sendmessage(data, msg_handle, nslp_id, sid, nslp_mri,
							sii_handle, attr, timeout, ip_ttl, ntlp_hop_count);

	MP(benchmark_journal::POST_SERIALIZE);

	return apimsg;
}


/**
 * Map APIMsg objects to event objects.
 */
event *gistka_mapper::map_to_event(const protlib::message *msg) const {
	using ntlp::APIMsg;

	event *ret = NULL;

	assert( msg != NULL );

	MP(benchmark_journal::PRE_MAPPING);

	if ( dynamic_cast<const APIMsg *>(msg) != NULL )
		ret = map_api_message(dynamic_cast<const APIMsg *>(msg));
		
	else if ( dynamic_cast<const mnslp_timer_msg *>(msg) != NULL )
		ret = map_timer_message(
			dynamic_cast<const mnslp_timer_msg *>(msg));
			
	else if ( dynamic_cast<const mnslp_event_msg *>(msg) != NULL ) 
	{
		const mnslp_event_msg *em
			= dynamic_cast<const mnslp_event_msg *>(msg);
		ret = em->get_event();
	}
	else
		LogError("received unknown protlib::message of type "
			<< msg->get_type_name() << " from "
			<< msg->get_qaddr_name());

	if ( ret == NULL )
		LogWarn("map_to_event(): mapping not possible");

	MP(benchmark_journal::POST_MAPPING);

	return ret;
}


event *gistka_mapper::map_api_message(const ntlp::APIMsg *msg) const 
{
	using ntlp::APIMsg;

	switch ( msg->get_subtype() ) {
		case APIMsg::RecvMessage:
			return map_api_receive_message(msg);
			break;

		case APIMsg::NetworkNotification:
			return map_api_network_notification(msg);
			break;

		case APIMsg::MessageStatus:
			return map_api_message_status(msg);
			break;

		default:
			LogError("map_to_event(): can't handle type "
					<< msg->get_subtype_name());
			break;
	}


	return NULL;
}


/**
 * Create a msg_event from the given APIMsg.
 *
 * This is called each time the NTLP gets a MNSLP message from the network.
 */
event *gistka_mapper::map_api_receive_message(
		const ntlp::APIMsg *apimsg) const 
{

	using namespace protlib;
	using ntlp::APIMsg;
	using ntlp::nslpdata;

	assert( apimsg->get_subtype() == APIMsg::RecvMessage );


	// Convert the NTLP session ID to our local session_id.
	session_id *sid = create_mnslp_session_id(apimsg->get_sessionid());


	/*
	 * The initial Query (a UDP datagram) does not contain a payload,
	 * because we requested reliable mode. So data can be NULL.
	 */
	nslpdata *data = apimsg->get_data();

	if ( data == NULL ) {
		LogInfo("received message contains no NSLP payload");

		// The NTLP instance asks us if we want to setup routing state.
		if ( apimsg->get_adjacency_check() == true ) {
			LogInfo("routing state check flag set");
			return new routing_state_check_event(
				sid, apimsg->get_mri()->copy());
		}
		else {
			// This could be triggered by GIST-Confirm in C-Mode
			// I don't think we have to do anything here.
			LogInfo("routing state check flag NOT set");
			LogDebug("SII-Handle " << apimsg->get_sii_handle());
			delete sid;
			return NULL;
		}
	}


	/*
	 * Parse the NSLP payload (the NTLP's body).
	 */
	MP(benchmark_journal::PRE_DESERIALIZE);
	NetMsg payload(data->get_buffer(), data->get_size()); // copies the data
	MNSLP_IEManager *mgr = MNSLP_IEManager::instance();

	IEErrorList errlist;
	uint32 num_read;

	IE *ie = mgr->deserialize(payload, cat_mnslp_msg, IE::protocol_v1,
		errlist, num_read, false);

	if ( ie == NULL ) {
		assert( ! errlist.is_empty() );
		LogError("deserializing NATFW message failed: "
				<< *(errlist.get()) );
		LogDebug("nslp-data (the NATFW message) payload: " << payload);
	}

	/*
	 * With the deserialize() call above, the MNSLP_IEManager can only
	 * create subclasses of mnslp_msg. If it doesn't something has gone
	 * wrong with the IE registration, which would be a programming error.
	 */
	mnslp_msg *m = NULL;

	if ( ie != NULL ) {
		m = dynamic_cast<mnslp_msg *>(ie);
		assert( m != NULL );
	}

	uint32 sii = apimsg->get_sii_handle();
	bool final_hop = apimsg->get_tx_attr().final_hop;

	// Note that m may be NULL if deserialize() failed!
	ntlp_msg *ret = new ntlp_msg(*sid, m, apimsg->get_mri()->copy(), sii);
	msg_event *evt = new msg_event(sid, ret, final_hop);

	MP(benchmark_journal::POST_DESERIALIZE);

	return evt;
}


event *gistka_mapper::map_api_network_notification(
		const ntlp::APIMsg *msg) const 
{

	using ntlp::APIMsg;

	assert( msg->get_subtype() == APIMsg::NetworkNotification );

	APIMsg::error_t status = msg->get_msgstatus();

	LogDebug("NetworkNotification: NSLPID " << msg->get_nslpid());
	LogDebug("NetworkNotification: SessionID " << *msg->get_sessionid());
	LogDebug("NetworkNotification: MRI " << *msg->get_mri());
	LogDebug("NetworkNotification: SII-Handle " << msg->get_sii_handle());
	LogDebug("NetworkNotification: notify value "
		<< status << " (" << msg->get_error_str(status) << ")");

	switch ( status ) {
	  case APIMsg::route_changed_status_bad:
		return new route_changed_bad_event(
			create_mnslp_session_id(msg->get_sessionid()) );
		break;

	  default:
		LogUnimp("unhandled NetworkNotification, status "
			<< status << " (" << msg->get_error_str(status) << ")");
	}

	return NULL;
}


/*
 * Using this message, we could discover problems like unreachable hosts 
 * earlier. But we can as well resend a few times and give up then.
 */
event *gistka_mapper::map_api_message_status(const ntlp::APIMsg *msg) const 
{
	using ntlp::APIMsg;

	assert( msg->get_subtype() == APIMsg::MessageStatus );

	APIMsg::error_t status = msg->get_msgstatus();

	LogDebug("MessageStatus:"
		<< " NSLP-Message-Handle=" << msg->get_nslpmsghandle()
		//<< " Transfer-Attributes=" << msg->get_tx_attr()
		<< " Error-Type=" << msg->get_error_str(status));

	// If no session ID is available, we don't create an event.
	if ( msg->get_sessionid() == NULL ) {
		LogDebug("MessageStatus has no SessionID");
		return NULL;
	}

	if ( status == APIMsg::error_msg_node_not_found )
		return new no_next_node_found_event(
			create_mnslp_session_id(msg->get_sessionid()) );

	LogError("MessageStatus error code unknown");

	return NULL;
}


event *gistka_mapper::map_timer_message(const mnslp_timer_msg *msg) const 
{
	assert( msg != NULL );

	timer_event *e = new timer_event(
		new session_id(msg->get_session_id()), msg->get_id());

	return e;
}


/**
 * Convert our local session_id to an ntlp::sessionid.
 */
ntlp::sessionid *gistka_mapper::create_ntlp_session_id(
		const session_id &s) const 
{

	uint128 raw = s.get_id();
	return  new ntlp::sessionid(raw.w1, raw.w2, raw.w3, raw.w4);
}


/**
 * Convert an ntlp::sessionid to our local session_id.
 */
session_id *gistka_mapper::create_mnslp_session_id(ntlp::sessionid *sid) const 
{
	assert( sid != NULL );

	uint128 raw_sid;
	sid->get_sessionid(raw_sid.w1, raw_sid.w2, raw_sid.w3, raw_sid.w4);

	return new session_id(raw_sid);
}

// EOF
