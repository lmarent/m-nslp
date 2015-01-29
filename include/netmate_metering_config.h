/// ----------------------------------------*- mode: C++; -*--
/// @file netmate_metering_config.h
/// The netmate_metering_config class.
/// ----------------------------------------------------------
/// $Id: netmate_metering_config.h 2558 2015-01-16 13:34:00 amarentes $
/// $HeadURL: https://./include/netmate_metering_config.h $
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
#ifndef MNSLP__NETMATE_METERING_CONFIG_H
#define MNSLP__NETMATE_METERING_CONFIG_H

#include "policy_action_mapping.h"

namespace mnslp {

/**
 * This class represents the configuration established for the netmate 
 * metering application
 **/
class netmate_metering_config : public metering_config
{
	public:
		netmate_metering_config();
		
		netmate_metering_config(const netmate_metering_config &rhs);
		
		~netmate_metering_config();
		
		virtual netmate_metering_config *copy() const;
		
		virtual void set_priority(int _priority);
		
		void set_metering_procedure(std::string metering_proc_name);
			
		void set_export_directory(std::string export_directory);

		void set_export_interval(double interval);    
		
		void set_export_procedure(std::string export_proc_name);
		
		virtual int get_priority() const;

		virtual int get_priority();
				
		std::string get_metering_procedure() const;
		
		std::string get_export_procedure() const;
		
		std::string get_export_directory() const;

		double get_export_interval() const;    

		virtual std::ostream &operator<<(std::ostream &out) const;
		
		virtual std::string to_string() const;
    
   		virtual bool is_equal(const metering_config &rhs);
   		
		virtual bool not_equal(const metering_config &rhs);


  protected:

		/// Name of the procedure that is implemented in the metering application
		std::string metering_procedure;
		
		/// This define the export procedure to be used for the export
		std::string export_procedure; 
		
		/// Export directory if apply for the export process.
		std::string export_directory;
		
		/// This defines how often the information is send to the collector.
		double export_report_interval;
	
};

}

#endif // MNSLP__NETMATE_METERING_CONFIG_H
