/// ----------------------------------------*- mode: C++; -*--
/// @file nop_policy_rule_installer.cpp
/// The nop_policy_rule_installer class.
/// ----------------------------------------------------------
/// $Id: nop_policy_rule_installer.cpp 2558 2015-01-16 11:00:00 amarentes $
/// $HeadURL: https://./src/nop_policy_rule_installer.cpp $
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

#include "address.h"
#include "logfile.h"

#include "nop_policy_rule_installer.h"


using namespace mnslp;
using namespace protlib::log;


#define LogError(msg) Log(ERROR_LOG, LOG_NORMAL, \
	"nop_policy_rule_installer", msg)
#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, \
	"nop_policy_rule_installer", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, \
	"nop_policy_rule_installer", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, \
	"nop_policy_rule_installer", msg)

#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, "nop_policy_rule_installer", \
	msg << " at " << __FILE__ << ":" << __LINE__)



nop_policy_rule_installer::nop_policy_rule_installer(
		mnslp_config *conf) throw () 
		: policy_rule_installer(conf) {

	// nothing to do
}


nop_policy_rule_installer::~nop_policy_rule_installer() throw () {
	// nothing to do
}


void nop_policy_rule_installer::setup() throw (policy_rule_installer_error) {

	policy_rule_installer::setup();
	LogDebug("NOP: setup()");

}


void nop_policy_rule_installer::check(const msg::mnslp_mspec_object *object)
		throw (policy_rule_installer_error) {

	LogDebug("NOP: check()");
}


void nop_policy_rule_installer::install(const mt_policy_rule *mt_rule)
		throw (policy_rule_installer_error) {

	if ( mt_rule != NULL )
		LogDebug("NOP: installing firewall policy rule " << *mt_rule);
}


void nop_policy_rule_installer::remove(const mt_policy_rule *mt_rule)
		throw (policy_rule_installer_error) {

	if ( mt_rule != NULL )
		LogDebug("NOP: removing firewall policy rule " << *mt_rule);
}

void nop_policy_rule_installer::remove_all()
		throw (policy_rule_installer_error) {

	LogDebug("NOP: removing all metering policy rules ");
}



// EOF
