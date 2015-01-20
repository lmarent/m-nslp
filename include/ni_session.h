/// ----------------------------------------*- mode: C++; -*--
/// @file ni_session.h
/// ni_session class.
/// ----------------------------------------------------------
/// $Id: ni_session.h 2558 2014-11-08 09:54:00 amarentes $
/// $HeadURL: https://./include/ni_session.h $
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
#ifndef MNSLP__NI_SESSION_H
#define MNSLP__NI_SESSION_H

#include "protlib_types.h"

#include "mnslp_config.h"
#include "session.h"
#include "events.h"
#include "mnslp_timers.h"
#include "msg/ntlp_msg.h"


namespace mnslp {
    using protlib::uint32;
    using protlib::hostaddress;

class event;
class api_configure_event;

/**
 * A session for an initiator.
 *
 * An object of this class holds all the data for a MNSLP signaling session
 * and also implements the NI state machine.
 *
 * There are two different attributes for proxy operation: proxy_mode and
 * proxy_session. If proxy_mode is set, then all outgoing CREATE messages
 * will contain a nonce and the P-flag is set. If proxy_session is set, then
 * this session has been created by another session and acts as a proxy for
 * the initiator which is non MNSLP-NSLP aware.
 *
 * Note that the getters and setters are declared protected to make them
 * accessible from a subclass. This is required for the test suite.
 */
class ni_session : public session {
	
  public:
  
	ni_session(const session_id &id, const mnslp_config *conf);
	
	~ni_session();

	bool is_final() const; // inherited from session

  protected:
	/**
	 * States of a session.
	 */
	enum state_t {
		STATE_CLOSE	= 0,
		STATE_PENDING_FORW	= 1,
		STATE_PENDING_PART	= 2,
		STATE_METERING_FORW	= 3,
		STATE_METERING_PART	= 4
	};

	ni_session(state_t s=STATE_CLOSE);

	void process_event(dispatcher *d, event *evt);
	
	inline state_t get_state() const { return state; }

	void set_last_configure_message(msg::ntlp_msg *msg);
	msg::ntlp_msg *get_last_configure_message() const;
	
	void set_last_refresh_message(msg::ntlp_msg *msg);
	msg::ntlp_msg *get_last_refresh_message() const;

	inline void set_configure_counter(uint32 num) { configure_counter = num; }
	inline uint32 get_configure_counter() const { return configure_counter; }

	inline void set_refresh_counter(uint32 num) { refresh_counter = num; }
	inline uint32 get_refresh_counter() const { return refresh_counter; }

	inline uint32 get_refresh_interval() const { return refresh_interval; }
	inline void set_refresh_interval(uint32 sec) { refresh_interval = sec; }
	inline void cal_refresh_interval() { refresh_interval = (uint32) lifetime * 2/3; }

	inline uint32 get_lifetime() const { return lifetime; }
	inline void set_lifetime(uint32 seconds) { lifetime = seconds; }

	inline ntlp::mri *get_mri() const { return routing_info; }
	void set_mri(ntlp::mri *m);

	inline bool is_proxy_mode() const { return proxy_mode; }
	
	inline void set_proxy_mode(bool value) { proxy_mode = value; }

	inline bool is_proxy_session() const { return proxy_session; }

	inline uint32 get_response_timeout() const { return response_timeout; }
	inline void set_response_timeout(uint32 t) { response_timeout = t; }

	inline uint32 get_max_retries() const { return max_retries; }
	inline void set_max_retries(uint32 m) { max_retries = m; }

	inline timer &get_response_timer() { return response_timer; }
	inline timer &get_refresh_timer() { return refresh_timer; }


  private:
	state_t state;

	ntlp::mri *routing_info;

	/*
	 * The latest CONFIGURE message we sent. We keep it because we need it for
	 * retransmission and to check if a received RESPONSE matches the
	 * CONFIGURE sent earlier.
	 */
	msg::ntlp_msg *last_configure_msg;

	/*
	 * The latest REFRESH message we sent. We keep it because we need it for
	 * retransmission and to check if a received RESPONSE matches the
	 * REFRESH sent earlier.
	 */
	msg::ntlp_msg *last_refresh_msg;
	
	bool proxy_mode;
	uint32 lifetime;
	uint32 refresh_interval;
	uint32 response_timeout;
	uint32 configure_counter; 
	uint32 refresh_counter; 
	uint32 max_retries;

	bool proxy_session;

	/*
	 * For an NI session, we have two types of timers: RESPONSE and REFRESH.
	 * At any time, there may be only one timer active per type.
	 */
	timer response_timer;
	timer refresh_timer;

	/*
	 * State machine methods:
	 */
	state_t handle_state_close(dispatcher *d, event *evt);
	state_t handle_state_pending_forward(dispatcher *d, event *evt);
	state_t handle_state_pending_participating(dispatcher *d, event *evt);
	state_t handle_state_metering_forward(dispatcher *d, event *evt);
	state_t handle_state_metering_participating(dispatcher *d, event *evt);

	/*
	 * Utility methods:
	 */
	void setup_session(api_configure_event *evt);
	msg::ntlp_msg *build_configure_message(api_configure_event *evt);
	msg::ntlp_msg *build_refresh_message(); 
	

	uint32 create_random_number() const;
	void inc_configure_counter();
	void inc_refresh_counter();

	friend std::ostream &operator<<(std::ostream &out, const ni_session &s);
	friend class session_manager;
};

std::ostream &operator<<(std::ostream &out, const ni_session &s);

inline bool ni_session::is_final() const 
{
	return get_state() == STATE_CLOSE;
}

inline void ni_session::inc_configure_counter() 
{
	configure_counter++;
}

inline void ni_session::inc_refresh_counter() 
{
	refresh_counter++;
}

inline msg::ntlp_msg *ni_session::get_last_configure_message() const 
{
	assert( last_configure_msg != NULL );
	return last_configure_msg;
}

inline void ni_session::set_last_configure_message(msg::ntlp_msg *msg) 
{
	delete last_configure_msg;
	last_configure_msg = msg;
}

inline msg::ntlp_msg *ni_session::get_last_refresh_message() const 
{
	assert( last_refresh_msg != NULL );
	return last_refresh_msg;
}

inline void ni_session::set_last_refresh_message(msg::ntlp_msg *msg) 
{
	delete last_refresh_msg;
	last_refresh_msg = msg;
}


inline void ni_session::set_mri(ntlp::mri *m) 
{
	delete routing_info;
	routing_info = m;
}


} // namespace mnlsp

#endif // MNSLP__NI_SESSION_H
