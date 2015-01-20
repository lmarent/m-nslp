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


using namespace mnslp;
using namespace protlib::log;
using protlib::uint32;


#define LogError(msg) ERRLog("mnslp_session", msg)
#define LogWarn(msg) WLog("mnslp_session", msg)
#define LogInfo(msg) ILog("mnslp_session", msg)
#define LogDebug(msg) DLog("mnslp_session", msg)

#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, "mnslp_session", \
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
nf_session::~nf_session() {
	delete ni_mri;
	delete nr_mri;
	delete configure_message;
	delete refresh_message;
}


std::ostream &mnslp::operator<<(std::ostream &out, const nf_session &s) {
	static const char *const names[] = { "CLOSE", 
										 "PENDING_FORWARD", 
										 "PENDING_PARTICIPATING", 
										 "METERING_FORWARD", 
										 "METERING_PARTICIPATING" };

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
ntlp_msg *nf_session::create_msg_for_nr(ntlp_msg *msg) const {

	return msg->copy_for_forwarding();
}


/**
 * Copy the given message and adjust it for forwarding to the NI.
 *
 */
ntlp_msg *nf_session::create_msg_for_ni(ntlp_msg *msg) const {

	return msg->copy_for_forwarding();
}

/**
 * Return a copy of the saved metering policy rule if we have one.
 */
mt_policy_rule *nf_session::get_mt_policy_rule_copy() const {
	if ( get_mt_policy_rule() == NULL )
		return NULL;
	else
		return get_mt_policy_rule()->copy();
}



/**
 * Create a metering policy rule from the given event and save it.
 *
 * If we receive a successful response later, we will activate the rule.
 * In case we don't support the requested policy rule, an exception is thrown.
 */
bool nf_session::save_mt_policy_rule(msg_event *evt) {
	
	using ntlp::mri_pathcoupled;

	assert( evt != NULL );

	mnslp_configure *configure = evt->get_configure();
	assert( configure != NULL );

	mri_pathcoupled *pc_mri
		= dynamic_cast<mri_pathcoupled *>(evt->get_mri());

	if ( pc_mri == NULL ) {
		LogError("no path-coupled MRI found");
		return false; // failure
	}

	assert( pc_mri->get_downstream() == true );

    /* TODO AM - define the policy rule for metering. 
	// TODO: We ignore sub_ports for now.
	if ( configure->get_subsequent_ports() != 0 )
		throw policy_rule_installer_error("unsupported sub_ports value",
			information_code::sc_signaling_session_failures,
			information_code::sigfail_unknown_policy_rule_action);

	fw_policy_rule::action_t action;

	switch ( create->get_rule_action() ) {
	  case extended_flow_info::ra_allow:
		action = fw_policy_rule::ACTION_ALLOW;
		break;
	  case extended_flow_info::ra_deny:
		action = fw_policy_rule::ACTION_DENY;
		break;
	  default:
		throw policy_rule_installer_error("unknown policy rule action",
			information_code::sc_signaling_session_failures,
			information_code::sigfail_unknown_policy_rule_action);
	}

	fw_policy_rule *r = new fw_policy_rule(action,
		pc_mri->get_sourceaddress(), pc_mri->get_sourceprefix(),
		pc_mri->get_sourceport(),
		pc_mri->get_destaddress(), pc_mri->get_destprefix(),
		pc_mri->get_destport(),
		pc_mri->get_protocol()
	);

	set_fw_policy_rule(r);
    */
	LogDebug("saved policy rule for later use: ");

	return true; // success
}


// For implementation of metering interface.


/****************************************************************************
 *
 * The state machine
 *
 ****************************************************************************/


/*
 * state: CLOSE
 */
nf_session::state_t nf_session::handle_state_close(dispatcher *d, event *evt) 
{
	using namespace msg;

	/*
	 * A msg_event arrived which contains a MNSLP Configure message.
	 */
	if ( is_mnslp_configure(evt) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		mnslp_configure *configure = e->get_configure();

		uint32 lifetime = configure->get_session_lifetime();
		uint32 msn = configure->get_msg_sequence_number();

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

		return handle_state_close_configure(d, e);
	}
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_CLOSE; // no change
	}
}


nf_session::state_t nf_session::handle_state_close_configure(
		dispatcher *d, msg_event *e) {

	using namespace msg;
	using ntlp::mri_pathcoupled;

	ntlp_msg *msg = e->get_ntlp_msg();

	// store one copy for further reference and pass one on
	set_last_configure_message( msg->copy() );
	mnslp_configure *c = e->get_configure();
	
	if (check_participating(c->get_selection_metering_entities()))
	{
		LogDebug("this node is a metering node");

		try {
			// TODO AM: implement the policy manager.
			//save_mt_policy_rule(e);

			mri_pathcoupled *mri_pc
				= dynamic_cast<mri_pathcoupled *>(msg->get_mri());
			assert( mri_pc != NULL );

			hostaddress ni_address = e->get_mri()->get_sourceaddress();

			d->send_message( create_msg_for_nr(msg) );

			state_timer.start(d, get_lifetime());
						
			return STATE_PENDING_PART;
			
		}
		catch ( policy_rule_installer_error &e ) {
			LogError("policy rule not supported: " << e);
			d->send_message( msg->create_error_response(e) );

			return STATE_CLOSE;
		}
	}
	else{
		mri_pathcoupled *mri_pc
			= dynamic_cast<mri_pathcoupled *>(msg->get_mri());
		assert( mri_pc != NULL );

		hostaddress ni_address = e->get_mri()->get_sourceaddress();

		d->send_message( create_msg_for_nr(msg) );

		state_timer.start(d, get_lifetime());
		
		return STATE_PENDING_FORW;
	
	}

}


/*
 * state: STATE_PENDING_FORW
 */
nf_session::state_t nf_session::handle_state_pending_forward(
		dispatcher *d, event *evt) {

	using namespace mnslp::msg;

	/*
	 * A msg_event arrived which contains a MNSLP RESPONSE message.
	 */
	if ( is_mnslp_response(evt, get_last_configure_message()) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		mnslp_response *resp = e->get_response();

		// Discard if this is no RESPONSE to our original CREATE.
		mnslp_configure *c = get_last_configure_message()->get_mnslp_configure();
		if ( ! resp->is_response_to(c) ) {
			LogWarn("RESPONSE doesn't match CONFIGURE, discarding");
			return STATE_PENDING_FORW; // no change
		}

		if ( resp->is_success() ) {
						
			LogDebug("initiated session " << get_id());

			// TODO AM: create the policy manager.
			// d->install_policy_rules(get_mt_policy_rule_copy());

			d->send_message( create_msg_for_ni(msg) );

			state_timer.start(d, get_lifetime());

			return STATE_METERING_FORW;
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
	 * Another CONFIGURE from the upstream peer arrived.
	 *
	 * Either the NI wants to shut down the session even before it started,
	 * or we didn't respond fast enough and the NI resends its CONFIGURE.
	 *
	 * TODO: What if this is a new CONFIGURE with lifetime > 0? Discard?
	 * Accept and even save policy rules?
	 */
	else if ( is_mnslp_configure(evt) ) {
		
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
			return STATE_PENDING_FORW; // no change
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

			return STATE_PENDING_FORW; // no change
		}
		else {
			LogWarn("invalid/unexpected CONFIGURE."); // TODO
			
			// TODO AM: create the policy manager.
			// remove_policy();
						
			set_lifetime(configure->get_session_lifetime());
			set_msg_sequence_number(configure->get_msg_sequence_number());

			return handle_state_close_configure(d, e);

		}
	}
	/*
	 * We can't reach the destination because there seems to be no GIST
	 * hop. If the proxy flag was set in the original CONFIGURE, however,
	 * then this node will act as a proxy and create an nr_session.
	 * 	 *
	 * Note: Authentication/Authorization has been checked in state CLOSE,
	 *       when the CONFIGURATION message arrived.
	 */
	else if ( is_no_next_node_found_event(evt) ) {

		LogInfo("no next node found, this node will act as a proxy");

		set_proxy_mode(true);

		// TODO AM: install policy rules.
		// d->install_policy_rules(get_mt_policy_rule_copy());

		/*
		 * Create the response that usually the NR would send.
		 */
		ntlp_msg *msg = get_last_configure_message();

		d->send_message( msg->create_success_response(lifetime) );

		state_timer.stop();

		return STATE_METERING_FORW;
	}
	/*
	 * Either GIST can't reach the destination or we waited long enough
	 * for the downstream peer to respond. Anyway, send a RESPONSE.
	 */
	else if ( is_no_next_node_found_event(evt)
			|| is_timer(evt, state_timer) ) {

		LogInfo("cannot reach destination");

		std::cout << "can not reach destination" << std::endl;

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
		return STATE_PENDING_FORW; // no change
	}
	/*
	 * Unexpected event.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_PENDING_FORW; // no change
	}
}


/*
 * state: STATE_PENDING_PART
 */
nf_session::state_t nf_session::handle_state_pending_participating(
		dispatcher *d, event *evt) {

	using namespace mnslp::msg;

	/*
	 * A msg_event arrived which contains a MNSLP RESPONSE message.
	 */
	if ( is_mnslp_response(evt, get_last_configure_message()) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		mnslp_response *resp = e->get_response();

		// Discard if this is no RESPONSE to our original CREATE.
		mnslp_configure *c = get_last_configure_message()->get_mnslp_configure();
		if ( ! resp->is_response_to(c) ) {
			LogWarn("RESPONSE doesn't match CONFIGURE, discarding");
			return STATE_PENDING_PART; // no change
		}

		if ( resp->is_success() ) {
			LogDebug("initiated session " << get_id());

			// TODO AM: create the policy manager.
			// d->install_policy_rules(get_mt_policy_rule_copy());

			d->send_message( create_msg_for_ni(msg) );

			state_timer.start(d, get_lifetime());

			return STATE_METERING_PART;
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
	 * Another CONFIGURE from the upstream peer arrived.
	 *
	 * Either the NI wants to shut down the session even before it started,
	 * or we didn't respond fast enough and the NI resends its CONFIGURE.
	 *
	 * TODO: What if this is a new CONFIGURE with lifetime > 0? Discard?
	 * Accept and even save policy rules?
	 */
	else if ( is_mnslp_configure(evt) ) {
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
			return STATE_PENDING_PART; // no change
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

			return STATE_PENDING_PART; // no change
		}
		else {
			LogWarn("Replacing CONFIGURE message."); 
			
			// TODO AM: create the policy manager.
			// remove_policy();
						
			set_lifetime(configure->get_session_lifetime());
			set_msg_sequence_number(configure->get_msg_sequence_number());

			return handle_state_close_configure(d, e);
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
		return STATE_PENDING_PART; // no change
	}
	/*
	 * Unexpected event.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_PENDING_PART; // no change
	}
}

/*
 * state: STATE_METERING_FORW
 */
nf_session::state_t nf_session::handle_state_metering_forward(
		dispatcher *d, event *evt) 
{

	using namespace mnslp::msg;
		
	/*
	 * A msg_event arrived which contains a MNSLP REFRESH message.
	 */
	if ( is_mnslp_refresh(evt) ) {
				
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
			return STATE_METERING_FORW; // no change!
		}

		if ( ! is_greater_than(msn, get_msg_sequence_number()) ) {
			LogWarn("discarding duplicate response.");
			return STATE_METERING_FORW; // no change
		}
		
		/*
		 * All preconditions have been checked.
		 */

		if ( lifetime > 0 ) {
			LogDebug("forwarder session refreshed.");

			response_timer.stop();
			set_lifetime(lifetime); // could be a new lifetime!
			set_msg_sequence_number(msn);

			// TODO: If we get a different policy rule -> update
			// See section 3.11.
			//save_fw_policy_rule(e);

			// store one copy for further reference and pass one on
			set_last_refresh_message( msg->copy() );

			d->send_message( create_msg_for_nr(msg) );

			response_timer.start(d, get_response_timeout());

			return STATE_METERING_FORW; // no change

		}
		else 
		{	// lifetime == 0
			LogDebug("terminating session.");

			response_timer.stop();

			// TODO AM: implement policy rules.
			// d->remove_policy_rules(get_mt_policy_rule_copy());

			d->send_message( create_msg_for_nr(msg) );
						
			// store one copy for further reference and pass one on
			set_last_refresh_message( msg->copy() );

			response_timer.start(d, get_response_timeout());
						
			return STATE_METERING_FORW; // Wait confirmation to close session
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
	else if ( ( is_no_next_node_found_event(evt)
			|| is_timer(evt, response_timer) ) ) {

		LogWarn("downstream peer did not respond");

		state_timer.stop();
		response_timer.stop();

		// TODO AM: implement policy rules.
		// d->remove_policy_rules(get_mt_policy_rule_copy());

		// TODO: check the spec!
		ntlp_msg *response = get_last_refresh_message()->create_response(
			information_code::sc_permanent_failure,
			information_code::fail_nr_not_reached);

		d->send_message( response );

		return STATE_CLOSE; // TODO: check!
	}
	/*
	 * Upstream peer didn't send a refresh in time.
	 */
	else if ( is_timer(evt, state_timer) ) {
		LogWarn("session timed out");
		response_timer.stop();

		// TODO AM: implement policy rules.
		// d->remove_policy_rules(get_mt_policy_rule_copy());

		// TODO: ReportAsyncEvent()

		return STATE_CLOSE;
	}
	
	/*
	 * A RESPONSE to a REFRESH arrived.
	 */
	else if ( is_mnslp_response(evt, get_last_refresh_message()) ) {
			
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		mnslp_response *response = e->get_response();

		// Discard if this is no RESPONSE to our original CREATE.
		mnslp_refresh *c = get_last_refresh_message()->get_mnslp_refresh();
		if ( ! response->is_response_to(c) ) {
			
			LogWarn("RESPONSE doesn't match REFRESH, discarding");
			return STATE_METERING_FORW;	// no change
		}
		
		if ( response->is_success() ) {
			LogDebug("upstream peer sent successful response.");
			d->send_message( create_msg_for_ni(msg) );

			if ( get_lifetime() == 0 ){
				state_timer.stop();
				// TODO AM: implement policy rules.
				// d->remove_policy_rules(get_mt_policy_rule_copy());
				return STATE_CLOSE;
			}
			else{
				state_timer.start(d, get_lifetime());
				return STATE_METERING_FORW; // no change
			}
		}
		else {
			LogWarn("error message received.");
			// TODO AM: implement policy rules.
			// d->remove_policy_rules(get_mt_policy_rule_copy());

			state_timer.stop();
			d->send_message( create_msg_for_ni(msg) );

			return STATE_CLOSE;
		}
	}
	/*
	 * GIST detected that one of our routes is no longer usable. This
	 * could be the route to the NI or to the NR.
	 */
	else if ( is_route_changed_bad_event(evt) ) {
		LogUnimp("route to the NI or to the NR is no longer usable");
		return STATE_CLOSE;
	}
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) {
		return STATE_METERING_FORW; // no change
	}
	/*
	 * Received unexpected event.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);		
		return STATE_METERING_FORW; // no change
	}
	
}


/*
 * state: STATE_METERING_PART
 */
nf_session::state_t nf_session::handle_state_metering_participating(
		dispatcher *d, event *evt) {

	using namespace mnslp::msg;
  
	/*
	 * A msg_event arrived which contains a MNSLP REFRESH message.
	 */
	if ( is_mnslp_refresh(evt) ) {
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
			return STATE_METERING_PART; // no change!
		}

		if ( ! is_greater_than(msn, get_msg_sequence_number()) ) {
			LogWarn("discarding duplicate response.");
			return STATE_METERING_PART; // no change
		}

		/*
		 * All preconditions have been checked.
		 */

		if ( lifetime > 0 ) {
			LogDebug("forwarder session refreshed.");

			response_timer.stop();
			set_lifetime(lifetime); // could be a new lifetime!
			set_msg_sequence_number(msn);

			// TODO: If we get a different policy rule -> update
			// See section 3.11.
			//save_fw_policy_rule(e);

			// store one copy for further reference and pass one on
			set_last_refresh_message( msg->copy() );

			d->send_message( create_msg_for_nr(msg) );

			response_timer.start(d, get_response_timeout());

			return STATE_METERING_PART; // no change
		}
		else {	// lifetime == 0
			LogDebug("forwarder session refreshed.");
			response_timer.stop();

			// TODO AM: implement policy rules.
			// d->remove_policy_rules(get_mt_policy_rule_copy());

			d->send_message( create_msg_for_nr(msg) );
						
			// store one copy for further reference and pass one on
			set_last_refresh_message( msg->copy() );

			response_timer.start(d, get_response_timeout());
						
			return STATE_METERING_PART; // Wait confirmation to close session

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

		// TODO: check the spec!
		ntlp_msg *response = get_last_refresh_message()->create_response(
			information_code::sc_permanent_failure,
			information_code::fail_nr_not_reached);

		d->send_message( response );

		return STATE_CLOSE; // TODO: check!
	}
	
	/*
	 * Upstream peer didn't send a refresh in time.
	 */
	else if ( is_timer(evt, state_timer) ) {
		LogWarn("session timed out");
		std::cout << "session timed out" << std::endl;
		response_timer.stop();
		// TODO AM: implement policy rules.
		// d->remove_policy_rules(get_mt_policy_rule_copy());

		// TODO: ReportAsyncEvent()

		return STATE_CLOSE;
	}
	
	/*
	 * A RESPONSE to a REFRESH arrived.
	 */
	else if ( is_mnslp_response(evt, get_last_refresh_message()) ) {
		
		std::cout << "is_mnslp response event" << std::endl;
		
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		mnslp_response *response = e->get_response();

		// Discard if this is no RESPONSE to our original CREATE.
		mnslp_refresh *c = get_last_refresh_message()->get_mnslp_refresh();
		if ( ! response->is_response_to(c) ) {
			LogWarn("RESPONSE doesn't match REFRESH, discarding");
			return STATE_METERING_PART;	// no change
		}

		
		if ( response->is_success() ) {
			LogDebug("upstream peer sent successful response.");

			d->send_message( create_msg_for_ni(msg) );
			if ( get_lifetime() == 0 ){
				state_timer.stop();
				// TODO AM: implement policy rules.
				// d->remove_policy_rules(get_mt_policy_rule_copy());
				return STATE_CLOSE;
			}
			else{
				state_timer.start(d, get_lifetime());
				return STATE_METERING_PART; // no change
			}
		}
		else {
			LogWarn("error message received.");
			// TODO AM: implement policy rules.
			// d->remove_policy_rules(get_mt_policy_rule_copy());
			state_timer.stop();
			d->send_message( create_msg_for_ni(msg) );

			return STATE_CLOSE;
		}
	}
	/*
	 * GIST detected that one of our routes is no longer usable. This
	 * could be the route to the NI or to the NR.
	 */
	else if ( is_route_changed_bad_event(evt) ) {
		LogUnimp("route to the NI or to the NR is no longer usable");
		// TODO AM: implement policy rules.
		// d->remove_policy_rules(get_mt_policy_rule_copy());
		return STATE_CLOSE;
	}
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) {
		return STATE_METERING_PART; // no change
	}
	/*
	 * Received unexpected event.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_METERING_PART; // no change
	}
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

		case nf_session::STATE_PENDING_FORW:
			state = handle_state_pending_forward(d, evt);
			break;

		case nf_session::STATE_PENDING_PART:
			state = handle_state_pending_participating(d, evt);
			break;

		case nf_session::STATE_METERING_FORW:
			state = handle_state_metering_forward(d, evt);
			break;

		case nf_session::STATE_METERING_PART:
			state = handle_state_metering_participating(d, evt);
			break;

		default:
			assert( false ); // invalid state
	}

	LogDebug("end process_event(): " << *this);
}

// EOF
