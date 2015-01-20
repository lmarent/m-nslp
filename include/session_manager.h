/// ----------------------------------------*- mode: C++; -*--
/// @file session_manager.h
/// The session manager.
/// ----------------------------------------------------------
/// $Id: session_manager.h 2558 2014-11-08 09:39:00 amarentes $
/// $HeadURL: https://./include/session_manager.h $
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
#ifndef MNSLP__SESSION_MANAGER_H
#define MNSLP__SESSION_MANAGER_H

#include <ext/hash_map>

#include "protlib_types.h"

#include "session.h"
#include "ni_session.h"
#include "nf_session.h"
#include "nr_session.h"


namespace mnslp {

/**
 * The session manager.
 *
 * The session manager is the interface to MNSLP's session table. It can be
 * used to get a session using its session ID. This class also serves as a
 * session factory, because it can verify that a created session_id is really
 * unique on this node.
 *
 * Instances of this class are thread-safe.
 */
class session_manager {

  public:
  
	session_manager(mnslp_config *conf);
	
	~session_manager();

	ni_session *create_ni_session();
		
	nf_session *create_nf_session(const session_id &sid);
	
	nr_session *create_nr_session(const session_id &sid);
	
	session *get_session(const session_id &sid);
	
	session *remove_session(const session_id &sid);

  private:
  
	pthread_mutex_t mutex;
	
	mnslp_config *config; // shared by many objects, don't delete
	
	hash_map<session_id, session *> session_table;
	
	typedef hash_map<session_id, session *>::const_iterator c_iter;

	session_id create_unique_id() const;

	// Large initial size to avoid resizing of the session table.
	static const int SESSION_TABLE_SIZE = 500000;
};


} // namespace mnslp

#endif // MNSLP__SESSION_MANAGER_H
