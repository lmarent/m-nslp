/// ----------------------------------------*- mode: C++; -*--
/// @file ni_session.cpp
/// The session for an NSIS Initiator.
/// ----------------------------------------------------------
/// $Id: ni_session.cpp 3165 2014-11-08 10:01:00 amarentes $
/// $HeadURL: https://./src/ni_session.cpp $
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
#include <cmath>
#include <sstream>
#include <openssl/rand.h>

#include "logfile.h"

#include "mnslp_config.h"
#include "events.h"
#include "msg/mnslp_msg.h"
#include "dispatcher.h"
#include "ni_session.h"
#include <iostream>


using namespace mnslp;
using namespace protlib::log;
using protlib::uint32;


#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, "ni_session", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, "ni_session", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, "ni_session", msg)



/**
 * Constructor.
 *
 * Use this if the session ID is known in advance.
 */
ni_session::ni_session(const session_id &id, const mnslp_config *conf)
		: session(id), state(STATE_CLOSE), routing_info(NULL),
		  last_configure_msg(NULL), last_refresh_msg(NULL), lifetime(0),
		  refresh_interval(20), response_timeout(0), configure_counter(0),
		  refresh_counter(0), max_retries(0), proxy_session(false),
		  response_timer(this), refresh_timer(this) {

	set_session_type(st_initiator);
	set_msg_sequence_number(create_random_number());

	assert( conf != NULL );
	set_response_timeout(conf->get_ni_response_timeout());
	set_max_retries(conf->get_ni_max_retries());
	set_msg_hop_count(conf->get_ni_msg_hop_count());
	
}


/**
 * Constructor for test cases.
 *
 * @param s the state to start in
 */
ni_session::ni_session(state_t s)
		: session(), state(s), routing_info(NULL),
		  last_configure_msg(NULL), last_refresh_msg(NULL), lifetime(30),
		  refresh_interval(20), response_timeout(2), configure_counter(0),
		  refresh_counter(0), max_retries(3), proxy_session(false),
		  response_timer(this), refresh_timer(this) {

	set_session_type(st_initiator);
	set_msg_sequence_number(create_random_number());

	// for testing, we create an empty MRI
	routing_info = new ntlp::mri_pathcoupled();
}


/**
 * Destructor.
 */
ni_session::~ni_session() {
	delete routing_info;
	delete last_configure_msg;
}


/**
 * Generate a 32 Bit random number.
 */
uint32 ni_session::create_random_number() const 
{
	unsigned value;
	int ret = RAND_bytes((unsigned char *) &value, sizeof(value));
	assert( ret == 1 );

	return value;
}


std::ostream &mnslp::operator<<(std::ostream &out, const ni_session &s) 
{
	static const char *const names[] = { "CLOSE", "PENDING_FORWARD", 
										 "PENDING_PARTICIPATING", 
										 "METERING_FORWARD", 
										 "METERING_PARTICIPATING" };

	return out << "[ni_session: id=" << s.get_id()
			   << ", state=" << names[s.get_state()] << "]";
}


/**
 * Build a MNSLP Configure message based the api configure event.
 *  This function makes a deep copy of the mspec_objects. So the session
 *  object can destroy them. It takes lifetime and 
 *
 * This will fetch the session lifetime and also increment the MSN.
 */
msg::ntlp_msg *ni_session::build_configure_message(api_configure_event *e, 
   						   std::vector<msg::mnslp_mspec_object *> & missing_objects) 
{
	using namespace mnslp::msg;

	assert( get_mri() != NULL );

	/*
	 * Build a MNSLP Configure message.
	 */
	mnslp_configure *configure = new mnslp_configure();

	if (get_lifetime() <= e->get_session_lifetime())
		configure->set_session_lifetime(get_lifetime());
	else
		configure->set_session_lifetime(e->get_session_lifetime());	
		
	configure->set_msg_sequence_number(next_msg_sequence_number());
	configure->set_selection_metering_entities(e->get_selection_metering_entities());
	configure->set_message_hop_count(get_msg_hop_count());

	/*
	 * Insert missing objects to install.
	 */
	std::vector<msg::mnslp_mspec_object *>::iterator it_objects;
	for ( it_objects = missing_objects.begin(); 
			it_objects != missing_objects.end(); it_objects++ )
	{
		configure->set_mspec_object((*it_objects)->copy());
	}

	/*
	 * Wrap the Configure inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), configure, get_mri()->copy(), 0);

	return msg;
}


/**
 * Build a MNSLP Refresh message based on the session's state.
 *
 * This will fetch the session lifetime and also increment the MSN.
 */
msg::ntlp_msg *ni_session::build_refresh_message() 
{
	using namespace mnslp::msg;

	assert( get_mri() != NULL );

	/*
	 * Build a MNSLP Configure message.
	 */
	mnslp_refresh *refresh = new mnslp_refresh();
	refresh->set_session_lifetime(get_lifetime());
	refresh->set_msg_sequence_number(next_msg_sequence_number());
	
	/*
	 * Wrap the Configure inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), refresh, get_mri()->copy(), 0);

	return msg;
}


void 
ni_session::setup_session(dispatcher *d, 
	          	  api_configure_event *e, 
			  std::vector<msg::mnslp_mspec_object *> &missing_objects) 
{
	
	/*
	 * Create the message routing information (MRI) which we're going to
	 * use for all messages.
	 */
	uint8 src_prefix = 32;
	uint8 dest_prefix = 32;
	uint16 flow_label = 0;
	uint16 traffic_class = 0;		// DiffServ CodePoint
	uint32 ipsec_spi = 0;			// IPsec SPI
	bool downstream = true;

	ntlp::mri *nslp_mri = new ntlp::mri_pathcoupled(
		e->get_source_address(), src_prefix,
		e->get_source_port(),
		e->get_destination_address(), dest_prefix,
		e->get_destination_port(),
		e->get_protocol(), flow_label, traffic_class, ipsec_spi,
		downstream
	);

	set_mri(nslp_mri);

	if ( e->get_session_lifetime() != 0 )
		set_lifetime(e->get_session_lifetime());

	uint32 r = (uint32) ceil(
		get_lifetime() / ( (get_max_retries()+0.5) * 1.5 ) );
	assert( r > 0 );

	set_refresh_interval(r); 

	uint32 t = (int) ceil( ( ( (rand() % 1000) / 1000.0) + 0.5 ) * r );

	set_response_timeout( t );

	// Check which metering object could be installed in this node.
	std::vector<msg::mnslp_mspec_object *>::const_iterator it_objects;
	for ( it_objects = e->get_metering_objects().begin(); 
			it_objects != e->get_metering_objects().end(); it_objects++)
	{
		mnslp_mspec_object *object = *it_objects;
		if (check_participating(e->get_selection_metering_entities())){
			if (d->check(object))
			{
			   rule->set_object(object->copy());
			}
			else
			{
			   missing_objects.push_back(object->copy());
			}
		}
		else
        {
		    missing_objects.push_back(object->copy());
		}
	}
	
	LogDebug("using lifetime: " << get_lifetime() << " seconds");
	LogDebug("using refresh interval: " << r << " seconds");
	LogDebug("using response timeout: " << t << " seconds");

}

/****************************************************************************
 *
 * state machine part
 *
 ****************************************************************************/


/*
 * state: STATE_CONFIGURE
 */
ni_session::state_t ni_session::handle_state_close(dispatcher *d, event *evt) 
{
	using msg::mnslp_configure;
	std::vector<msg::mnslp_mspec_object *> missing_objects;

	LogDebug("Initiating state handle_state_close ");

	/*
	 * API Create event received.
	 */	
	if ( is_api_configure(evt) ) {
		api_configure_event *e = dynamic_cast<api_configure_event *>(evt);
		
		// Initialize the session based on user-provided parameters
		setup_session(d, e, missing_objects);

		set_last_configure_message( build_configure_message(e, missing_objects) );

		d->send_message( get_last_configure_message()->copy() );
		
		response_timer.start(d, get_response_timeout());

		if ( e->get_return_queue() != NULL ) {
			message *m = new mnslp_event_msg(get_id(), NULL);
			e->get_return_queue()->enqueue(m);
		}

		// Release the memory allocated to the missing objects vector
		std::vector<msg::mnslp_mspec_object *>::iterator it_objects;
		for ( it_objects = missing_objects.begin(); 
			it_objects != missing_objects.end(); it_objects++)
		{
			delete(*it_objects);
		}
		missing_objects.clear();

		return STATE_PENDING;
		
	}
	/*
	 * An invalid event has been received.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_CLOSE;
	}
}


/*
 * state: STATE_PENDING
 */
ni_session::state_t ni_session::handle_state_pending(
		dispatcher *d, event *evt) {

	
	LogDebug("Initiating state pending ");
	using namespace mnslp::msg;

	/*
	 * A response timeout was triggered.
	 */
	if ( is_timer(evt, response_timer) ) {

		// Retry. Send the Configure message again and start a new timer.
		if ( get_configure_counter() < get_max_retries() ) {
			LogWarn("response timeout, restarting timer.");
			inc_configure_counter();
			d->send_message( get_last_configure_message()->copy() );

			response_timer.start(d, get_response_timeout());

			return STATE_PENDING; // no change
		}
		// Retry count exceeded, abort.
		else {
			d->report_async_event("got no response for our Configure");
			return STATE_CLOSE;
		}
	}
	/*
	 * The NTLP can't reach the destination.
	 */
	else if ( is_no_next_node_found_event(evt) ) {
		LogInfo("cannot reach destination");

		return STATE_CLOSE;
	}
	
	/*
	 * API teardown event received.
	 */
	else if ( is_api_teardown(evt) ) {
		LogDebug("received API teardown event");

		// Send a Refresh message with a session lifetime of 0.
		set_lifetime(0);
		set_last_configure_message(NULL);

		// Uninstall the previous rules.
		if (rule->get_number_rule_keys() > 0)
			d->remove_policy_rules(rule);
			
		d->send_message( build_refresh_message() );

		return STATE_CLOSE;
	}
	
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) {
		return STATE_PENDING; // no change
	}
	
	/*
	 * A msg_event arrived which contains a MNSLP RESPONSE message.
	 */
	else if ( is_mnslp_response(evt, get_last_configure_message()) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		mnslp_response *resp = e->get_response();

		LogDebug("received response " << *resp);

		// Discard if this is no RESPONSE to our original CONFIGURE.
		mnslp_configure *c = get_last_configure_message()->get_mnslp_configure();
		if ( ! resp->is_response_to(c) ) {
			LogWarn("RESPONSE doesn't match CONFIGURE, discarding");
			return STATE_PENDING; // no change
		}

		if ( resp->is_success() ) {
			
			LogDebug("initiated session " << get_id());
			d->report_async_event("CONFIGURE session initiated");
			response_timer.stop();
			
			// Check whether someone in the path change the initial lifetime
			if (resp->get_session_lifetime() != get_lifetime())
			{
				set_lifetime(resp->get_session_lifetime());
				cal_refresh_interval();
			}
			
			refresh_timer.start(d, get_refresh_interval());
	
			mt_policy_rule * result = d->install_policy_rules(rule);
			
			// Verify that every rule that passed the checking process could be installed.
			if (result->get_number_mspec_objects() == rule->get_number_mspec_objects() )
			{
				std::cout << "Policy rules installed" 
						  << result->get_number_mspec_objects() << std::endl;
				// free the space allocated to the rule to be installed.
				delete(rule);
				// Assign the response as the rule installed.
				rule = result;
				set_configure_counter(0);
				return STATE_METERING;
				  
			}
			else{
				
				std::cout << "Error installing the policy rule" << std::endl;
				// teardown the session because an error occurs
				set_lifetime(0);
				set_last_configure_message(NULL);
				delete(rule);
				// Assign the response as the rule installed.
				rule = result;

				// Uninstall the previous rules.
				if (rule->get_number_rule_keys() > 0)
					d->remove_policy_rules(rule);
				rule = result;
				d->send_message( build_refresh_message());
			}
		}
		else {
			d->report_async_event("cannot initiate Configure session");
			return STATE_CLOSE;
		}
	}
	
	/*
	 * Some other, unexpected event arrived.
	 */
	else {
		std::cout << "discarding unexpected event" << std::endl;
		LogInfo("discarding unexpected event " << *evt);
		return STATE_PENDING; // no change
	}
}


/*
 * state: STATE_METERING
 */
ni_session::state_t ni_session::handle_state_metering(
		dispatcher *d, event *evt) {

	using namespace mnslp::msg;
	
	LogDebug("Initiating state metering");
	
	/*
	 * A refresh timer was triggered.
	 */
	if ( is_timer(evt, refresh_timer) ) {

		LogDebug("received refresh timer");

		// Build a new REFRESH message, it stores a copy for refreshing.
		set_last_refresh_message( build_refresh_message() );

		// dispatcher will delete
		d->send_message( get_last_refresh_message()->copy() );

        // Set the refresh counter to zero
        set_refresh_counter(0);

		response_timer.start(d, get_response_timeout());

		return STATE_METERING; // no change

	}
	/*
	 * A response timout was triggered.
	 */
	else if ( is_timer(evt, response_timer) ) {

		LogDebug("received response timeout timer");

		// Retry. Send the refresh message again and start a new timer.
		if ( get_refresh_counter() < get_max_retries() ) {
			inc_refresh_counter();

			d->send_message( get_last_refresh_message()->copy() );

			response_timer.start(d, get_response_timeout());

			return STATE_METERING; // no change
		}
		// Retry count exceeded, abort.
		else {
			// Uninstall the previous rules.
			if (rule->get_number_rule_keys() > 0)
				d->remove_policy_rules(rule);
			
			d->report_async_event("got no response to our REFRESH message");
			return STATE_CLOSE;
		}
	}
	/*
	 * API teardown event received. The user wants to end the session.
	 */
	else if ( is_api_teardown(evt) ) {
		LogDebug("received API teardown event");

		// Send a CREATE message with a session lifetime of 0.
		set_lifetime(0);
		set_last_refresh_message(NULL);

		// Uninstall the previous rules.
		std::cout << "Number of rules installed:" 
				  << rule->get_number_rule_keys() << std::endl;
		if (rule->get_number_rule_keys() > 0) 
			d->remove_policy_rules(rule);

		d->send_message( build_refresh_message() );

		return STATE_CLOSE;
	}
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) {
		return STATE_METERING; // no change
	}
	/*
	 * A Mnslp_response message arrived in response to our Refresh message.
	 */
	else if ( is_mnslp_response(evt, get_last_refresh_message() ) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		mnslp_response *resp = e->get_response();

		LogDebug("received RESPONSE: " << *resp);

		/*
		 * Discard if this is no RESPONSE to our last REFRESH.
		 */
		mnslp_refresh *c = get_last_refresh_message()->get_mnslp_refresh();
		if ( ! resp->is_response_to(c) ) {
			LogWarn("RESPONSE doesn't match REFRESH, discarding");
			return STATE_METERING; // no change
		}

		if ( resp->is_success() ) {
			d->report_async_event("REFRESH successful");

			response_timer.stop();
			refresh_timer.start(d, get_refresh_interval());

			set_refresh_counter(0);

			return STATE_METERING; // no change
		}
		else {

			// Uninstall the previous rules.
			if (rule->get_number_rule_keys() > 0)
				d->remove_policy_rules(rule);

			d->report_async_event("REFRESH session died");
			return STATE_CLOSE;
		}

	}
	/*
	 * Some other, unexpected event arrived.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_METERING; // no change
	}
}


/**
 * Process an event.
 *
 * This method implements the transition function of the state machine.
 */
void ni_session::process_event(dispatcher *d, event *evt) {
	
	std::cout << "begin process_event(): " << std::endl;
	LogDebug("begin process_event(): " << *this);
		
	switch ( get_state() ) {

		case ni_session::STATE_CLOSE:
			state = handle_state_close(d, evt);
			break;

		case ni_session::STATE_PENDING:
			state = handle_state_pending(d, evt);
			break;

		case ni_session::STATE_METERING:
			state = handle_state_metering(d, evt);
			break;

		default:
			assert( false ); // invalid state
	}

	LogDebug("end process_event(): " << *this);
}

// EOF
