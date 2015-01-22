/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_config.cpp
/// Configuration for a MNSLP instance.
/// ----------------------------------------------------------
/// $Id: mnslp_config.cpp 4118 2014-11-11 09:41:00 amarentes $
/// $HeadURL: https://./mnslp_config.cpp $
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

#include "mnslp_config.h"

using namespace mnslp;

void
mnslp_config::repository_init() 
{
  DLog("mnslp_config", "start - creating configuration parameter singleton");
  configpar_repository::create_instance(mnslp::mnslp_realm+1);
} // end namespace



/** sets the repository pointer and registers all parameters 
 *  (not done in constructor, because of allowing simulation hosts to initialize later)
 **/ 
void
mnslp_config::setRepository(configpar_repository* cfp_rep)
{
	cfgpar_rep= cfp_rep;

	if (cfgpar_rep == 0)
		throw  configParExceptionNoRepository();
	
	// register the realm
	cfgpar_rep->registerRealm(mnslp_realm, "mnslp", mnslpconf_maxparno);
	DLog("mnslp_config", "registered mnslp realm with realm id " << (int) mnslp_realm);
	
	// now register all parameters
	registerAllPars();
}


void 
mnslp_config::registerAllPars()
{
  DLog("mnslp_config::registerAllPars", "starting registering mnslp parameters.");

  // register all mnslp parameters now
  registerPar( new configpar<string>(mnslp_realm, mnslpconf_conffilename, "config", "configuration file name", true, "nsis-ka.conf") );
  registerPar( new configpar<uint32>(mnslp_realm, mnslpconf_dispatcher_threads, "dispatcher-threads", "number of dispatcher threads", true, 1) );
  registerPar( new configpar<uint32>(mnslp_realm, mnslpconf_ni_max_session_lifetime, "ni-max-session-lifetime", "NI session lifetime in seconds", true, 30, "s") );
  registerPar( new configpar<uint32>(mnslp_realm, mnslpconf_ni_max_retries, "ni-max-retries", "NI max retries", true, 3) );
  registerPar( new configpar<uint32>(mnslp_realm, mnslpconf_ni_response_timeout, "ni-response-timeout", "NI response timeout", true, 2, "s") );
  registerPar( new configpar<uint32>(mnslp_realm, mnslpconf_nf_max_session_lifetime, "nf-max-session-lifetime", "NF max session lifetime in seconds", true, 60, "s") );
  registerPar( new configpar<uint32>(mnslp_realm, mnslpconf_nf_response_timeout, "nf-response-timeout", "NF response timeout", true, 2, "s") );
  registerPar( new configpar<bool>(mnslp_realm, mnslpconf_ms_is_meter, "ms-is-meter", "NE is meter", true, true) );
  registerPar( new configpar<bool>(mnslp_realm, mnslpconf_ms_install_policy_rules, "ms-install-policy-rules", "ME install policy rules", true, true) );
  registerPar( new configpar<string>(mnslp_realm, mnslpconf_ms_export_config_file, "export-config", "export configuration file name", true, "/home/luis/NSIS/nsis-ka-0.97/etc/export.xml") );
  registerPar( new configpar<string>(mnslp_realm, mnslpconf_ms_filter_config_file, "filter-config", "filter configuration file name", true, "/home/luis/NSIS/nsis-ka-0.97/etc/filters.xml") );
  registerPar( new configpar<string>(mnslp_realm, mnslpconf_ms_metering_application, "metering-app", "metering applicartion name", true, "netmate") );
  registerPar( new configpar<string>(mnslp_realm, mnslpcong_ms_metering_user, "metering-user", "metering application user", true, "admin") );
  registerPar( new configpar<string>(mnslp_realm, mnslpcong_ms_metering_server, "metering-server", "metering http server", true, "localhost") );
  registerPar( new configpar<string>(mnslp_realm, mnslpcong_ms_metering_def_xsl, "xsl-for-decoding-metering-results", "metering results decoding", true, "/home/luis/NSIS/nsis-ka-0.97/etc/reply2.xsl") );
  registerPar( new configpar<string>(mnslp_realm, mnslpcong_ms_metering_port, "metering-port", "metering port", true, "12244") );

  registerPar( new configpar<uint32>(mnslp_realm, mnslpconf_nr_max_session_lifetime, "nr-max-session-lifetime", "NR max session lifetime in seconds", true, 60, "s") );
  
  DLog("mnslp_config::registerAllPars", "finished registering mnslp parameters.");
}


// EOF
