/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_xml_node_reader.h
/// The mnslp_xml_node_reader class.
/// ----------------------------------------------------------
/// $Id: mnslp_xml_node_reader.h 2558 2015-01-19 14:50:00 amarentes $
/// $HeadURL: https://./include/mnslp_xml_node_reader.h $
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
#ifndef MNSLP__MNSLP_XML_NODE_READER_H
#define MNSLP__MNSLP_XML_NODE_READER_H


#include <string>
#include <libxml/xmlreader.h>

namespace mnslp {

class mnslp_xml_node_reader
{

   public:
    
    mnslp_xml_node_reader(){}
    ~mnslp_xml_node_reader(){}

   protected:
	std::string processAttribute( xmlTextReaderPtr reader, 
								  std::string atrribute );
								  
	std::string processTextNode( int level, xmlTextReaderPtr reader );


};

}
#endif // MNSLP__MNSLP_XML_NODE_READER_H


