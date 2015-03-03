/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_libipfix.cpp
/// Tools for processing IPFIX messages in NSIS metering.
/// ----------------------------------------------------------
/// $Id: mnslp_libipfix.cpp 2558 2014-11-14 14:11:00 amarentes $
/// $HeadURL: https://./include/mnslp_libipfix.c $
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <iostream>
#include <stdexcept>      // std::out_of_range

#include <fcntl.h>
#include <netdb.h>

#include "msg/mnslp_ipfix_field.h"
#ifdef SSLSUPPORT
#include "ipfix_ssl.h"
#endif

#include "msg/mnslp_ipfix_message.h"
#include "msg/mnslp_ipfix_data_record.h"
#include "msg/mnslp_ipfix_exception.h"
#include "msg/mnslp_ipfix_template.h"

#include "msg/ipfix_t.h"


namespace mnslp {
  namespace msg {


/*----- defines ----------------------------------------------------------*/

#define NODEBUG


#define INSERTU16(b,l,val) \
        { uint16_t _t=htons((val)); memcpy((b),&_t,2); (l)+=2; }
#define INSERTU32(b,l,val) \
        { uint32_t _t=htonl((val)); memcpy((b),&_t,4); (l)+=4; }

#define INSERT_U16_NOENCODE(b,l,val) \
		{ uint16_t _t = val; memcpy((b),&_t,2); (l)+=2; }

#define INSERT_U32_NOENCODE(b,l,val) \
        { uint32_t _t=val; memcpy((b),&_t,4); (l)+=4; }

#define READ16_NOENCODE(val,b) \
        { memcpy((&val),b,2); }

#define READ32_NOENCODE(val,b) \
	    { memcpy((&val),b,4); } 


#define READ16(val,b) \
        { uint16_t _t; memcpy((&_t),b,2); val= ntohs(_t); }

#define READ32(val,b) \
		{ uint32_t _t; memcpy((&_t),b,4); val = ntohl(_t); }

const char *const mnslp_ipfix_message::ie_name = "mnslp_ipfix_mspec";


mnslp_ipfix_message::mnslp_ipfix_message( ):
		mnslp_mspec_object(OBJECT_TYPE, tr_mandatory, false),
		message(NULL), g_tstart(0), encode_network(true)
{    
    init( 0, IPFIX_VERSION );
}

mnslp_ipfix_message::mnslp_ipfix_message( int sourceid, int ipfix_version, bool _encode_network):
		mnslp_mspec_object(OBJECT_TYPE, tr_mandatory, false), 
		message(NULL), g_tstart(0), 
		encode_network(_encode_network), require_output(true)
{

    switch( ipfix_version ) 
    {
      case IPFIX_VERSION_NF9:
          break;
      case IPFIX_VERSION:
          break;
      default:
          throw mnslp_ipfix_bad_argument("Invalid IP Fix Version");
    }
    
    init( sourceid, ipfix_version );

}


mnslp_ipfix_message::~mnslp_ipfix_message( void )
{

    close( );
    g_tstart = 0;
        
}



mnslp_ipfix_message *
mnslp_ipfix_message::new_instance() const {
	mnslp_ipfix_message *q = NULL;
	catch_bad_alloc( q = new mnslp_ipfix_message() );
	return q;
}

mnslp_ipfix_message *
mnslp_ipfix_message::copy() const {
	mnslp_ipfix_message *q = NULL;
	
	catch_bad_alloc( q = new mnslp_ipfix_message(*this) );
	return q;
}

size_t 
mnslp_ipfix_message::get_serialized_size(coding_t coding) const 
{
	size_t val_return;
	val_return = HEADER_LENGTH;
	val_return = val_return + get_offset();
	
	// For GIST it is required to add padding if the message is not multiple
	// of 4.
	int num_padding = val_return %4;
	if (num_padding != 0)
		num_padding = 4 - num_padding;
		
	val_return = val_return + num_padding;
	return val_return;
}


bool 
mnslp_ipfix_message::deserialize_body(NetMsg &msg, uint16 body_length,
		IEErrorList &err, bool skip) {

	std::cout << "deserialize body:" << body_length << std::endl;

	uchar *messdef;
	int num_read=0;
	uchar *padding = new uchar[4];
	int num_padding = 0;

	messdef = (uchar *) malloc(sizeof(uchar) * body_length);
	uint32 start_pos = msg.get_pos();
	
	msg.copy_to(messdef, start_pos, body_length);
	close();
	
	num_read = mnslp_ipfix_import(messdef, body_length );

	std::cout << "after import message - bytes read:" << num_read 
			  << "body lenght:" << body_length << std::endl;

	// Manage the possible padding added in the origin.
	num_padding = body_length - num_read;	
	for (int i = 0 ; i < num_padding; i++ )
	{
		if (messdef[num_read + i] != 0){
			return false;
		}
	}
	std::cout << "here I am" << std::endl;
	msg.set_pos(start_pos + body_length);
	return true;	
}

bool 
mnslp_ipfix_message::check_body() const
{
    if (get_offset() > 0 ){
		return true;
    }
    else 
    {
		return false;
    }
}

bool mnslp_ipfix_message::equals_body(const mnslp_object &obj) const 
{

	const mnslp_ipfix_message *other
		= dynamic_cast<const mnslp_ipfix_message *>(&obj);

	return other != NULL
		&& this->isEqual(*other);
}

const char *
mnslp_ipfix_message::get_ie_name() const 
{
	return ie_name;
}


ostream &mnslp_ipfix_message::print_attributes(ostream &os) const 
{
	// TODO AM: Missing function definition
	return os;
}

void 
mnslp_ipfix_message::serialize_body(NetMsg &msg) const 
{
	uchar *padding;
	int num_padding = 0;
	int offset = 0;
	uint32 start_pos = msg.get_pos();	
	msg.copy_from(get_message(),start_pos, get_offset());	
	msg.set_pos(start_pos + get_offset());
	
	// For GIST it is required to add padding if the message is not multiple
	// of 4.
	offset = get_offset();
	num_padding = offset % 4;

	if ( num_padding != 0 ){
		num_padding = 4 - num_padding; // How many additional bytes are required.
		std::cout << "get offset:" << offset << "Num padding:" << num_padding << std::endl;
		padding = new uchar[4];
		std::cout << "voy aqui" << std::endl;
		for (int i = 0 ; i < num_padding; i++ ){
			padding[i] = 0;
		}
		uint32 start_pos2 = msg.get_pos();
		std::cout << "voy aqui 1" << std::endl;
		msg.copy_from(padding,start_pos2, num_padding);
		msg.set_pos(start_pos2 + num_padding);
		std::cout << "voy aqui 3" << std::endl;
	}
	
	std::cout << "Num bytes copied:" << msg.get_pos() - start_pos << std::endl;

}

void 
mnslp_ipfix_message::set_encode_mode(bool _encode_network)
{
	encode_network = _encode_network;
}

/*
 * name:        ipfix_add_vendor_information_elements()
 * parameters:  field_t- field structure
 * description: add information elements to global list of field types
 * remarks:
 */
void mnslp_ipfix_message::add_vendor_information_element( const ipfix_field_type_t &field_t )
{
	
    if ( ! g_tstart ) {  
        throw mnslp_ipfix_bad_argument("Message not initialized");
    }

    g_ipfix_fields.AddFieldType(field_t);

}

/*
 * name:        ipfix_add_vendor_information_elements()
 * parameters:  field_t- field structure
 * description: add information elements to global list of field types
 * remarks:
 */
void mnslp_ipfix_message::add_vendor_information_element(int _eno, int _ftype, ssize_t _length, 
														 int _coding, 
														 const std::string _name, 
														 const std::string _documentation )
{
	
    if ( ! g_tstart ) {  
        throw mnslp_ipfix_bad_argument("Message not initialized");
    }

    g_ipfix_fields.AddFieldType(_eno, _ftype, _length, _coding, _name, _documentation);
    
}

void mnslp_ipfix_message::allocate_additional_memory(size_t additional)
{
	if (message)
	{
		if ((message->offset + additional) > message->buffer_lenght)
		{
			message->buffer=(uchar *)realloc(message->buffer, message->offset + additional + 1);
			if (message->buffer == NULL)
				message->buffer_lenght = message->offset + additional + 1;
			else
				throw mnslp_ipfix_bad_argument("Could not increse the memory of the final char pointer");
		}
	}
	else
	{
		throw mnslp_ipfix_bad_argument("Not initializated message");
	}
}

/**
 * name:        init()
 * parameters:
 * remarks:     init module, read field type info.
 */
void mnslp_ipfix_message::init( int sourceid, int ipfix_version )
{

    ipfix_t       *i;

	try
	{
		i = new ipfix_t();
	}
	catch(std::bad_alloc& exc)
	{
		if ( i != NULL )
			delete( i );
        throw std::bad_alloc(exc);
    }

    i->sourceid  = sourceid;
    i->version   = ipfix_version;
    message = i;
    
    g_tstart = time(NULL);
    signal( SIGPIPE, SIG_IGN );
    g_lasttid = 255;

    g_ipfix_fields.initialize_forward();

    g_ipfix_fields.initialize_reverse();
    
}

/*
 * name:        new_template()
 * parameters:
 * return:     
 */
uint16_t  
mnslp_ipfix_message::new_template( int nfields )
{
    mnslp_ipfix_template  *t;

    if ( ! message  ) 
        throw mnslp_ipfix_bad_argument("Message is not initialized");
    
    if ( nfields<1 )
		throw mnslp_ipfix_bad_argument("Invalid number of fields");

    /** alloc mem
     */
    try
    {
		
		t = new mnslp_ipfix_template();
		
		/** generate template id, todo!
		 */
		g_lasttid++;
		t->set_id( g_lasttid );
		t->set_maxfields( nfields );

		/** add template to template container
		 */
		(message->templates).add_template(t);
		
		/** The message change, so it requires a new output 
		 */
		require_output = true;
		
		return t->get_template_id();
	
	}
	catch (std::bad_alloc& exc)
	{
		if (t != NULL)
			delete t;
		throw std::bad_alloc(exc);
	}

}

/*
 * name:        new_data_template()
 * parameters:
 * return:      
 */
uint16_t 
mnslp_ipfix_message::new_data_template( int nfields )
{
    uint16_t templid;
    mnslp_ipfix_template *templ;

	/** The message changed, so it requires a new output 
	 */
	require_output = true;

    templid = new_template( nfields );
    templ = message->templates.get_template(templid);
    templ->set_type(DATA_TEMPLATE);
    return templid;
    
}

/*
 * name:        ipfix_new_option_template()
 * parameters:
 * return:      
 */
uint16_t 
mnslp_ipfix_message::new_option_template( int nfields )
{
    uint16_t templid;
    mnslp_ipfix_template  *templ;

	/** The message changed, so it requires a new output 
	 */
	require_output = true;

    templid =  new_template( nfields );
    templ = message->templates.get_template(templid);
    templ->set_type(OPTION_TEMPLATE);
    return templid;
}


mnslp_ipfix_field
mnslp_ipfix_message::get_field_definition( int eno, int type )
{
	return g_ipfix_fields.get_field(eno, type);
}


/*
 * name:        add_field()
 * parameters:
 * return:      
 */
void 
mnslp_ipfix_message::add_field(  uint16_t templid,
               uint32_t         eno,
               uint16_t         type,
               uint16_t         length ) 
{
    
    mnslp_ipfix_template *templ;     
    templ = message->templates.get_template(templid);
    
    if (templ== NULL)
		throw mnslp_ipfix_bad_argument("template not included in the message");
    
    if ( ( templ->get_numfields() < templ->get_maxfields() )
         && (type < IPFIX_EFT_VENDOR_BIT) ) {
        /** set template field
         */

        mnslp_ipfix_field field = g_ipfix_fields.get_field(eno, type);
        
        if (encode_network)
			templ->add_data_field(length,KNOWN,1,field);
		else
			templ->add_data_field(length,KNOWN,0,field);

		/** The message changed, so it requires a new output 
		*/
		require_output = true;

    }
    else 
        throw mnslp_ipfix_bad_argument("Maximum number of field reach");
    
}

/*
 * name:        add_scope_field()
 * parameters:
 * return:      
 */
void 
mnslp_ipfix_message::add_scope_field( uint16_t templid,
                     uint32_t         eno,
                     uint16_t         type,
                     uint16_t         length )
{
    int i;
    mnslp_ipfix_template *templ; 
    
    templ = message->templates.get_template(templid);

    if (templ == NULL)
		throw mnslp_ipfix_bad_argument("template not included in the message");

    if ( templ->get_type() != OPTION_TEMPLATE )
		throw mnslp_ipfix_bad_argument("The template is not an option template");    

    if ( templ->get_numfields() < templ->get_maxfields() ) {

        mnslp_ipfix_field field = g_ipfix_fields.get_field(eno, type);
        
         if (encode_network)
			templ->add_scope_field(length,KNOWN,1,field);
		else
			templ->add_scope_field(length,KNOWN,0,field);

		/** The message changed, so it requires a new output 
		*/
		require_output = true;
			
    }
    else 
        throw mnslp_ipfix_bad_argument("Maximum number of field reach");
}

/*
 * name:        delete_template()
 * parameters:
 * return:
 */
void mnslp_ipfix_message::delete_template( uint16_t templid )
{

    /** remove template from list
     */
    message->templates.delete_template(templid);

	/** The message changed, so it requires a new output 
	 */
	require_output = true;
    
}

/*
 * name:        delete_all_templates()
 * parameters:
 * return:
 */
void mnslp_ipfix_message::delete_all_templates( void )
{
    /** remove all templates from list
     */
    message->templates.delete_all_templates();

	/** The message changed, so it requires a new output 
	 */
	require_output = true;

}


/*
 * name:        ipfix_make_template()
 * parameters:
 * return:      generates a new template and stores a pointer to it into the templ parameter
 */
uint16_t 
mnslp_ipfix_message::make_template( export_fields_t *fields, 
									    int nfields )
{
    mnslp_ipfix_template *t;
    uint16_t templid;
    int i;

    templid = new_data_template( nfields );

	try
	{
		for ( i=0; i<nfields; i++ ){
			add_field( templid, fields[i].eno, fields[i].ienum, fields[i].length);
		}
		
		/** The message changed, so it requires a new output 
		*/
		require_output = true;
		
		return templid;
				
	}
	catch(...)
	{
		delete_template( templid );
		throw mnslp_ipfix_bad_argument("Could not insert all fields"); 
	}

}

void 
mnslp_ipfix_message::finish_cs( void )
{
    size_t   buflen;
    uint8_t  *buf;
	std::string 	func = "finish_cs";

    /* finish current dataset */
    if ( (buf = message->cs_header) ==NULL )
        return;
    buflen = 0;
        
    if (encode_network == true){
		INSERTU16( buf+buflen, buflen, message->cs_tid );
		INSERTU16( buf+buflen, buflen, message->cs_bytes );
	}
	else{
		INSERT_U16_NOENCODE( buf+buflen, buflen, message->cs_tid );
		INSERT_U16_NOENCODE( buf+buflen, buflen, message->cs_bytes );		
	}
    message->cs_bytes = 0;
    message->cs_offset = 0;
    message->cs_header = NULL;
    message->cs_tid = 0;
}

/*
 * name:        _write_hdr()
 * parameters:
 * return:      0/-1
 */
void 
mnslp_ipfix_message::_write_hdr( void )
{
    int hsize, buflen = 0;
    time_t      now = time(NULL);

    /** fill ipfix header
     */
    char *buf; 
    if ( message->version == IPFIX_VERSION_NF9 ) 
    {
        hsize = 20;
        if ( hsize + message->offset > message->buffer_lenght ) 
			allocate_additional_memory(hsize + message->offset - message->buffer_lenght );
        
        /* write header before any other data */
		if ( message->offset > 0 ) 
			memmove( message->buffer + hsize, message->buffer, message->offset );

        message->seqno++;
        if (encode_network == true){
			INSERTU16( message->buffer+buflen, buflen, message->version );
			INSERTU16( message->buffer+buflen, buflen, message->nrecords );
			INSERTU32( message->buffer+buflen, buflen, ((now-g_tstart)*1000));
			INSERTU32( message->buffer+buflen, buflen, now );
			INSERTU32( message->buffer+buflen, buflen, message->seqno );
			INSERTU32( message->buffer+buflen, buflen, message->sourceid );
		}
		else{
			INSERT_U16_NOENCODE( message->buffer+buflen, buflen, message->version );
			INSERT_U16_NOENCODE( message->buffer+buflen, buflen, message->nrecords );
			INSERT_U32_NOENCODE( message->buffer+buflen, buflen, ((now-g_tstart)*1000));
			INSERT_U32_NOENCODE( message->buffer+buflen, buflen, now );
			INSERT_U32_NOENCODE( message->buffer+buflen, buflen, message->seqno );
			INSERT_U32_NOENCODE( message->buffer+buflen, buflen, message->sourceid );		
		}
		message->offset += hsize;
    }
    else 
    {
        hsize = IPFIX_HDR_BYTES;
        std::cout << "buffer offset:" << message->offset 
				  << "hdr bytes:" << IPFIX_HDR_BYTES << std::endl;
        if ( hsize + message->offset > message->buffer_lenght ) 
			allocate_additional_memory(hsize + message->offset - message->buffer_lenght );

        /* write header before any other data */
		if ( message->offset > 0 )
			memmove( message->buffer + hsize, message->buffer, message->offset );
		
        if (encode_network == true){
			INSERTU16( message->buffer+buflen, buflen, message->version );
			INSERTU16( message->buffer+buflen, buflen, message->offset + IPFIX_HDR_BYTES );
			INSERTU32( message->buffer+buflen, buflen, now );
			INSERTU32( message->buffer+buflen, buflen, message->nrecords );
			INSERTU32( message->buffer+buflen, buflen, message->sourceid );
		}
		else{
			INSERT_U16_NOENCODE( message->buffer+buflen, buflen, message->version );
			INSERT_U16_NOENCODE( message->buffer+buflen, buflen, message->offset + IPFIX_HDR_BYTES );
			INSERT_U32_NOENCODE( message->buffer+buflen, buflen, now );
			INSERT_U32_NOENCODE( message->buffer+buflen, buflen, message->nrecords );
			INSERT_U32_NOENCODE( message->buffer+buflen, buflen, message->sourceid );		
		}
		message->length = message->offset + IPFIX_HDR_BYTES;
		message->exporttime = now;
		message->offset += hsize;		
    }
    std::cout << "final message size:" << message->offset
		      << "message lenght:" << message->length << std::endl;
}



/* name:        _output_flush()
 * parameters:
 * remarks:    
 */
void 
mnslp_ipfix_message::_output_flush( void )
{
    int               ret;
    std::string 	  func = "_output_flush";

    if ( (message==NULL) || (message->offset==0) )
        return;
    
    if ( message->cs_tid > 0 ) {
        /* finish current dataset */
        finish_cs( );
    }

}

/*
 * name:        ipfix_close()
 * parameters:
 * return:      0 = ok, -1 = error
 */
void 
mnslp_ipfix_message::close( void )
{
    if ( message != NULL )
        delete (message);
   
    g_ipfix_fields.clear();
    data_list.clear();
}



/*
 * name:        _write_template()
 * parameters:
 * return:      0/-1
 */
void 
mnslp_ipfix_message::_write_template( mnslp_ipfix_template  *templ )
{
    size_t            buflen, tsize=0, ssize=0, osize=0;
    uchar             *buf;
    uint16_t          tmp16;
    int               i, n;

    /** calc template size
     */
    if ( templ->get_type() == OPTION_TEMPLATE ) {
        for ( i=0, ssize=0; i<templ->get_number_scopefields(); i++ ) {
            ssize += 4;
            if ((templ->get_field(i).elem).get_field_type().eno != IPFIX_FT_NOENO)
                ssize += 4;
        }
        std::cout << "Option Template - number of fields:" << templ->get_numfields() << std::endl;
        for ( osize=0; i<templ->get_numfields(); i++ ) {
            osize += 4;
            if ((templ->get_field(i).elem).get_field_type().eno != IPFIX_FT_NOENO)
                osize += 4;
        }
        tsize = 10 + osize + ssize;
    } 
    else {
        for ( tsize=8,i=0; i < templ->get_numfields(); i++ ) {
            tsize += 4;
            if ((templ->get_field(i).elem).get_field_type().eno != IPFIX_FT_NOENO)
                tsize += 4;
        }
    }
    
    std::cout << "template size:" << tsize << std::endl;

    /* check space */
    if ( tsize + message->offset > message->buffer_lenght ) 
    {
         allocate_additional_memory(tsize + message->offset - message->buffer_lenght );
    }

    /* write template prior to data */
    if ( message->offset > 0 ) 
    {
         memmove( message->buffer + tsize, message->buffer, message->offset );
         if ( message->cs_tid )
             message->cs_header += tsize;
             message->cs_offset += tsize;
    }

    buf = message->buffer;
    buflen = 0;

    /** insert template set into buffer
     */
    if ( message->version == IPFIX_VERSION_NF9 ) {
        if ( templ->get_type() == OPTION_TEMPLATE ) {
			if (encode_network == true){
				INSERTU16( buf+buflen, buflen, IPFIX_SETID_OPTTEMPLATE_NF9);
				INSERTU16( buf+buflen, buflen, tsize );
				INSERTU16( buf+buflen, buflen, templ->get_template_id() );
				INSERTU16( buf+buflen, buflen, ssize );
				INSERTU16( buf+buflen, buflen, osize );
			}
			else{
				INSERT_U16_NOENCODE( buf+buflen, buflen, IPFIX_SETID_OPTTEMPLATE_NF9);
				INSERT_U16_NOENCODE( buf+buflen, buflen, tsize );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_template_id() );
				INSERT_U16_NOENCODE( buf+buflen, buflen, ssize );
				INSERT_U16_NOENCODE( buf+buflen, buflen, osize );				
			}
        } 
        else {
            if (encode_network == true){
				INSERTU16( buf+buflen, buflen, IPFIX_SETID_TEMPLATE_NF9);
				INSERTU16( buf+buflen, buflen, tsize );
				INSERTU16( buf+buflen, buflen, templ->get_template_id() );
				INSERTU16( buf+buflen, buflen, templ->get_numfields() );
			}
			else{
				INSERT_U16_NOENCODE( buf+buflen, buflen, IPFIX_SETID_TEMPLATE_NF9);
				INSERT_U16_NOENCODE( buf+buflen, buflen, tsize );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_template_id() );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_numfields() );
			}
        }
    } 
    else 
    {
        if ( templ->get_type() == OPTION_TEMPLATE ) {
			if (encode_network == true){
				INSERTU16( buf+buflen, buflen, IPFIX_SETID_OPTTEMPLATE );
				INSERTU16( buf+buflen, buflen, tsize );
				INSERTU16( buf+buflen, buflen, templ->get_template_id() );
				INSERTU16( buf+buflen, buflen, templ->get_numfields() );
				INSERTU16( buf+buflen, buflen, templ->get_number_scopefields() );
			}
			else{
				INSERT_U16_NOENCODE( buf+buflen, buflen, IPFIX_SETID_OPTTEMPLATE );
				INSERT_U16_NOENCODE( buf+buflen, buflen, tsize );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_template_id() );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_numfields() );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_number_scopefields() );				
			}
        } 
        else {
            if (encode_network == true){
				INSERTU16( buf+buflen, buflen, IPFIX_SETID_TEMPLATE);
				INSERTU16( buf+buflen, buflen, tsize );
				INSERTU16( buf+buflen, buflen, templ->get_template_id() );
				INSERTU16( buf+buflen, buflen, templ->get_numfields() );
			}
			else{
				INSERT_U16_NOENCODE( buf+buflen, buflen, IPFIX_SETID_TEMPLATE);
				INSERT_U16_NOENCODE( buf+buflen, buflen, tsize );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_template_id() );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_numfields() );				
			}
        }
    }

    if ( templ->get_type() == OPTION_TEMPLATE ) 
    {
        n = templ->get_numfields();
        std::cout << "num scope fields:" << templ->get_number_scopefields() << std::endl;
        for ( i=0; i<templ->get_number_scopefields(); i++ ) {
            if ( (templ->get_field(i).elem).get_field_type().eno == IPFIX_FT_NOENO ) {
				if (encode_network == true){
					INSERTU16( buf+buflen, buflen, (templ->get_field(i).elem).get_field_type().ftype );
					INSERTU16( buf+buflen, buflen, templ->get_field(i).flength );
				}
				else{
					INSERT_U16_NOENCODE( buf+buflen, buflen, (templ->get_field(i).elem).get_field_type().ftype );
					INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_field(i).flength );
				}
            } 
            else {
                tmp16 = (templ->get_field(i).elem).get_field_type().ftype|IPFIX_EFT_VENDOR_BIT;
                if (encode_network == true){
					INSERTU16( buf+buflen, buflen, tmp16 );
					INSERTU16( buf+buflen, buflen, templ->get_field(i).flength );
					INSERTU32( buf+buflen, buflen, (templ->get_field(i).elem).get_field_type().eno );
				}
				else{
					INSERT_U16_NOENCODE( buf+buflen, buflen, tmp16 );
					INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_field(i).flength );
					INSERT_U32_NOENCODE( buf+buflen, buflen, (templ->get_field(i).elem).get_field_type().eno );					
				}
            }
        }
    } 
    else {
        i = 0;
        n = templ->get_numfields();
    }

    for ( ; i<templ->get_numfields(); i++ )
    {
        std::cout << "entro field:" << i << std::endl;
        if ( (templ->get_field(i).elem).get_field_type().eno == IPFIX_FT_NOENO ) {
			if (encode_network == true){
				INSERTU16( buf+buflen, buflen, (templ->get_field(i).elem).get_field_type().ftype );
				INSERTU16( buf+buflen, buflen, templ->get_field(i).flength );
			}
			else{
				INSERT_U16_NOENCODE( buf+buflen, buflen, (templ->get_field(i).elem).get_field_type().ftype );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_field(i).flength );				
			}
        } 
        else {
            tmp16 = (templ->get_field(i).elem).get_field_type().ftype|IPFIX_EFT_VENDOR_BIT;
            if (encode_network == true){
				INSERTU16( buf+buflen, buflen, tmp16 );
				INSERTU16( buf+buflen, buflen, templ->get_field(i).flength );
				INSERTU32( buf+buflen, buflen, (templ->get_field(i).elem).get_field_type().eno );
			}
			else{
				INSERT_U16_NOENCODE( buf+buflen, buflen, tmp16 );
				INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_field(i).flength );
				INSERT_U32_NOENCODE( buf+buflen, buflen, (templ->get_field(i).elem).get_field_type().eno );				
			}
        }
    }
    templ->set_time_send( time(NULL) );
	std::cout << "buffer len:" << buflen << std::endl;
    message->offset += buflen;
    if ( message->version == IPFIX_VERSION_NF9 )
         message->nrecords++;

}

void
mnslp_ipfix_message::output(void)
{
	
	if (require_output == true)
	{
		
		// Clear the internal buffer.
		message->reinitiate_buffer();
		
		std::list<int>::iterator it;
		std::list<int> lst_templates = get_template_list();
		
		// Put in internal buffer template definition and data related for non-option
		// templates
		for ( it = lst_templates.begin(); it != lst_templates.end(); ++it)
		{
			mnslp_ipfix_template *templ = get_template(*it);
			if (templ->get_type() == OPTION_TEMPLATE)
				output_set( *it );
		}
		
		// Put in internal buffer template definition and data related. For option
		// templates.
		for ( it = lst_templates.begin(); it != lst_templates.end(); ++it)
		{
			mnslp_ipfix_template *templ = get_template(*it);
			if (templ->get_type() != OPTION_TEMPLATE)
				output_set( *it );
		}

		// Calculate message header information and put it on the buffer.
		_write_hdr( );
		
		/** The message changed, so it requires a new output 
		*/
		require_output = false;
	}

}

/*
 * name:        output_set()
 * parameters:  Write in the buffer template definition and all 
 * 				data records related to templateid given as parameter.
 * return:      
 */
void 
mnslp_ipfix_message::output_set( uint16_t templid )
{
    int               i, newset_f=0;
    size_t            buflen, datasetlen;
    uint8_t           *p, *buf;
    mnslp_ipfix_template * templ; 
    
    templ = message->templates.get_template(templid);

    /** parameter check
     */
    if (templ==NULL)
        throw mnslp_ipfix_bad_argument("template not included in the message");
    	
    /** writes the templates if it was not done before
     */
    if ( templ->get_tsend() == 0 ){
        _write_template( templ ); 
    }
    

    /** get size of data set, check space
    */ 
    if ( templ->get_template_id() == message->cs_tid ) 
    {
        newset_f = 0;
        datasetlen = 0;
    }
    else 
    {
        if ( message->cs_tid > 0 ) {
            finish_cs( );
        }
        newset_f = 1;
        datasetlen = 4;
    }
        	
    // insert the data records associated with the template.
    for ( int data_index= 0; data_index < data_list.size(); data_index++)
	{
		
		if (data_list[data_index].get_template_id() == templ->get_template_id())
		{
			
			mnslp_ipfix_data_record g_data = data_list[data_index];
			for ( i=0; i < templ->get_numfields(); i++ )
			{
				mnslp_ipfix_field_key field_key = mnslp_ipfix_field_key((templ->get_field(i).elem).get_field_type().eno, 
																		(templ->get_field(i).elem).get_field_type().ftype);
				if ( templ->get_field(i).flength == IPFIX_FT_VARLEN ) {
					
					if ( g_data.get_length(field_key) > 254 )
						datasetlen += 3;
					else
						datasetlen += 1;
				} 
				else 
					if ( g_data.get_length(field_key) > templ->get_field(i).flength )
						throw mnslp_ipfix_bad_argument("Data length greater than field definition lenght");
						
				datasetlen += g_data.get_length(field_key);
			}


			if ( (message->offset + datasetlen) > message->buffer_lenght )
				allocate_additional_memory(datasetlen + message->offset - message->buffer_lenght );
			
			// fill buffer 
			buf    = (uint8_t*)(message->buffer) + message->offset;
			buflen = 0;

			if ( newset_f ) {
				
				// insert data set 
				message->cs_bytes = 0;
				message->cs_header = buf;
				message->cs_offset = message->offset;
				message->cs_tid = templ->get_template_id();
				
				
				if (encode_network == true){
					INSERTU16( buf+buflen, buflen, templ->get_template_id() );
					INSERTU16( buf+buflen, buflen, datasetlen );
				}
				else{
					INSERT_U16_NOENCODE( buf+buflen, buflen, templ->get_template_id() );
					INSERT_U16_NOENCODE( buf+buflen, buflen, datasetlen );			
				}
			}
			
			for ( i=0; i < templ->get_numfields(); i++ ) {
								
				mnslp_ipfix_field_key field_key = mnslp_ipfix_field_key((templ->get_field(i).elem).get_field_type().eno, 
																		(templ->get_field(i).elem).get_field_type().ftype);
				
				if ( templ->get_field(i).flength == IPFIX_FT_VARLEN ) {
					if ( g_data.get_length(field_key) > 254 ) {
						*(buf+buflen) = 0xFF;
						buflen++;
						if (encode_network == true){
							INSERTU16( buf+buflen, buflen, g_data.get_length(field_key) );
						}
						else{
							INSERT_U16_NOENCODE( buf+buflen, buflen, g_data.get_length(field_key) );
						}
					}
					else {
						*(buf+buflen) = g_data.get_length(field_key);
						buflen++;
					}
				}
				
				(templ->get_field(i).elem).encode( g_data.get_field(field_key), 
												   buf+buflen, 
												   templ->get_field(i).relay_f );

				buflen += g_data.get_length(field_key);
			}
			message->nrecords ++;
			message->offset += buflen;
			message->cs_bytes += buflen;
			finish_cs( );
		    if ( message->version == IPFIX_VERSION )
				message->seqno ++;
		}
		std::cout << "data len:" << data_index << std::endl;
	}
    _output_flush( );
		        
}


void 
mnslp_ipfix_message::include_data( uint16_t templid, 
								   mnslp_ipfix_data_record &data )
{
    int i;
	mnslp_ipfix_template *templ;
	
	templ = message->templates.get_template(templid);
	
    if ( !templ ) {
        throw mnslp_ipfix_bad_argument("Parameter template not defined");
    }
       
    if ( ( templ->get_numfields() != data.get_num_fields())  ){
        throw mnslp_ipfix_bad_argument("The number of field values is different from template's fields");
	}
				
    data_list.push_back(data);

	/** The message changed, so it requires a new output 
	*/
	require_output = true;
    
}

uchar * 
mnslp_ipfix_message::get_message(void) const
{
	if (message != NULL) 
	{
		if (require_output == true)
		{
			return NULL;
		}
		else
			return message->buffer;
	}
	else
		return NULL;
}



int 
mnslp_ipfix_message::get_offset(void) const
{
	if (message != NULL){
		if (require_output == true)
		{
			return 0;
		}
			return message->offset;
	}
	else
		return 0;
}

mnslp_ipfix_message::mnslp_ipfix_message(uchar * param, size_t message_length, bool _encode_network):
	mnslp_mspec_object(OBJECT_TYPE, tr_mandatory, false),
	message(NULL), g_tstart(0), encode_network(_encode_network), require_output(true)
{
	int nrecords;
	nrecords = mnslp_ipfix_import(param, message_length );

}


/*
 * name:        mnslp_ipfix_parse_hdr()
 * parameters:
 */
void 
mnslp_ipfix_message::mnslp_ipfix_parse_hdr( uchar *mes, int offset )
{
    uint16_t _count, _length, _version;
    uint32_t _sysuptime, _unixtime, _exporttime, _seqno, _sourceid; 
        
    std::string func = "mnslp_ipfix_parse_hdr";
    if (encode_network == true){
		READ16(_version, mes);
	}
	else{
		READ16_NOENCODE(_version,mes);
	}
		
    switch ( _version ) {
      case IPFIX_VERSION_NF9:

		  std::cout << " Arrive 1.3" << std::endl;
		  fflush( stdout) ;

          if ( (offset) < IPFIX_HDR_BYTES_NF9 )
              throw mnslp_ipfix_bad_argument("Invalid message given");
          
          if (encode_network == true)
          {
			READ16(_count, mes+2);
			READ32(_sysuptime, mes+4);
			READ32(_unixtime, mes+8);
			READ32(_seqno, mes+12);
			READ32(_sourceid, mes+16);
		  }
		  else
		  {
			 READ16_NOENCODE(_count, mes+2);
			 READ32_NOENCODE(_sysuptime,mes+4);
			 READ32_NOENCODE(_unixtime, mes+8);
			 READ32_NOENCODE(_seqno, mes+12);
			 READ32_NOENCODE(_sourceid, mes+16);			 
		  }
          
          /* Initialize the message object */
		  init(_sourceid, _version); 
		  message->version = _version;
		  message->count = _count;
          message->sysuptime = _sysuptime;
          message->unixtime = _unixtime;
          message->seqno = _seqno;
          message->sourceid = _sourceid;
          break;

      case IPFIX_VERSION:

		  std::cout << " Arrive 1.4" << std::endl;

          if ( (offset) < IPFIX_HDR_BYTES )
              throw mnslp_ipfix_bad_argument("Length of the message header is less than required");

		  if (encode_network == true){
		      READ16(_length, mes+2);
		      READ32(_exporttime, mes+4);		      
		      READ32(_seqno, mes+8);
		      READ32(_sourceid, mes+12);
		  }
		  else{
			  READ16_NOENCODE(_length,mes+2);
			  READ32_NOENCODE(_exporttime,mes+4);
			  READ32_NOENCODE(_seqno,mes+8);
			  READ32_NOENCODE(_sourceid,mes+12);
		  }
          std::cout << "header received" << _version 
					<< "length:"         << _length
					<< "exporttime:"	 << _exporttime
					<< "seqno:"			 << _seqno
					<< "sourceid:"		 << _sourceid << std::endl;

		  /* Initialize the message object */
		  init(_sourceid, _version); 
          message->version = _version;
          message->length = _length;
          message->exporttime = _exporttime;
          message->seqno = _seqno;
          message->sourceid = _sourceid;
          break;

      default:
		  std::cout << " Arrive 1.5" << std::endl;
          message->version = -1;
          throw mnslp_ipfix_bad_argument("Invalid Message Version");
    }
	std::cout << " Arrive 1.2" << std::endl;
	
}


/*
 * name:        mnslp_ipfix_decode_trecord()
 * func:        create or update template inside ifh. 
 * return:      0/-1
 */
void
mnslp_ipfix_message::mnslp_ipfix_decode_trecord( int setid,
												 const uchar     *buf2,
												 size_t         len,
												 int            *nread )
{
    mnslp_ipfix_template *t;
    uint16_t          templid, nfields, nscopefields, ndatafields;
    int               i;
    size_t            offset;
    std::string       func = "ipfix_decode_trecord";

    errno = EIO;
    const uint8_t * buf = reinterpret_cast<const uint8_t*>(buf2);

    /** read template header
     */
    std::cout << "Reading template - setid::" << setid << std::endl;
    switch( setid ) {
      case IPFIX_SETID_OPTTEMPLATE:
		  std::cout << "read option template" << std::endl;
          if ( len<6 )
              throw mnslp_ipfix_bad_argument("invalid message lenght");
          if (encode_network == true){    
		      READ16(templid, buf);
			  READ16(nfields, buf+2);
              READ16(nscopefields, buf+4);
		  }
		  else{
		      READ16_NOENCODE(templid,buf);
			  READ16_NOENCODE(nfields,buf+2);
              READ16_NOENCODE(nscopefields,buf+4);			  
		  }
          offset = 6;
          ndatafields = nfields - nscopefields;
          break;
      case IPFIX_SETID_OPTTEMPLATE_NF9:
      {          
          std::cout << "read option template 2" << std::endl;
          size_t scopelen, optionlen;
          if ( len<6 )
              throw mnslp_ipfix_bad_argument("invalid message lenght");
          
          if (encode_network == true){
			  READ16(templid, buf);
			  READ16(scopelen, buf+2);
			  READ16(optionlen, buf+4);
		  }
		  else{
			  READ16_NOENCODE(templid, buf);
			  READ16_NOENCODE(scopelen,buf+2);
			  READ16_NOENCODE(optionlen, buf+4);			  
	      }
		  
          offset = 6;
          if ( (scopelen+optionlen) < len ) {
              throw mnslp_ipfix_bad_argument("Invalid template data");
          } 
          nscopefields = scopelen / 4;
          ndatafields  = optionlen / 4;
          nfields   = nscopefields + ndatafields;
          break;
      }
      case IPFIX_SETID_TEMPLATE:
      case IPFIX_SETID_TEMPLATE_NF9:
          std::cout << "data template" << std::endl;
          if ( len<4 )
             throw mnslp_ipfix_bad_argument("invalid message lenght");
          if (encode_network == true){
			  READ16(templid, buf);
              READ16(nfields, buf+2);
          }
          else{
			  READ16_NOENCODE(templid,buf);
              READ16_NOENCODE(nfields,buf+2);
		  }
					
          offset = 4;
          ndatafields  = nfields;
          nscopefields = 0;
          break;
      default:
          throw mnslp_ipfix_bad_argument("Invalid template the setid is wrong");
    }

    if ( nfields == 0 ) {
        /** 
         * template withdrawal message
         */
        if ( templid == setid ) {
            delete_all_templates();
        }
        else{
			message->templates.delete_template(templid);
		}

        *nread = offset;
    }


    /** 
     * gets template node ( create or obtain it from the message).
     */
    try
    {
		t = message->templates.get_template(templid);
        // Replace the template.
        message->templates.delete_template( templid );
    }
    catch(mnslp_ipfix_bad_argument bad){
		// The template must be created
	}	
	
	/** alloc mem
	*/
	switch( setid ) {
		case IPFIX_SETID_OPTTEMPLATE:
		case IPFIX_SETID_OPTTEMPLATE_NF9:
			templid = new_option_template(nfields);
			break;
		case IPFIX_SETID_TEMPLATE:
		case IPFIX_SETID_TEMPLATE_NF9:
			templid = new_data_template( nfields );
			break;
	}
	t = message->templates.get_template(templid);	
    t->set_id(templid);
    t->set_maxfields(nfields);
    
    /** read field definitions
     */
    try
    {
		for( i=0;  i < nfields; i++ ) {
			if (offset >= len)
				throw mnslp_ipfix_bad_argument("Field in template has a longer length than message"); 
			else
				if (i < nscopefields)
					read_field(t, buf+offset, len-offset, &offset, true);
				else
					read_field(t, buf+offset, len-offset, &offset, false);
		}
		*nread = offset;		
	}
	catch (mnslp_ipfix_bad_argument bad)
	{
		message->templates.get_template(templid);
		throw mnslp_ipfix_bad_argument("Could not read the template information"); 
	}
	
	std::cout << "Finish Ok reading template" << std::endl;
}


int
mnslp_ipfix_message::get_num_templates(void) const
{
	if (message == NULL)
		return 0;
	else
		return (message->templates).get_num_templates();
}

void 
mnslp_ipfix_message::read_field(mnslp_ipfix_template *templ, 
							    const uint8_t  *buf, 
							    size_t   buflen, 
							    size_t   *nread,
							    bool 	 isscopefield )
{
    uint16_t ftype;
    uint16_t length;
    int unknown_f;
    int relay_f;
    int eno;
    
    if (encode_network == true)
		relay_f = 1;
	else 
		relay_f = 0;    

    if ( buflen<4 ) 
    {
        throw mnslp_ipfix_bad_argument("Buffer has not enough information to read a field");
    }
    
    if (templ== NULL){
		throw mnslp_ipfix_bad_argument("Invalid template definition for reading a field");
	}
    
    // Verifies that the field can be added
    if ( ( templ->get_numfields() >= templ->get_maxfields() )){
		throw mnslp_ipfix_bad_argument("It cannot insert the new field, max field reached");	
	}
    
    // Reads the field from the buffer.
    if (encode_network == true){
		READ16(ftype, buf);
		READ16(length, buf+2);
	}
	else{
		READ16_NOENCODE(ftype, buf);
		READ16_NOENCODE(length, buf+2);		
	}
    (*nread) += 4;

    if ( ftype & IPFIX_EFT_VENDOR_BIT ) {
        if ( buflen<8 ) {
            throw mnslp_ipfix_bad_argument("Invalid buffer len for reading a vendor field");
        }
        ftype &= (~IPFIX_EFT_VENDOR_BIT);
        if (encode_network == true){
			READ32(eno, buf+4);
		}
		else{
			READ32_NOENCODE(eno,buf+4);
		}
        (*nread) += 4;
    } 
    else {
        eno = IPFIX_FT_NOENO;
    }
		
    try
    {
        mnslp_ipfix_field field = g_ipfix_fields.get_field(eno, (int) ftype);
        if (isscopefield)
			templ->add_scope_field(length, KNOWN, relay_f, field);
        else
			templ->add_data_field(length, KNOWN, relay_f, field);
				
	}
	catch(mnslp_ipfix_bad_argument e)
	{	
        /** unknown field -> generate node
         */
        /* mark node, so we can drop it later */
        mnslp_ipfix_field field = g_ipfix_fields.get_field(0, 0);
        if (isscopefield)
			templ->add_scope_field(length, UNKNOWN, relay_f, field);
		else
			templ->add_data_field(length, UNKNOWN, relay_f, field);
	}
		
}


/*
 * name:        mnslp_ipfix_decode_datarecord()
 * parameters:
 * desc:        this func parses and exports the ipfix data set
 * todo:        parse message before calling this func
 */
void mnslp_ipfix_message::mnslp_ipfix_decode_datarecord( mnslp_ipfix_template *templ,
														 uchar      		  *buf, 
														 int                buflen,
														 int                *nread )
{
    uint8_t       *p;
    int           i, len, bytesleft;
    std::string   func = "mnslp_ipfix_decode_datarecord";
    mnslp_ipfix_data_record g_data(templ->get_template_id());
    char salida[30];
    
    std::cout << "Reading datarecord" << std::endl;
    
    /** parse message
     */
    bytesleft = buflen;
    *nread    = 0;
    p         = reinterpret_cast<uint8_t*>(buf);
        
    for ( i=0; i < templ->get_numfields(); i++ ) {

        len = templ->get_field(i).flength;
        if ( len == IPFIX_FT_VARLEN ) {
            len =*p;
            p++;
            (*nread) ++;
            if ( len == 255 ) {
				if (encode_network == true){
					READ16(len, p);
				}
				else{
					READ16_NOENCODE(len,p);
				}
                p += 2;
                (*nread) +=2;
            }
        }
		
        bytesleft -= len;
        if ( bytesleft < 0 ) {
            throw mnslp_ipfix_bad_argument("Invalid buffer len for reading the data");
        }
		
		
		
        mnslp_ipfix_value_field value;
        if (encode_network){
			value = (templ->get_field(i).elem).decode(p,len, 1);
		}
		else{
			value = (templ->get_field(i).elem).decode(p,len, 0);
		}

		(templ->get_field(i).elem).snprint(salida, 30, value);
				
        g_data.insert_field((templ->get_field(i).elem).get_field_type().eno, 
						    (templ->get_field(i).elem).get_field_type().ftype, value); 
						    
        p        += len;
        (*nread) += len;
    }
    
    data_list.push_back(g_data);
    
    std::cout << "Finish Reading datarecord" << std::endl;

}

mnslp_ipfix_template * 
mnslp_ipfix_message::get_template(uint16_t templid)
{
	if (message != NULL){
		return message->templates.get_template(templid);
	}
	else{
		return NULL;
	}
}

mnslp_ipfix_template * 
mnslp_ipfix_message::get_template(uint16_t templid) const
{
	if (message != NULL)
		return message->templates.get_template(templid);
	else
		return NULL;
}


int 
mnslp_ipfix_message::mnslp_ipfix_import( uchar  *buffer,
									     size_t message_length )
{
    uchar                 *buf;                 /* ipfix payload */
    uint16_t             setid, setlen;        /* set id, set lenght */
    int                  i, nread, offset;     /* counter */
    int                  bytes, bytesleft;
    int                  err_flag = 0;
    std::string          func = "mnslp_ipfix_import";

	std::cout << "mnslp_ipfix_import" << message_length << std::endl;

    if (message_length < 2)
		throw mnslp_ipfix_bad_argument("Invalid Message");

    mnslp_ipfix_parse_hdr( buffer, message_length );
	
    switch( message->version ) {
      case IPFIX_VERSION_NF9:
          buf   = buffer;
          nread = IPFIX_HDR_BYTES_NF9;
          break;
      case IPFIX_VERSION:
          buf   = buffer;
          nread = IPFIX_HDR_BYTES;
          break;
      default:
          throw mnslp_ipfix_bad_argument("Invalid IPFIX Version");
    }
	
    /** read ipfix sets
     */
    for ( i=0; (nread+4) < message_length; i++ ) {


        if ( (message->version == IPFIX_VERSION_NF9)
             && (i>=message->count) ) {
            break;
        }

        /** read ipfix record header (set id, lenght). Verifies that the lenght 
         *  given is valid.
         */
        if (encode_network == true){
			READ16(setid, buf+nread);
			READ16(setlen, buf+nread+2);
		}
		else{
			READ16_NOENCODE(setid, buf+nread);
			READ16_NOENCODE(setlen, buf+nread+2);
		}
		std::cout << "read setid:" << setid << std::endl;
		std::cout << "setlen:" << setlen << std::endl;	
        nread  += 4;
        if ( setlen < 4 ) {
            // std::string err1 = func + "set" + std::to_string(i+1) + ": invalid set length " + std::to_string(setlen);
            continue;
        }
        setlen -= 4;  // this corresponds to the record's header.
        std::cout << "message_length:" << 	message_length << std::endl;	
        std::cout << "nread:" << nread << std::endl;	
        std::cout << "setlen:" << setlen << std::endl;		
        if (setlen > (message_length - nread)) {
			int ii;

			for (ii = 0; ii < message_length; ii++)
				fprintf(stderr, "[%02x]", ((buffer)[ii] & 0xFF));
			fprintf(stderr, "\n");

			//std::string err2 = func + "set" + std::to_string(i+1) + ": message too short (" + std::to_string(setlen+nread) + ">" + std::to_string(offset)+ ")";
			goto end;
		}

		/** read rest of ipfix message
         */
        if ( (setid == IPFIX_SETID_TEMPLATE_NF9)
             || (setid == IPFIX_SETID_OPTTEMPLATE_NF9)
             || (setid == IPFIX_SETID_TEMPLATE)
             || (setid == IPFIX_SETID_OPTTEMPLATE) ) {
            /** parse a template set ( option or normal template ).
             */
            for (offset = nread, bytesleft = setlen; bytesleft > 4;) {

				// mlogf(4, "[%s] set%d: decode template, setlen=%d, left=%d\n",
				//		func, i + 1, setlen, bytesleft);

				mnslp_ipfix_decode_trecord(setid, buf + offset, bytesleft, &bytes);
				bytesleft -= bytes;
				offset += bytes;
	    }
            nread += setlen;
        }
        else if ( setid > 255 )
        {
	    /** get template
            */
            mnslp_ipfix_template *templ = get_template(setid);
            if ( templ == NULL ) {
                nread += setlen;
                err_flag = 1;
            }
            else {
                /** read data records
                 */
                for ( offset=nread, bytesleft=setlen; bytesleft>0; ) {
                    mnslp_ipfix_decode_datarecord( templ, buf+offset, bytesleft,
                                                  &bytes );
                    
                    bytesleft -= bytes;
                    offset    += bytes;
                    
                    std::cout << "bytes left" << bytesleft << std::endl;
                    
                }
                if ( bytesleft ) {
                    // mlogf( 3, "[%s] set%d: skip %d bytes padding\n",
                    //       func, i+1, bytesleft );
                }
                nread += setlen;
            }
        }
        else {
            // mlogf( 0, "[%s] set%d: invalid set id %d, set skipped!\n",
            //       func, i+1, setid );
	    nread += setlen;
        }
    } /* for (read sets */

    if ( err_flag )
        goto errend;

end:

	std::cout << "import finish ok" << std::endl;
    message->copy_raw_message(buffer, nread);
    
    // Establishes correct values for the current data set.
    if ( setid > 255 ){
		message->cs_tid = setid;
		message->cs_bytes = setlen;
		message->cs_offset = nread - setlen;
		message->cs_header = (u_int8_t *) message->buffer + message->cs_offset;
	}
    message->nrecords = data_list.size();
    
    /** The message does not require ouput.
     */
    require_output = false;
    
    return nread;

 errend:
	std::cout << "import not finish ok" << std::endl;
    data_list.clear();
    return -1;
	
}

bool 
mnslp_ipfix_message::isEqual (const mnslp_mspec_object &rhs) const
{
	const mnslp_ipfix_message* obj = dynamic_cast< const mnslp_ipfix_message* >( &rhs );
	
	if (obj != NULL)
	{
		if ((obj->message == NULL) and (message == NULL))
			return true;

		if ((obj->message != NULL) and (message == NULL))
			return false;

		if ((obj->message == NULL) and (message != NULL))
			return false;
		
		// compare message header information.
		if ((obj->message != NULL) and (message != NULL)){
			
			if (*(obj->message) != *message){
				std::cout << "Different header" << std::endl;
				return false;
			}							
			try
			{
				for (int i = 0; i < data_list.size(); i++){
					if ( data_list[i] != obj->data_list[i] ){
						std::cout << "Different data list" << std::endl;
						return false;
					}
				}
			}
			catch(const std::out_of_range& oor)
			{
				return false;
			}
		}
		return true;
	}
	else{
		return false;
	}			
}

bool
mnslp_ipfix_message::notEqual (const mnslp_mspec_object &rhs) const
{
	return !(isEqual(rhs));
}

/**
* Assignment operator. 
*/
mnslp_ipfix_message &
mnslp_ipfix_message::operator=(const mnslp_ipfix_message &other)
{
	
	if (other.message != NULL)
	   message = new ipfix_t(*(other.message));
	g_ipfix_fields = other.g_ipfix_fields;
	g_tstart = other.g_tstart;
	g_lasttid = other.g_lasttid;
	data_list = other.data_list;
	encode_network = other.encode_network;

	/** The message changed, so it requires a new output 
	*/
	require_output = true;
	
	return *this;
	
}

mnslp_ipfix_message::mnslp_ipfix_message(const mnslp_ipfix_message &other):
mnslp_mspec_object(OBJECT_TYPE, tr_mandatory, false),
message(NULL), g_tstart(0), encode_network(true), require_output(true)
{
	
	if (other.message != NULL)
	   message = new ipfix_t(*(other.message));
	g_ipfix_fields = other.g_ipfix_fields;
	g_tstart = other.g_tstart;
	g_lasttid = other.g_lasttid;
	data_list = other.data_list;
	encode_network = other.encode_network;
	require_output = other.require_output;
		
}

std::list<int> 
mnslp_ipfix_message::get_template_list(void) const
{
	if (message != NULL)
		return message->templates.get_template_list();
	else{
		std::list<int> empty;
		return empty;
	}
}

bool 
mnslp_ipfix_message::include_all_data_fields(mnslp_ipfix_template *templ) const
{
	// Iterate over the data records, if the data record belongs the template
	// given verifies that it has the same number of fields.
	// Return false if there is no data record for the template.
	
	std::vector<mnslp_ipfix_data_record>::const_iterator i;
	bool val_return = false;
		
	for ( i= data_list.begin(); i < data_list.end(); i++){
		if ((*i).get_template_id() == templ->get_template_id()){
			val_return = true;
			
			// Verify that the record data and template have the same
			// amount of fields.
			if ((*i).get_num_fields() != templ->get_numfields()){
				return false;
			}
				
			// Verify that both have the same field keys.
			try
			{
				for (int j = 0; j < templ->get_numfields(); j++ ){
					mnslp_ipfix_field field = templ->get_field(j).elem;
					mnslp_ipfix_value_field value = (*i).get_field(field.get_field_type().eno, 
														field.get_field_type().ftype);
				}
			}
			catch (msg::mnslp_ipfix_bad_argument &e)
			{
				return false;
			}
		}
	}
	
	return val_return;
}

std::list<std::string> 
mnslp_ipfix_message::get_field_data_values(mnslp_ipfix_template *templ, 
								const mnslp_ipfix_field &field) const
{
	
	std::list<std::string> list_return;
	std::vector<mnslp_ipfix_data_record>::const_iterator i;	
	for ( i= data_list.begin(); i != data_list.end(); i++)
	{
		if ((*i).get_template_id() == templ->get_template_id())
		{
			mnslp_ipfix_value_field value = (*i).get_field(field.get_field_type().eno, 
														   field.get_field_type().ftype);
			list_return.push_back(field.print_value(value));
		}
	}
	return list_return;
}	

  } // namespace msg
} // namespace mnslp
