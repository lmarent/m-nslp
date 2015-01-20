/// ----------------------------------------*- mode: C++; -*--
/// @file session_id.cpp
/// The session_id class.
/// ----------------------------------------------------------
/// $Id: session_id.cpp 2558 2014-11-05 16:26:00 $
/// $HeadURL: https://./src/session_id.cpp $
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
#include <assert.h>
#include <openssl/rand.h>

#include "logfile.h"

#include "session.h"
// #include "dispatcher.h"


using namespace mnslp;
using namespace protlib::log;


/**
 * Constructor.
 *
 * Initializes this object with a random 128 Bit session ID. The random
 * numbers used are cryptographically strong (according to OpenSSL's docs).
 */
session_id::session_id() {
	// Use OpenSSL's cryptographically strong random numbers.
	int ret = RAND_bytes((unsigned char *) &id, sizeof(id));

	assert( ret == 1 );
}


std::ostream &mnslp::operator<<(std::ostream &out, const session_id &sid) {
	using namespace std;

	uint128 val = sid.get_id();

	ios_base::fmtflags old_fmt = out.flags(); // save stream format

	out << uppercase << hex
		<< setw(8) << setfill('0') << val.w1 << '_'
		<< setw(8) << setfill('0') << val.w2 << '_'
		<< setw(8) << setfill('0') << val.w3 << '_'
		<< setw(8) << setfill('0') << val.w4;

	out.flags(old_fmt); // revert to old stream format

	return out;
}

// EOF
