/// ----------------------------------------*- mode: C++; -*--
/// @file policy_action.cpp
/// The policy_action class.
/// ----------------------------------------------------------
/// $Id: policy_action.h 2558 2015-01-16 13:49:00 amarentes $
/// $HeadURL: https://./src/policy_action.cpp $
// ===========================================================
//                      
// Copyright (C) 2014-2015, all rights reserved by
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
#include <algorithm>
#include <string>
#include <limits>

#include "policy_action_mapping.h"
#include "netmate_policy_action_mapping.h"
#include "msg/information_code.h"
#include "policy_rule_installer.h"


namespace mnslp {


policy_action_mapping::policy_action_mapping():
	policy_field_mapping(), meter_config(NULL)
{
	// NOTHING TO DO.
}

policy_action_mapping::~policy_action_mapping()
{

    if (meter_config != NULL)
		delete(meter_config);
}

	
bool
policy_action_mapping::is_equal(const policy_action_mapping &rhs) const
{
	
	if (meter_config->not_equal(*(rhs.meter_config)))
		return false;
			
	return policy_field_mapping::operator ==(rhs);
	
}

bool
policy_action_mapping::not_equal(const policy_action_mapping &rhs) const
{
	return ! (is_equal(rhs));
}

std::ostream &
policy_action_mapping::operator<<(std::ostream &out)
{
	out << " metering_application:" << metering_application; 
	if (meter_config != NULL)
		meter_config->operator<<(out);
	
	policy_field_mapping::operator<<(out);
	
	return out;
}    

std::string
policy_action_mapping::to_string()
{
	ostringstream out;
	out << " metering_application:" << metering_application;
	if (meter_config != NULL)
		meter_config->operator<<(out);
		
	policy_field_mapping::operator<<(out);	
	
	return  out.str();
}    

int policy_action_mapping::get_priority() const
{
	if (meter_config != NULL)
		return meter_config->get_priority();
	else
		return std::numeric_limits<int>::max();
}

void policy_action_mapping::set_metering_configuration(metering_config *met_conf)
{
	meter_config = met_conf;
}

const metering_config * 
policy_action_mapping::get_metering_configuration() const
{
	if (meter_config != NULL)
		return meter_config;
	else
		return NULL;
}

policy_action_mapping *
policy_action_mapping::make(std::string app)
{
    std::transform(app.begin(), app.end(),app.begin(), ::toupper);
    if (app.compare("NETMATE") == 0)
		 return new netmate_policy_action_mapping();
	else
		return NULL;
}
   
} // namespace mnslp
