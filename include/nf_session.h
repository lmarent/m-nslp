/// ----------------------------------------*- mode: C++; -*--
/// @file nf_session.h
/// The nf_session class.
/// ----------------------------------------------------------
/// $Id: nf_session.h 2558 2014-11-08 13:37:00 amarentes $
/// $HeadURL: https://./include/nf_session.h $
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
#ifndef MNSLP__NF_SESSION_H
#define MNSLP__NF_SESSION_H

#include "protlib_types.h"

#include "session.h"
#include "events.h"
#include "policy_rule.h"
#include "msg/mnslp_msg.h"


namespace mnslp {
    using protlib::uint32;
    using protlib::hostaddress;

class event;
class msg_event;

/**
 * A session for a forwarder.
 *
 * This does *not* include the EXT part.
 */
class nf_session : public session {
	
  public:
  
	nf_session(const session_id &id, const mnslp_config *conf);
	
	~nf_session();

	bool is_final() const; // inherited from session

  protected:
	/**
	 * States of a session.
	 */
	enum state_t {
		STATE_CLOSE		= 0,
		STATE_PENDING	= 1,
		STATE_METERING	= 2
	};

	nf_session(state_t s, const mnslp_config *conf);

	void process_event(dispatcher *d, event *evt);
	
	state_t get_state() const { return state; }

	uint32 get_lifetime() const { return lifetime; }
	
	void set_lifetime(uint32 seconds) { lifetime = seconds; }

	inline timer &get_state_timer() { return state_timer; }
	
	inline timer &get_response_timer() { return response_timer; }

	inline uint32 get_max_lifetime() const { return max_lifetime; }
	
	inline void set_max_lifetime(uint32 t) { max_lifetime = t; }

	inline uint32 get_response_timeout() const { return response_timeout; }
	
	inline void set_response_timeout(uint32 t) { response_timeout = t; }

	msg::ntlp_msg *get_last_configure_message() const;
	
	void set_last_configure_message(msg::ntlp_msg *msg);

	msg::ntlp_msg *get_last_refresh_message() const;
	
	void set_last_refresh_message(msg::ntlp_msg *msg);


	inline bool is_proxy_mode() const { return proxy_mode; }
	
	inline void set_proxy_mode(bool value) { proxy_mode = value; }
	
	ntlp::mri_pathcoupled *get_ni_mri() const;
	void set_ni_mri(ntlp::mri_pathcoupled *m);

	ntlp::mri_pathcoupled *get_nr_mri() const;
	void set_nr_mri(ntlp::mri_pathcoupled *m);


  private:
  
	state_t state;

	const mnslp_config *config;	// shared among sessions, don't delete!

	bool proxy_mode;
	
	uint32 lifetime;
	
	uint32 max_lifetime;
	
	uint32 response_timeout;
	
	timer state_timer;
	
	timer response_timer;

	ntlp::mri_pathcoupled *ni_mri;	// the MRI to use for reaching the NI
	ntlp::mri_pathcoupled *nr_mri;	// the MRI to use for reaching the NR
	msg::ntlp_msg *configure_message;
	msg::ntlp_msg *refresh_message;

	state_t process_state_close(dispatcher *d, event *evt);
	state_t handle_state_close(dispatcher *d, event *evt);
	
	state_t handle_state_pending(dispatcher *d, event *evt);
	
	state_t handle_state_metering(dispatcher *d, event *evt);
	
	ntlp::mri_pathcoupled *create_mri_inverted(
		ntlp::mri_pathcoupled *orig_mri) const;
		
	ntlp::mri_pathcoupled *create_mri_with_dest(
		ntlp::mri_pathcoupled *orig_mri) const;
		
	ntlp_msg *create_msg_for_nr(ntlp_msg *msg) const;
	
	ntlp_msg *create_msg_for_ni(ntlp_msg *msg) const;
	
	msg::ntlp_msg *
	build_configure_message(msg_event *e, 
							std::vector<msg::mnslp_mspec_object *> & missing_objects);
							
	msg::ntlp_msg * build_teardown_message(); 

	void set_pc_mri(msg_event *evt) throw (request_error);
	
	void set_mt_policy_rule(dispatcher *d, 
							 msg_event *evt,
							 std::vector<msg::mnslp_mspec_object *> &missing_objects);

	friend std::ostream &operator<<(std::ostream &out, const nf_session &s);
};

std::ostream &operator<<(std::ostream &out, const nf_session &s);


inline bool nf_session::is_final() const {
	return get_state() == STATE_CLOSE;
}

inline void nf_session::set_last_configure_message(msg::ntlp_msg *msg) {
	delete configure_message;
	configure_message = msg;
}

inline msg::ntlp_msg *nf_session::get_last_configure_message() const {
	assert( configure_message != NULL );
	return configure_message;
}

inline void nf_session::set_last_refresh_message(msg::ntlp_msg *msg) {
	delete refresh_message;
	refresh_message = msg;
}

inline msg::ntlp_msg *nf_session::get_last_refresh_message() const {
	assert( refresh_message != NULL );
	return refresh_message;
}

inline ntlp::mri_pathcoupled *nf_session::get_ni_mri() const {
	assert( ni_mri != NULL );
	return ni_mri;
}

inline void nf_session::set_ni_mri(ntlp::mri_pathcoupled *mri) {
	delete ni_mri;
	ni_mri = mri;
}

inline ntlp::mri_pathcoupled *nf_session::get_nr_mri() const {
	assert( nr_mri != NULL );
	return nr_mri;
}

inline void nf_session::set_nr_mri(ntlp::mri_pathcoupled *mri) {
	delete nr_mri;
	nr_mri = mri;
}


} // namespace mnslp

#endif // MNSLP__NF_SESSION_H
