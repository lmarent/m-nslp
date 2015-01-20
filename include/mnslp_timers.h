/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_timers.h
/// Timer-related classes.
/// ----------------------------------------------------------
/// $Id: mnslp_timers.h 2558 2014-11-08 08:58:00 amarentes $
/// $HeadURL: https://./include/mnslp_timers.h $
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
#ifndef MNSLP__MNSLP_TIMERS_H
#define MNSLP__MNSLP_TIMERS_H

#include "protlib_types.h"
#include "timer_module.h"
#include "messages.h"

#include "session_id.h"


namespace mnslp {
  using namespace protlib;


/**
 * This class extends the TimerMsg from protlib with a session ID.
 */
class mnslp_timer_msg : public TimerMsg {
  public:
	mnslp_timer_msg(session_id id,
					qaddr_t source=qaddr_unknown, bool send_error=false)
		: TimerMsg(source, send_error), sid(id) { }

	session_id get_session_id() const { return sid; }

  private:
	session_id sid;
};


class session;
class dispatcher;

/**
 * A wrapper around the dispatcher's start_timer function.
 */
class timer {
  public:
	timer(session *s);
	timer(session *s, id_t initial_id);

	inline id_t get_id() const { return id; }
	void start(dispatcher *d, int seconds);
	void restart(dispatcher *d, int seconds);
	void stop();

	// needed for the test suite
	inline void set_id(id_t new_id) { id = new_id; }
	
  private:
	id_t id;
	session *owning_session;
};


} // namespace mnslp

#endif // MNSLP__MNSLP_TIMERS_H
