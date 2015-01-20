/*
 * Test the mnslp_notify class.
 *
 * $Id: mnslp_notify.cpp 1730 2014-11-05 12:57:33Z  $
 * $HeadURL: https://./test/mnslp_notify.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "network_message.h"

#include "msg/mnslp_notify.h"
#include "msg/mnslp_object.h"

#include "msg/mnslp_ie.h"
#include "msg/mnslp_msg.h"
#include "msg/mnslp_ipfix_message.h"
#include "msg/mnslp_ipfix_field.h"
#include "msg/mnslp_ipfix_data_record.h"
#include "msg/information_code.h"




using namespace mnslp::msg;


class MnslpNotifyTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( MnslpNotifyTest );

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

CPPUNIT_TEST_SUITE_REGISTRATION( MnslpNotifyTest );


void MnslpNotifyTest::testBasics() {
	mnslp_notify c1;

	c1.set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);
	CPPUNIT_ASSERT( c1.get_severity_class() == information_code::sc_success );
	CPPUNIT_ASSERT( c1.get_response_code() == information_code::suc_successfully_processed );
	CPPUNIT_ASSERT( c1.get_response_object_type() == information_code::obj_none );


}


void MnslpNotifyTest::testComparison() {
	mnslp_notify *c1 = new mnslp_notify();
	mnslp_notify *c2 = new mnslp_notify();
	c2->set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);

	CPPUNIT_ASSERT( *c1 == *c1 );
	CPPUNIT_ASSERT( *c2 == *c2 );
	CPPUNIT_ASSERT( *c1 != *c2 );
	CPPUNIT_ASSERT( *c2 != *c1 );

	c1->set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);
	CPPUNIT_ASSERT( *c1 == *c2 );
	CPPUNIT_ASSERT( *c2 == *c1 );

	delete c2;
	delete c1;
}

void MnslpNotifyTest::testCopying() {
	mnslp_notify *c1 = new mnslp_notify();

	mnslp_notify *c2 = c1->copy();
	CPPUNIT_ASSERT( c1 != c2 );
	CPPUNIT_ASSERT( *c1 == *c2 );

	delete c2;
	delete c1;
}


void MnslpNotifyTest::testManager() {

	MNSLP_IEManager::clear();

	MNSLP_IEManager::register_known_ies();

	MNSLP_IEManager *mgr = MNSLP_IEManager::instance();

    /* 
     * Builds the notify message
     */
	mnslp_notify *m1 = new mnslp_notify();
	m1->set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);
       
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
