/// ----------------------------------------*- mode: C++; -*--
/// @file netmate_policy_rule_installer.cpp
/// The netmate_policy_rule_installer class.
/// ----------------------------------------------------------
/// $Id: netmate_policy_rule_installer.cpp 2558 2015-01-16 11:32:00 amarentes $
/// $HeadURL: https://./src/netmate_policy_rule_installer.cpp $
// ===========================================================
//                      
// Copyright (C) 2005-2007, all rights reserved by
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
#include <sstream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#include "address.h"
#include "logfile.h"

#include "policy_rule_installer.h"

// curl includes
#include <curl/curl.h>

using namespace mnslp;
using mnslp::msg::information_code;
using namespace protlib::log;


#define LogError(msg) Log(ERROR_LOG, LOG_NORMAL, \
	"netmate_policy_rule_installer", msg)
#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, \
	"netmate_policy_rule_installer", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, \
	"netmate_policy_rule_installer", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, \
	"netmate_policy_rule_installer", msg)
#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, \
	"netmate_policy_rule_installer", \
	msg << " at " << __FILE__ << ":" << __LINE__)


netmate_ipfix_policy_rule_installer::netmate_ipfix_policy_rule_installer(
		mnslp_config *conf) throw () 
		: policy_rule_installer(conf) {

	// nothing to do
}


netmate_ipfix_policy_rule_installer::~netmate_ipfix_policy_rule_installer() throw () {
	// nothing to do
}


void netmate_ipfix_policy_rule_installer::setup()
		throw (policy_rule_installer_error) {

	/*
	 * Establishes the ip address, user and password for metering connection.
	 * It eliminates all metering activities previously configurated.
	 * It checks if the metering is working on this node, if not put the
	 * configuration as not posible to do metering. 
	 */
	policy_rule_installer::setup();
    

}



void netmate_ipfix_policy_rule_installer::check(const mt_policy_rule *object)
		throw (policy_rule_installer_error) {

	LogDebug("check(): ");

		throw policy_rule_installer_error("IP protocol has to be set",
			information_code::sc_signaling_session_failures,
			information_code::sigfail_plain_ip_not_permitted);
}


void netmate_ipfix_policy_rule_installer::install(const mt_policy_rule *object )
		throw (policy_rule_installer_error) {

	LogDebug("install(): ");


}


void netmate_ipfix_policy_rule_installer::remove(const mt_policy_rule *object)
		throw (policy_rule_installer_error) {

	LogDebug("remove(): " );


}


void netmate_ipfix_policy_rule_installer::remove_all()
		throw (policy_rule_installer_error) {


	LogDebug("remove_all(): ");
}

std::string  
netmate_ipfix_policy_rule_installer::create_connection_command(void) const
		throw () {


	LogDebug("create_connection_command(): ");
}

std::string  
netmate_ipfix_policy_rule_installer::create_action_command(
					const msg::mnslp_mspec_object *object) const
		throw () {


	LogDebug("create_action_command(): " );
}

std::string  
netmate_ipfix_policy_rule_installer::create_filter_command(
					const msg::mnslp_mspec_object *object) const
		throw () {


	LogDebug("create_filter_command(): ");
}


// EOF
