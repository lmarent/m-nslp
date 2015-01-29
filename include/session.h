/// ----------------------------------------*- mode: C++; -*--
/// @file session.h
/// The session classes.
/// ----------------------------------------------------------
/// $Id: session.h 2558 2014-11-06 08:21:00Z  $
/// $HeadURL: https://./include/session.h $
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
#ifndef MNSLP__SESSION_H
#define MNSLP__SESSION_H

#include "protlib_types.h"
#include "session_id.h"
#include "address.h"
#include "policy_rule.h"
#include <vector>


namespace mnslp {
    using protlib::uint8;
    using protlib::uint32;
    using protlib::hostaddress;

class dispatcher;
class event;
class msg_event;


/**
 * An exception to be thrown if a request from a peer cannot be fulfilled.
 *
 * The reason for this is given can be retrieved using get_msg(), and if
 * a response message has to be sent, the provided MNSLP compatible
 * severity and response code values can be used.
 */
class request_error : public std::exception {

  public:
	request_error(const std::string &msg,
		uint8 severity=0, uint8 response_code=0) throw ()
		: msg(msg), severity(severity), response_code(response_code) { }

	virtual ~request_error() throw () { }

	std::string get_msg() const throw () { return msg; }

	uint8 get_severity() const throw () { return severity; }

	uint8 get_response_code() const throw () { return response_code; }

  private:
	std::string msg;
	uint8 severity;
	uint8 response_code;
};


/**
 * An exception to be thrown if a cobfigure request has a greater 
 * lifetime that is allowed by the entity.
 *
 */
class override_lifetime : public std::exception {

  public:
	override_lifetime(const std::string &msg,
		uint8 severity=0, uint8 response_code=0) throw ()
		: msg(msg), severity(severity), response_code(response_code) { }

	virtual ~override_lifetime() throw () { }

	std::string get_msg() const throw () { return msg; }

	uint8 get_severity() const throw () { return severity; }

	uint8 get_response_code() const throw () { return response_code; }
	
	virtual  const char* what() const throw() { return msg.c_str(); } 

  private:
	std::string msg;
	uint8 severity;
	uint8 response_code;
};


inline std::ostream &operator<<(std::ostream &os, const request_error &err) {
	return os << err.get_msg();
}

/**
 * The abstract session class.
 *
 * Only three subclasses will exist, one for initiator, forwarder, and
 * receiver, respectively.
 */
class session {

  public:
	enum session_type_t {
		st_initiator	= 0,
		st_forwarder	= 1,
		st_receiver	= 2
	};

	virtual ~session();

	virtual void process(dispatcher *d, event *evt);

	inline session_id get_id() const { return id; }
	
	inline session_type_t get_session_type() const { return type; }
	
	inline void set_session_type(session_type_t _type) { type = _type; }

	uint32 get_msg_sequence_number() const { return msn; }
	
	void set_msg_sequence_number(uint32 value) { msn = value; }
	
	uint32 next_msg_sequence_number();

	virtual bool is_final() const = 0;

	void set_mspec_object(msg::mnslp_mspec_object *object);
	
	size_t get_number_mspec_objects();
	
  protected:
  
	session();
	
	session(const session_id &sid);

	void check_lifetime(uint32 lifetime, uint32 max_lifetime)
		const throw (override_lifetime);
		
	void check_authorization(dispatcher *d, const msg_event *e)
		const throw (request_error);
		
	bool check_participating(const uint32 _sme);
		
	protlib::appladdress get_nr_address(msg_event *e) const;

	/**
	 * Process the given event and change the session's state if necessary.
	 *
	 * This method has to be implemented by subclasses. The process()
	 * method calls process_event() and locks the session while it
	 * executes.
	 */
	virtual void process_event(dispatcher *d, event *evt) = 0;

  private:
  
	session_id id;
	
	session_type_t type;

	uint32 msn;
	
	mt_policy_rule rule;

	pthread_mutex_t	mutex;

	void init();
};


} // namespace mnslp

#endif // MNSLP__SESSION_H
