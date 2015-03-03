/// ----------------------------------------*- mode: C++; -*--
/// @file nf_session.cpp
/// The session for an NSIS Forwarder.
/// ----------------------------------------------------------
/// $Id: nf_session.cpp 3165 2014-11-08 13:44:00 amarentes $
/// $HeadURL: https://./src/nf_session.cpp $
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

#include "mri.h"	// from NTLP

#include "mnslp_config.h"
#include "events.h"
#include "msg/mnslp_msg.h"
#include "dispatcher.h"
#include "session.h"
#include "msg/information_code.h"
#include <iostream>


using namespace mnslp;
using namespace protlib::log;
using protlib::uint32;


#define LogError(msg) ERRLog("nf_session", msg)
#define LogWarn(msg) WLog("nf_session", msg)
#define LogInfo(msg) ILog("nf_session", msg)
#define LogDebug(msg) DLog("nf_session", msg)

#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, "nf_session", \
	msg << " at " << __FILE__ << ":" << __LINE__)


/**
 * Constructor.
 *
 * Use this if the session ID is known in advance.
 */
nf_session::nf_session(const session_id &id, const mnslp_config *conf)
		: session(id), state(nf_session::STATE_CLOSE), config(conf),
		  proxy_mode(false), lifetime(0), max_lifetime(0),
		  response_timeout(0), state_timer(this), response_timer(this),
		  ni_mri(NULL), nr_mri(NULL), configure_message(NULL), 
		  refresh_message(NULL)
{
	set_session_type(st_forwarder);
	assert( config != NULL );
}


/**
 * Constructor for test cases.
 *
 * @param s the state to start in
 */
nf_session::nf_session(nf_session::state_t s, const mnslp_config *conf)
		: session(), state(s), config(conf),
		  proxy_mode(false), lifetime(0), max_lifetime(60),
		  response_timeout(0), state_timer(this), response_timer(this),
		  ni_mri(NULL), nr_mri(NULL), configure_message(NULL),
		  refresh_message(NULL)
{
	set_session_type(st_forwarder);
}


/**
 * Destructor.
 */
nf_session::~nf_session() 
{
	
	if (ni_mri != NULL)
		delete ni_mri;
	
	if (nr_mri != NULL)
		delete nr_mri;
	
	if (configure_message != NULL)
		delete configure_message;
	
	if (refresh_message != NULL)
		delete refresh_message;
		
}


std::ostream &mnslp::operator<<(std::ostream &out, const nf_session &s) {
	static const char *const names[] = { "CLOSE", 
										 "PENDING", 
										 "METERING" };

	return out << "[nf_session: id=" << s.get_id()
		<< ", state=" << names[s.get_state()] << "]";
}


/****************************************************************************
 *
 * Utilities
 *
 ****************************************************************************/

/**
 * Copy the given message and adjust it for forwarding to the NR.
 *
 */
ntlp_msg *nf_session::create_msg_for_nr(ntlp_msg *msg) const 
{
	return msg->copy_for_forwarding();
}


/**
 * Copy the given message and adjust it for forwarding to the NI.
 *
 */
ntlp_msg *nf_session::create_msg_for_ni(ntlp_msg *msg) const 
{
	return msg->copy_for_forwarding();
}


void nf_session::set_pc_mri(msg_event *evt) throw (request_error)
{
	using ntlp::mri_pathcoupled;
	
	LogDebug( "Begin set_pc_mri()");

	assert( evt != NULL );

	mri_pathcoupled *pc_mri
		= dynamic_cast<mri_pathcoupled *>(evt->get_mri());

	if ( pc_mri == NULL ) 
	{
		LogError("no path-coupled MRI found");
		throw request_error("no path-coupled MRI found",
			information_code::sc_permanent_failure,
			information_code::fail_configuration_failed); // failure
	}

	assert( pc_mri->get_downstream() == true );

	// store the rewritten MRIs for later use
	set_nr_mri( create_mri_with_dest(pc_mri) );
	set_ni_mri( create_mri_inverted(pc_mri) );

	LogDebug( "End set_pc_mri()");

}

/**
 * Create a metering policy rule from the given event and save it.
 *
 * If we receive a successful response later, we will activate the rule.
 * In case we don't support the requested policy rule, an exception is thrown.
 */
void nf_session::set_mt_policy_rule(dispatcher *d, 
									 msg_event *evt,
									 std::vector<msg::mnslp_mspec_object *> &missing_objects) 
{
	
	LogDebug( "Begin set_mt_policy_rule()");
	
	std::vector<msg::mnslp_mspec_object *> objects;
	assert( evt != NULL );
	mnslp_configure *configure = evt->get_configure();
	assert( configure != NULL );
	configure->get_mspec_objects(objects);
	
	// Check which metering object could be installed in this node.
	std::vector<msg::mnslp_mspec_object *>::const_iterator it_objects;
	for ( it_objects = objects.begin(); it_objects != objects.end(); it_objects++)
	{
		const mnslp_mspec_object *object = *it_objects;
		if (check_participating(
			   configure->get_selection_metering_entities()))
		{
			if (d->check(object))
				rule->set_object(object->copy());
			else
				missing_objects.push_back(object->copy());
		}
		else
		{
			missing_objects.push_back(object->copy());
		}
	}
	LogDebug("End set_mt_policy_rule() ");
}


// For implementation of metering interface.


/****************************************************************************
 *
 * The state machine
 *
 ****************************************************************************/



msg::ntlp_msg *
nf_session::build_configure_message(msg_event *e, 
								std::vector<msg::mnslp_mspec_object *> & missing_objects) 
{
	using namespace mnslp::msg;
	
	LogDebug( "Begin build_configure_message");
		
	assert( get_nr_mri() != NULL );
	
	mnslp_configure *c = e->get_configure();

	/*
	 * Build a MNSLP Configure message.
	 */
	mnslp_configure *configure = new mnslp_configure();
	
	if (get_lifetime() <= c->get_session_lifetime())
		configure->set_session_lifetime(get_lifetime());
	else
		configure->set_session_lifetime(c->get_session_lifetime());
		
	configure->set_msg_sequence_number(c->get_msg_sequence_number());
	configure->set_selection_metering_entities(c->get_selection_metering_entities());
	configure->set_message_hop_count(c->get_message_hop_count());

	/*
	 * Insert missing objects to install.
	 */
	std::vector<msg::mnslp_mspec_object *>::iterator it_objects;
	for ( it_objects = missing_objects.begin(); 
			it_objects != missing_objects.end(); it_objects++ )
	{
		LogDebug( "Adding object to message");
		configure->set_mspec_object((*it_objects)->copy());
	}

	/*
	 * Wrap the Configure inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), configure, get_nr_mri()->copy(), 0);

	LogDebug( "End build_configure_message");
	
	return msg;
}


/**
 * Build a MNSLP Refresh message to teardown other configurations
 * This could happend when the something went wrong installing the policy rules
 *
 * This will fetch the MSN and also increment the MSN.
 */
msg::ntlp_msg *nf_session::build_teardown_message() 
{
	using namespace mnslp::msg;
	
	LogDebug( "Begin build_teardown_message");

	assert( get_nr_mri() != NULL );

	/*
	 * Build a MNSLP Configure message.
	 */
	mnslp_refresh *refresh = new mnslp_refresh();
	refresh->set_session_lifetime(0);
	refresh->set_msg_sequence_number(next_msg_sequence_number());
	
	/*
	 * Wrap the Configure inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), refresh, get_nr_mri()->copy(), 0);
	
	LogDebug( "Ending build_teardown_message");
	return msg;
}

nf_session::state_t
nf_session::process_state_close(dispatcher *d, event *evt)
{
	using ntlp::mri_pathcoupled;
	
	LogDebug( "Begin process State Close");
	
	msg_event *e = dynamic_cast<msg_event *>(evt);
	ntlp_msg *msg = e->get_ntlp_msg();

	// store one copy for further reference and pass one on
	mnslp_configure *c = e->get_configure();
	set_last_configure_message( msg->copy() );
	try 
	{
		std::vector<msg::mnslp_mspec_object *> missing_objects;
				
		set_pc_mri(e);
		
		set_mt_policy_rule(d, e, missing_objects);

		mri_pathcoupled *mri_pc
						= dynamic_cast<mri_pathcoupled *>(msg->get_mri());
		assert( mri_pc != NULL );

		hostaddress ni_address = e->get_mri()->get_sourceaddress();

		// Create the new message to send foreward.
		d->send_message( build_configure_message(e, missing_objects) );

		state_timer.start(d, get_lifetime());

		// Release the memory allocated to the missing objects vector
		std::vector<msg::mnslp_mspec_object *>::iterator it_objects;
		for ( it_objects = missing_objects.begin(); 
				it_objects != missing_objects.end(); it_objects++)
		{
			delete(*it_objects);
		}
		missing_objects.clear();
		
		LogDebug( "Ending process State Close New state PENDING");		
		return STATE_PENDING;
	}	
	catch ( policy_rule_installer_error &exc ) {
		LogError("policy rule not supported: " << exc);
		d->send_message( msg->create_error_response(exc) );
		return STATE_CLOSE;
	}
	catch ( request_error &exc ) {
		LogError("Mri information not found : " << exc);
		d->send_message( msg->create_error_response(exc) );
		return STATE_CLOSE;	
	}
	
}


nf_session::state_t 
nf_session::handle_state_close(dispatcher *d, event *evt) 
{

	using namespace msg;
	using ntlp::mri_pathcoupled;
	
	LogDebug("begin handle_state_close(): " << *this);

	/*
	 * A msg_event arrived which contains a MNSLP Configure message.
	 */
	if ( is_mnslp_configure(evt) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();

		// store one copy for further reference and pass one on
		mnslp_configure *c = e->get_configure();
		uint32 lifetime = c->get_session_lifetime();
		uint32 msn = c->get_msg_sequence_number();

		set_max_lifetime(config->get_nf_max_session_lifetime());
		set_response_timeout(config->get_nf_max_session_lifetime());

		/*
		 * Before proceeding check several preconditions.
		 */
		try {
			check_lifetime(lifetime, get_max_lifetime());
			check_authorization(d, e);
		}
		catch ( override_lifetime &e) {
			lifetime = get_max_lifetime();
		}
		catch ( request_error &e ) {
			LogError(e);
			d->send_message( msg->create_error_response(e) );
			return STATE_CLOSE;
		}
		if ( lifetime == 0 ) {
			LogWarn("lifetime == 0, discarding message");
			return STATE_CLOSE;
		}
		/*
		 * All basic preconditions match, the CONFIGURE seems to be valid.
		 * Now setup the session.
		 */
		LogDebug("forwarder session initiated, waiting for a response");

		set_lifetime(lifetime);
		set_msg_sequence_number(msn);
		
		return process_state_close(d, evt);

	}
	
}

/*
 * state: STATE_PENDING
 */
nf_session::state_t 
nf_session::handle_state_pending(dispatcher *d, event *evt) {

	using namespace mnslp::msg;

	LogDebug("begin handle_state_pending(): " << *this);
	
	/*
	 * Another CONFIGURE from the upstream peer arrived.
	 *
	 * Either the NI wants to shut down the session even before it started,
	 * or we didn't respond fast enough and the NI resends its CONFIGURE.
	 *
	 * TODO: What if this is a new CONFIGURE with lifetime > 0? Discard?
	 * Accept and even save policy rules?
	 */
	if ( is_mnslp_configure(evt) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		mnslp_configure *configure = e->get_configure();

		mnslp_configure *previous = dynamic_cast<mnslp_configure *>(
			get_last_configure_message()->get_mnslp_configure());
		assert( previous != NULL );

		try {
			check_authorization(d, e);
		}
		catch ( request_error &e ) {
			LogError(e);
			d->send_message( msg->create_error_response(e) );
			return STATE_PENDING; // no change
		}

		if ( configure->get_msg_sequence_number() >
				previous->get_msg_sequence_number()
				&& configure->get_session_lifetime() == 0 ) {

			LogDebug("terminating session.");

			state_timer.stop();

			d->send_message( msg->copy_for_forwarding() );

			return STATE_CLOSE;
		}
		else if ( configure->get_msg_sequence_number()
				== previous->get_msg_sequence_number() ) {

			LogWarn("NI resent the initial CONFIGURE. Passing it on.");

			d->send_message( msg->copy_for_forwarding() );

			state_timer.start(d, get_response_timeout());

			return STATE_PENDING; // no change
		}
		else {
			LogWarn("Replacing CONFIGURE message."); 
									
			set_lifetime(configure->get_session_lifetime());
			set_msg_sequence_number(configure->get_msg_sequence_number());

			return process_state_close(d, e);
		}
	}
	
	/*
	 * Either GIST can't reach the destination or we waited long enough
	 * for the downstream peer to respond. Anyway, send a RESPONSE.
	 */
	else if ( is_no_next_node_found_event(evt)
			|| is_timer(evt, state_timer) ) {

		LogInfo("cannot reach destination");

		ntlp_msg *latest = get_last_configure_message();
		ntlp_msg *resp = latest->create_response(
			information_code::sc_permanent_failure,
			information_code::fail_nr_not_reached);

		d->send_message( resp );

		// TODO: ReportAsyncEvent() ?

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
		ntlp_msg *msg = e->get_ntlp_msg();
		mnslp_response *resp = e->get_response();

		// Discard if this is no RESPONSE to our original Configure.
		mnslp_configure *c = get_last_configure_message()->get_mnslp_configure();
		if ( ! resp->is_response_to(c) ) {
			LogWarn("RESPONSE doesn't match CONFIGURE, discarding");
			return STATE_PENDING; // no change
		}

		if ( resp->is_success() ) {
			LogDebug("initiated session " << get_id());

			mt_policy_rule * result = d->install_policy_rules(rule);
						// Verify that every rule that passed the checking process could be installed.
			if (result->get_number_mspec_objects() == rule->get_number_mspec_objects() )
			{
				// free the space allocated to the rule to be installed.
				delete(rule);
			
				// Assign the response as the rule installed.
				rule = result;
				d->send_message( create_msg_for_ni(msg) );
				state_timer.start(d, get_lifetime());
				return STATE_METERING;
			}
			else
			{
				set_lifetime(0);
				delete(rule);
				// Assign the response as the rule installed.
				rule = result;
				// Uninstall the previous rules.
				if (rule->get_number_rule_keys() > 0)
					d->remove_policy_rules(rule);
				
				// Create message towards nr to teardown what we have done before 
				ntlp_msg * tear_down = build_teardown_message();
				d->send_message( tear_down );

				// Send an error message to the ni.
				d->send_message( msg->create_response(
								 information_code::sc_permanent_failure, 
								 information_code::fail_internal_error));
				return STATE_CLOSE;
			}
		}
		else {
			LogDebug("forwarding error msg from upstream peer");

			// TODO: ReportAsyncEvent() ?
			state_timer.stop();

			d->send_message( create_msg_for_ni(msg) );

			return STATE_CLOSE;
		}
	}
	
	/*
	 * Unexpected event.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_PENDING; // no change
	}

	LogDebug("Ending handle_state_pending(): " << *this);

}


/*
 * state: STATE_METERING
 */
nf_session::state_t nf_session::handle_state_metering(
		dispatcher *d, event *evt) {

	using namespace mnslp::msg;
  
	LogDebug("Begining handle_state_metering(): " << *this);
  
	/*
	 * A msg_event arrived which contains a MNSLP REFRESH message.
	 */
	if ( is_mnslp_refresh(evt) ) 
	{
				
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		mnslp_refresh *refresh = e->get_refresh();

		uint32 lifetime = refresh->get_session_lifetime();
		uint32 msn = refresh->get_msg_sequence_number();

		/*
		 * Before proceeding check several preconditions.
		 */
		try {
			check_lifetime(lifetime, get_max_lifetime());
			check_authorization(d, e);
		}
		catch ( override_lifetime &e) {
			lifetime = get_max_lifetime();
		}
		catch ( request_error &e ) {
			LogError(e);
			d->send_message( msg->create_error_response(e) );
			return STATE_METERING; // no change!
		}

		if ( ! is_greater_than(msn, get_msg_sequence_number()) ) {
			LogWarn("discarding duplicate response.");
			return STATE_METERING; // no change
		}

		/*
		 * All preconditions have been checked.
		 */
		set_lifetime(lifetime); // could be a new lifetime!
		if ( lifetime > 0 ) {
			LogDebug("forwarder session refreshed.");

			response_timer.stop();

			set_msg_sequence_number(msn);

			// store one copy for further reference and pass one on
			set_last_refresh_message( msg->copy() );

			d->send_message( create_msg_for_nr(msg) );

			response_timer.start(d, get_response_timeout());

			return STATE_METERING; // no change
		}
		else {	// lifetime == 0
			LogDebug("forwarder session refreshed lifetime 0.");
			
			response_timer.stop();
			
			d->send_message( create_msg_for_nr(msg) );
						
			// store one copy for further reference and pass one on
			set_last_refresh_message( msg->copy() );

			response_timer.start(d, get_response_timeout());
						
			// Wait confirmation to close session
			return STATE_METERING; 

		}
	}
	
	/*
	 * Downstream peer didn't respond.
	 * 
	 * This is the synchronous case, because the upstream peer is still
	 * waiting for a response to its REFRESH message.
	 *
	 * Note: We can safely ignore this when we're in proxy mode.
	 */
	else if ( ( is_no_next_node_found_event(evt) || 
	            is_timer(evt, response_timer) ) ) {
		LogWarn("downstream peer did not respond");
		
		state_timer.stop();
		
		// Uninstall the previous rules.
		if (rule->get_number_rule_keys() > 0)
			d->remove_policy_rules(rule);

		// TODO: check the spec!
		ntlp_msg *response = get_last_refresh_message()->create_response(
			information_code::sc_permanent_failure,
			information_code::fail_nr_not_reached);

		d->send_message( response );

		return STATE_CLOSE; 
	}
	
	/*
	 * Upstream peer didn't send a refresh in time.
	 */
	else if ( is_timer(evt, state_timer) ) 
	{		
		LogWarn("session timed out");
		response_timer.stop();

		// Uninstall the previous rules.
		if (rule->get_number_rule_keys() > 0)
			d->remove_policy_rules(rule);

		// TODO: ReportAsyncEvent()

		return STATE_CLOSE;
	}
	/*
	 * GIST detected that one of our routes is no longer usable. This
	 * could be the route to the NI or to the NR.
	 */
	else if ( is_route_changed_bad_event(evt) ) {
		LogUnimp("route to the NI or to the NR is no longer usable");

		// Uninstall the previous rules.
		if (rule->get_number_rule_keys() > 0)
			d->remove_policy_rules(rule);

		return STATE_CLOSE;
	}	
	/*
	 * A RESPONSE to a REFRESH arrived.
	 */
	else if ( is_mnslp_response(evt, get_last_refresh_message()) ) 
	{
			
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		mnslp_response *response = e->get_response();
		
		// Discard if this is not a RESPONSE to our original CREATE.
		mnslp_refresh *c = get_last_refresh_message()->get_mnslp_refresh();
		if ( ! response->is_response_to(c) ) 
		{			
			LogWarn("RESPONSE doesn't match REFRESH, discarding");
			return STATE_METERING;	// no change
		}

		
		if ( response->is_success() ) {
			
			LogDebug("upstream peer sent successful response.");
			d->send_message( create_msg_for_ni(msg) );
			if ( get_lifetime() == 0 )
			{
				state_timer.stop();	
				// Uninstall the previous rules.
				if (rule->get_number_rule_keys() > 0)
					d->remove_policy_rules(rule);	
				return STATE_CLOSE;
			}
			else
			{
				state_timer.start(d, get_lifetime());
				return STATE_METERING; // no change
			}
		}
		else {
			LogWarn("error message received.");
			state_timer.stop();
			// Uninstall the previous rules.
			if (rule->get_number_rule_keys() > 0)
				d->remove_policy_rules(rule);
			
			d->send_message( create_msg_for_ni(msg) );

			return STATE_CLOSE;
		}
	}
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) {
		return STATE_METERING; // no change
	}
	/*
	 * Received unexpected event.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_METERING; // no change
	}

	LogDebug("Ending handle_state_metering(): " << *this);
	
}

/**
 * Process an event.
 *
 * This method implements the transition function of the state machine.
 */
void nf_session::process_event(dispatcher *d, event *evt) {

	LogDebug("begin process_event(): " << *this);

	switch ( get_state() ) {

		case nf_session::STATE_CLOSE:
			state = handle_state_close(d, evt);
			break;

		case nf_session::STATE_PENDING:
			state = handle_state_pending(d, evt);
			break;

		case nf_session::STATE_METERING:
			state = handle_state_metering(d, evt);
			break;

		default:
			assert( false ); // invalid state
	}

	LogDebug("end process_event(): " << *this);
}

/**
 * Create a new MRI but with a rewritten destination.
 * TODO AM: check how it works with NAT operation.
 * This is needed for NAT operation. 
 */
ntlp::mri_pathcoupled *nf_session::create_mri_with_dest(
		ntlp::mri_pathcoupled *orig_mri) const {

	LogDebug("begin create_mri_with_dest(): " << *this);

	assert( orig_mri != NULL );
	assert( orig_mri->get_downstream() == true );

	// create a new MRI with rewritten destination
	ntlp::mri_pathcoupled *new_mri = new ntlp::mri_pathcoupled(
		orig_mri->get_sourceaddress(),
		orig_mri->get_sourceprefix(),
		orig_mri->get_sourceport(),
		orig_mri->get_destaddress(),				
		orig_mri->get_destprefix(),
		orig_mri->get_destport(),
		orig_mri->get_protocol(),
		orig_mri->get_flowlabel(),
		orig_mri->get_ds_field(),
		orig_mri->get_spi(),
		orig_mri->get_downstream()
	);

	LogDebug("End create_mri_with_dest(): " << *this);

	return new_mri;
}

/**
 * Create a new MRI but with the direction flag inverted.
 */
ntlp::mri_pathcoupled *nf_session::create_mri_inverted(
		ntlp::mri_pathcoupled *orig_mri) const {

	LogDebug("begin create_mri_inverted(): " << *this);

	assert( orig_mri != NULL );
	assert( orig_mri->get_downstream() == true );

	ntlp::mri_pathcoupled *new_mri = orig_mri->copy();
	new_mri->invertDirection();

	LogDebug("end create_mri_inverted(): " << *this);

	return new_mri;
}

// EOF
