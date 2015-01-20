/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_timers.cpp
/// Timer-related classes.
/// ----------------------------------------------------------
/// $Id: mnslp_timers.cpp 2558 2014-11-11 10:08:00 amarentes $
/// $HeadURL: https://./src/mnslp_timers.cpp $
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
#include "mnslp_timers.h"
#include "dispatcher.h"
#include <iostream>


using namespace mnslp;


/**
 * Constructor.
 */
timer::timer(session *s)
		: id(0), owning_session(s) {
	// nothing to do
}


void timer::start(dispatcher *d, int seconds) {

	id = d->start_timer(owning_session, seconds);

}

void timer::restart(dispatcher *d, int seconds) {
	id = d->start_timer(owning_session, seconds);
}

void timer::stop() {
	id = 0;
}


// EOF
