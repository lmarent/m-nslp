/// ----------------------------------------*- mode: C++; -*--
/// @file netmate_metering_config.cpp
/// The netmate_metering_config class.
/// ----------------------------------------------------------
/// $Id: netmate_metering_config.cpp 2558 2015-01-16 13:49:00 amarentes $
/// $HeadURL: https://./src/netmate_metering_config.cpp $
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

#include "netmate_metering_config.h"
#include <sstream>


namespace mnslp {


netmate_metering_config::netmate_metering_config()
{
	// NOTHING TO DO.
}


netmate_metering_config::netmate_metering_config(const netmate_metering_config &rhs)
{
	priority = rhs.priority;
	metering_procedure = rhs.metering_procedure;
	export_procedure = rhs.export_procedure;
	export_directory = rhs.export_directory;
	export_report_interval = rhs.export_report_interval;
}

	
netmate_metering_config::~netmate_metering_config()
{
	// NOTHING TO DO.
}

netmate_metering_config *
netmate_metering_config::copy() const
{
	netmate_metering_config *q = NULL;
	q = new netmate_metering_config(*this);
	return q;
}

void netmate_metering_config::set_metering_procedure(std::string metering_proc_name)
{
	metering_procedure = metering_proc_name;
}
    
void netmate_metering_config::set_priority(int _priority)
{
	priority = _priority;
}

void netmate_metering_config::set_export_directory(std::string _export_directory)
{
	export_directory = _export_directory;
}

void netmate_metering_config::set_export_interval(double interval)
{
	export_report_interval = interval;
}   

void netmate_metering_config::set_export_procedure(std::string export_proc_name)
{
	export_procedure = export_proc_name;
}     

int 
netmate_metering_config::get_priority()
{
	return priority;
}

int 
netmate_metering_config::get_priority() const
{
	return priority;
}

  
std::string 
netmate_metering_config::get_metering_procedure() const
{
	return metering_procedure;
}
    
std::string 
netmate_metering_config::get_export_procedure() const
{
	return export_procedure;
}
    
std::string 
netmate_metering_config::get_export_directory() const
{
	return export_directory;
}

double 
netmate_metering_config::get_export_interval() const
{
	return export_report_interval;
}

bool
netmate_metering_config::is_equal(const metering_config &rhs)
{
	const netmate_metering_config* b = dynamic_cast< const netmate_metering_config* >( &rhs );
		
	if (priority != rhs.get_priority())
		return false;
	
	if (metering_procedure.compare(b->metering_procedure) != 0) 
		return false;

	if (export_procedure.compare(b->export_procedure) != 0) 
		return false;

	if (export_directory.compare(b->export_directory) != 0) 
		return false;

	if (export_report_interval != b->export_report_interval) 
		return false;

	return true;
	

}

bool
netmate_metering_config::not_equal(const metering_config &rhs)
{
	return ! (is_equal(rhs));
}

std::ostream &
netmate_metering_config::operator<<(std::ostream &out) const
{
	out << " metering_procedure:" << metering_procedure
		<< " export_procedure:" << export_procedure
		<< " export_directory:" << export_directory
		<< " export_interval:" << export_report_interval
		<< std::endl;
	return out;
}    

std::string
netmate_metering_config::to_string() const
{
	std::ostringstream out;
	
	out << " metering_procedure:" << metering_procedure
		<< " export_procedure:" << export_procedure
		<< " export_directory:" << export_directory
		<< " export_interval:" << export_report_interval
		<< std::endl;

	return  out.str();
}    

   
} // namespace mnslp
