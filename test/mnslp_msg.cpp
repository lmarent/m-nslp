/*
 * mnslp_msg.cpp - Test the natfw_msg class.
 *
 * $Id: mnslp_msg.cpp 2438 2014-11-15 15:07:34Z stud-matfried $
 * $HeadURL: https://./test/mnslp_msg.cpp $
 *
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "network_message.h"

#include "msg/mnslp_ie.h"
#include "msg/mnslp_msg.h"
#include "msg/mnslp_object.h"

using namespace mnslp::msg;


class MnslpMsgTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( MnslpMsgTest );

	CPPUNIT_TEST( testComparison );
	CPPUNIT_TEST( testCopying );


	CPPUNIT_TEST_SUITE_END();

  public:
	void testComparison();
	void testCopying();
};

CPPUNIT_TEST_SUITE_REGISTRATION( MnslpMsgTest );


void MnslpMsgTest::testComparison() {
	mnslp_refresh *m1 = new mnslp_refresh();
	mnslp_refresh *m2 = new mnslp_refresh();

	CPPUNIT_ASSERT( *m1 == *m1 );
	CPPUNIT_ASSERT( *m2 == *m2 );
	CPPUNIT_ASSERT( *m1 == *m2 );
	CPPUNIT_ASSERT( *m2 == *m1 );
   
	m1->set_session_lifetime(10);	
	CPPUNIT_ASSERT( *m1 != *m2 );

	m2->set_session_lifetime(10);
	CPPUNIT_ASSERT( *m1 == *m2 );

	delete m2;
	delete m1;
}


void MnslpMsgTest::testCopying() {
	mnslp_configure *m1 = new mnslp_configure();

	mnslp_configure *m2 = m1->copy();
	CPPUNIT_ASSERT( m1 != m2 );
	CPPUNIT_ASSERT( *m1 == *m2 );

	delete m2;
	delete m1;
}




// EOF
