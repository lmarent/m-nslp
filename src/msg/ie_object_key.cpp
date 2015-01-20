/// ----------------------------------------*- mode: C++; -*--
/// @file ie_object_key.cpp
/// Keys used to store NSIS metering objects.
/// ----------------------------------------------------------
/// $Id: ie_object_key.cpp 2558 2014-12-26  $
/// $HeadURL: https://./src/msg/ie_object_key.cpp $
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

#include <sstream>
#include "msg/ie_object_key.h"


namespace protlib 
{

std::string 
ie_object_key::to_string() const
{
	std::ostringstream o1;
	o1 << "object_type:" << object_type;
	o1 << "seq_nbr:" << seq_nbr;
	std::string str6 = o1.str();
	return str6;

}

}
