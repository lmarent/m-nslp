/*
 * Test the mnslp_notify class.
 *
 * $Id: mnslp_notify.cpp 1730 2014-11-05 12:57:33Z  $
 * $HeadURL: https://./test/mnslp_notify.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "network_message.h"

#include "msg/mnslp_options.h"
#include "msg/mnslp_object.h"

#include "msg/mnslp_ie.h"
#include "msg/mnslp_msg.h"
#include "msg/mnslp_ipfix_field.h"
#include "msg/mnslp_ipfix_data_record.h"
#include "msg/information_code.h"
#include "msg/msg_sequence_number.h"



using namespace mnslp::msg;


class MnslpOptionsTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( MnslpOptionsTest );

	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testComparison );
	CPPUNIT_TEST( testCopying );
	CPPUNIT_TEST( testManager );

	CPPUNIT_TEST_SUITE_END();

  public:
	void testBasics();
	void testComparison();
	void testCopying();
	void testManager();
};

CPPUNIT_TEST_SUITE_REGISTRATION( MnslpOptionsTest );


void MnslpOptionsTest::testBasics() {
	mnslp_options c1;
	uint32 numTmp = 4;

	CPPUNIT_ASSERT( c1.get_msg_sequence_number() == 0 );
	c1.set_msg_sequence_number(numTmp);
	CPPUNIT_ASSERT( c1.get_msg_sequence_number() == 4 );


}


void MnslpOptionsTest::testComparison() {
	
	uint32 numTmp = 4;
	
	mnslp_options *c1 = new mnslp_options();
	mnslp_options *c2 = new mnslp_options();
	c2->set_msg_sequence_number(numTmp);

	CPPUNIT_ASSERT( *c1 == *c1 );
	CPPUNIT_ASSERT( *c2 == *c2 );
	CPPUNIT_ASSERT( *c1 != *c2 );
	CPPUNIT_ASSERT( *c2 != *c1 );

	c1->set_msg_sequence_number(numTmp);
	
	CPPUNIT_ASSERT( *c1 == *c2 );
	CPPUNIT_ASSERT( *c2 == *c1 );

	delete c2;
	delete c1;
}

void MnslpOptionsTest::testCopying() {
	uint32 numTmp = 4;
	
	mnslp_options *c1 = new mnslp_options();
	c1->set_msg_sequence_number(numTmp);

	mnslp_options *c2 = c1->copy();
	CPPUNIT_ASSERT( c1 != c2 );
	CPPUNIT_ASSERT( *c1 == *c2 );

	delete c2;
	delete c1;
}


void MnslpOptionsTest::testManager() {

	uint32 numTmp = 4;
	MNSLP_IEManager::clear();
	MNSLP_IEManager::register_known_ies();
	MNSLP_IEManager *mgr = MNSLP_IEManager::instance();

    /* 
     * Builds the notify message
     */
	mnslp_options *m1 = new mnslp_options();
	m1->set_msg_sequence_number(numTmp);
       
	NetMsg msg( m1->get_serialized_size(IE::protocol_v1) );
	uint32 bytes_written;
	m1->serialize(msg, IE::protocol_v1, bytes_written);

	/*
	 * Now read the serialized object.
	 */
		
	msg.set_pos(0);
	IEErrorList errlist;
	uint32 num_read;

	IE *ie = mgr->deserialize(msg, cat_mnslp_msg, IE::protocol_v1, errlist,
			num_read, false);
			
	CPPUNIT_ASSERT( ie != NULL );
	CPPUNIT_ASSERT( errlist.is_empty() );
	CPPUNIT_ASSERT( num_read == ie->get_serialized_size(IE::protocol_v1) );
	CPPUNIT_ASSERT( ie != NULL );
    CPPUNIT_ASSERT( *m1 == *ie );
    
    mgr->clear();
        
}

// EOF
