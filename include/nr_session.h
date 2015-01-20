/// ----------------------------------------*- mode: C++; -*--
/// @file nr_session.h
/// The nr_session class.
/// ----------------------------------------------------------
/// $Id: nr_session.h 2558 2014-11-08 10:46:00 amarentes $
/// $HeadURL: https://./include/nr_session.h $
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
#ifndef MNSLP__NR_SESSION_H
#define MNSLP__NR_SESSION_H

#include "protlib_types.h"

#include "session.h"
#include "events.h"
#include "policy_rule.h"
#include "msg/ntlp_msg.h"


namespace mnslp {
    using protlib::uint8;
    using protlib::uint32;
    using protlib::hostaddress;

class event;
class msg_event;

/**
 * A session for a responder.
 */
class nr_session : public session {
	
  public:
  
	nr_session(const session_id &id, mnslp_config *conf);
	
	~nr_session();

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

	void process_event(dispatcher *d, event *evt);
	
	state_t get_state() const;

	nr_session(state_t s=STATE_CLOSE, uint32 msn=0);
	
	inline timer &get_state_timer() { return state_timer; }
	
	mt_policy_rule *get_mt_policy_rule() const;

	void set_mt_policy_rule(mt_policy_rule *r);	

  private:
  
	state_t state;

	mnslp_config *config;		// shared among instances, don't delete!

	uint32 lifetime;
	uint32 max_lifetime;
	timer state_timer;
	
	mt_policy_rule *mt_rule;


	/*
	 * State machine methods:
	 */
	state_t handle_state_close(dispatcher *d, event *evt);
	state_t handle_state_metering_forward(dispatcher *d, event *e);	
	state_t handle_state_metering_participating(dispatcher *d, event *e);

	msg::ntlp_msg *build_trace_response(ntlp_msg *msg) const;


	/*
	 * Utility methods:
	 */
	uint32 get_lifetime() const { return lifetime; }
	void set_lifetime(uint32 seconds) { lifetime = seconds; }

	inline uint32 get_max_lifetime() const { return max_lifetime; }
	inline void set_max_lifetime(uint32 t) { max_lifetime = t; }

	friend std::ostream &operator<<(std::ostream &out, const nr_session &s);
	
	/*
	 * Metering methods:
	 */
	bool save_mt_policy_rule(msg_event *evt);
	
	mt_policy_rule *get_mt_policy_rule_copy() const;
	
	
};

std::ostream &operator<<(std::ostream &out, const nr_session &s);


inline nr_session::state_t nr_session::get_state() const 
{
	return state;
}

inline bool nr_session::is_final() const 
{
	return get_state() == STATE_CLOSE;
}


inline void nr_session::set_mt_policy_rule(mt_policy_rule *r) {
	delete mt_rule;
	mt_rule = r;
}

inline mt_policy_rule *nr_session::get_mt_policy_rule() const {
	return mt_rule; // may return NULL!
}


} // namespace mnslp

#endif // MNSLP__NR_SESSION_H
