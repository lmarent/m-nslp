/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_daemon.h
/// The MNSLP daemon thread.
/// ----------------------------------------------------------
/// $Id: mnslp_daemon.h 2558 2014-11-08 09:32:00 amarentes $
/// $HeadURL: https://./include/mnslp_daemon.h $
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
#ifndef MNSLP__MNSLP_DAEMON_H
#define MNSLP__MNSLP_DAEMON_H

#include "protlib_types.h"

#include "ntlp_starter.h" // from NTLP

#include "mnslp_config.h"
#include "session_manager.h"
#include "policy_rule_installer.h"


namespace mnslp {
  using protlib::uint32;
  using ntlp::NTLPStarterParam;
  using ntlp::NTLPStarter;


/**
 * Encapsulated parameters for a mnslp_daemon thread.
 */
class mnslp_daemon_param : public ThreadParam {
  public:
	mnslp_daemon_param(const char *name, const mnslp_config &conf)
		: ThreadParam((uint32)-1, name), config(conf) { }

	mnslp_config config;
};


/**
 * The MNSLP daemon thread.
 *
 * This thread is the MNSLP daemon implemenation. It starts a NTLP thread,
 * registers with it and handles all messages it gets from the NTLP.
 */
class mnslp_daemon : public Thread {
  public:
  
	mnslp_daemon(const mnslp_daemon_param &param);
	
	virtual ~mnslp_daemon();

	virtual void startup();

	virtual void main_loop(uint32 thread_num);

	virtual void shutdown();

  private:
  
	mnslp_config config;

	session_manager session_mgr;
		
	policy_rule_installer *rule_installer;

	ThreadStarter<NTLPStarter, NTLPStarterParam> *ntlp_starter;
};


void init_framework();
void cleanup_framework();


} // namespace mnslp

#endif // MNSLP__MNSLP_DAEMON_H
