/// ----------------------------------------*- mode: C++; -*--
/// @file dispatcher.cpp
/// The dispatcher receives events and calls handlers.
/// ----------------------------------------------------------
/// $Id: dispatcher.cpp 2558 2014-11-10 11:22:00 amarentes $
/// $HeadURL: https://./src/dispatcher.cpp $
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
#include "dispatcher.h"
#include "events.h"
#include "benchmark_journal.h"
#include <iostream>


using namespace mnslp;
using namespace mnslp::msg;
using namespace protlib::log;


#define LogError(msg) ERRLog("dispatcher", msg)
#define LogWarn(msg) WLog("dispatcher", msg)
#define LogInfo(msg) ILog("dispatcher", msg)
#define LogDebug(msg) DLog("dispatcher", msg)

#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, "dispatcher", \
	msg << " at " << __FILE__ << ":" << __LINE__)


#ifdef BENCHMARK
  extern benchmark_journal journal;
#endif


/**
 * Constructor.
 *
 * Creates an empty dispatcher. Note that the session_manager, 
 * policy rule installer, and the mnslp_config objects may be 
 * shared between many dispatchers. Thus the destructor will 
 * not delete them.
 *
 * @param m the session manager to use for all session lookups
 * @param p the policy rule installer for interfacing with the operating system
 * @param conf a configuration for this node
 */
dispatcher::dispatcher(session_manager *m, policy_rule_installer *p, 
					   mnslp_config *conf)
		: session_mgr(m), rule_installer(p), config(conf) {

	// nothing to do
}


/**
 * Destructor.
 */
dispatcher::~dispatcher() {
	// nothing to do
}


/**
 * Process an incoming event.
 *
 * Depending on the event, sessions may be created, modified, or deleted.
 * Sometimes events will be discarded, too.
 */
void dispatcher::process(event *evt) throw () {
	assert( evt != NULL );

	LogDebug("processing received event " << *evt);

	// log all incoming MNSLP messages for debugging
	const msg_event *e = dynamic_cast<const msg_event *>(evt);
	if ( e != NULL && e->get_ntlp_msg() != NULL ) {
		assert( e->get_session_id() != NULL );
		LogDebug("received message for session "
				<< *(e->get_session_id()) << " "
				<< *(e->get_ntlp_msg()));
	}

	/*
	 * For a routing_state_check_event, we currently just send an answer.
	 * We could also hand it off to the state machines, but this
	 * implementation accepts GIST queries from anywhere.
	 */
	if ( is_routing_state_check(evt) ) {
		routing_state_check_event *rsc =
			dynamic_cast<routing_state_check_event *>(evt);

		LogInfo("Accepting QUERY");

		send_receive_answer(rsc);
		return;
	}
	
	/*
	 * We received a MNSLP message that we couldn't parse. In this
	 * case, we don't hand it down to the session state machines.
	 * Instead, an error response message is sent back immediately.
	 */
	else if ( is_invalid_mnslp_msg(evt) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();

		LogInfo("sending response for invalid MNSLP message");

		// TODO: specify response code!
		msg::ntlp_msg *resp = msg->create_response(
			information_code::sc_protocol_error, 0);

		send_message( resp );
		return;
	}


	/*
	 * TODO: At this point, we could do some basic error checking on the
	 *       message to avoid creating state for obviously dubious messages.
	 */
	MP(benchmark_journal::PRE_SESSION_MANAGER);

	session *s = NULL;
	session_id *id = evt->get_session_id();

	// If the event has a session ID, try to lookup the session.
	if ( id != NULL )
		s = session_mgr->get_session(*id);

	/*
	 * There can be several reasons if we don't find the session.
	 * In some cases (tg_CREATE, rx_CREATE, etc.), we create a new
	 * session.
	 */
	if ( s == NULL )
		s = create_session(evt);

	MP(benchmark_journal::POST_SESSION_MANAGER);

	/*
	 * If we have a session now, process the event. Otherwise simply
	 * discard it. Top candidates for discarding are obsolete timers.
	 */
	if ( s != NULL ) {
		try {
			MP(benchmark_journal::PRE_SESSION);
			s->process(this, evt);
			MP(benchmark_journal::POST_SESSION);
		}
		catch ( ... ) {
			LogError("process() threw exception, aborting session");
			session_mgr->remove_session(s->get_id());
			return;
		}

		/*
		 * If a session is in state FINAL after processing, delete it.
		 */
		if ( s->is_final() ) {
			session_mgr->remove_session(s->get_id());
			delete s;
		}
	}
	else {
		// Don't log obsolete timers, there are lots of them.
		if ( ! is_timer(evt) )
			LogWarn("discarding event " << *evt);
	}
}


/**
 * Analyzes the given event and creates a session, if appropriate.
 *
 * This method may only be called if the session manager doesn't already have
 * a session for the event.
 */
session *dispatcher::create_session(event *evt) const throw () {
	session *s = NULL;
	session_id *id = evt->get_session_id();

	if ( is_api_configure(evt) ) {
		s = session_mgr->create_ni_session();
	}
	else if ( is_mnslp_configure(evt) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);

		// If addressed to us and not in proxy start an NR session.
		if ( e->is_for_this_node() )
			s = session_mgr->create_nr_session(*id);
		else
			s = session_mgr->create_nf_session(*id);
	}

	return s;
}


/**
 * Send a MNSLP message.
 *
 * This method will delete the msg object after it is done with it.
 *
 * @param msg the message to send
 */
void dispatcher::send_message(msg::ntlp_msg *msg) throw () {
	LogDebug("sending message for session "
			<< msg->get_session_id() << " " << *msg);

	ntlp::APIMsg *apimsg = mapper.create_api_msg(msg);

	bool success = apimsg->send_to(mnslp_config::OUTPUT_QUEUE_ADDRESS);
	assert( success );

	LogDebug("message sent");

	delete msg;
}


void dispatcher::send_receive_answer(
		const routing_state_check_event *evt) const {

	// convert session ID
	session_id *sid = evt->get_session_id();
	uint128 sid_raw = sid->get_id();
	ntlp::sessionid *ntlp_sid = mapper.create_ntlp_session_id(*sid);

	ntlp::APIMsg *reply = new ntlp::APIMsg();
	reply->set_source(mnslp_config::INPUT_QUEUE_ADDRESS);

	reply->set_recvmessageanswer(mnslp_config::NSLP_ID, ntlp_sid,
		evt->get_mri()->copy(), NULL,
		ntlp::APIMsg::directive_establish);

	bool success = reply->send_to(mnslp_config::OUTPUT_QUEUE_ADDRESS);
	assert( success );
}


/**
 * Starts a relative timer.
 *
 * This starts a timer that is triggered at the given time in the future.
 * When the timer goes off, a timer_event object will appear in the
 * dispatcher's input queue.
 *
 * @param s the session this timer is for
 * @param seconds the number of seconds from now
 * @return a timer ID which can be used to recognize this timer
 */
id_t dispatcher::start_timer(const session *s, int seconds) throw () {

	// Timer message, false means not to send errors back to us.
	mnslp_timer_msg *msg = new mnslp_timer_msg(
		s->get_id(), mnslp_config::INPUT_QUEUE_ADDRESS, false);

	id_t ret = msg->get_id(); // save it now to avoid a race condition

	msg->start_relative(seconds);
	msg->send_to(mnslp_config::TIMER_MODULE_QUEUE_ADDRESS);

	LogDebug("started timer " << ret << " for session " << s->get_id());

	return ret;
}


/**
 * Report an asynchronous event to the user.
 *
 * TODO: Interface and implementation details are not decided yet.
 */
void dispatcher::report_async_event(std::string msg) throw () {
	LogInfo("reporting async event to user: " << msg);
	LogUnimp("report_async_event() not implemented");
}


/**
 * Install the given policy rules.
 */
mt_policy_rule * 
dispatcher::install_policy_rules(const mt_policy_rule *mt_rule)
		throw (policy_rule_installer_error) {

	assert( rule_installer != NULL );

	if ( mt_rule != NULL )
		LogDebug("installing MNSLP policy rule " << *mt_rule);

	mt_policy_rule * result = rule_installer->install(mt_rule);
	return result;
}


/**
 * Remove the given policy rules.
 */
void dispatcher::remove_policy_rules(const mt_policy_rule *mt_rule)
		throw (policy_rule_installer_error) {

	assert( rule_installer != NULL );

	if ( mt_rule != NULL )
		LogDebug("removing MNSLP policy rule " << *mt_rule);

	mt_policy_rule * result = rule_installer->remove(mt_rule);
	
	if (result->get_number_rule_keys() != 
			mt_rule->get_number_rule_keys())
			throw policy_rule_installer_error("Invalid ipfix message",
				msg::information_code::sc_signaling_session_failures,
				msg::information_code::sigfail_wrong_conf_message); 
				
}

bool dispatcher::check(const msg::mnslp_mspec_object *object) {	
		
	assert( rule_installer != NULL );
	
	if ( object != NULL )
		LogDebug("Checking mspec object " << *object);
	
	try {
		rule_installer->check(object);
		return true;
	}
	catch (policy_rule_installer_error &e){
		return false;
	}

}

bool dispatcher::is_authorized(const msg_event *evt) const throw () {
	LogUnimp("implement dispatcher::is_authorized()!");
	return true;
}


// EOF
