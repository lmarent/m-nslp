/*
 * Test the session_id class.
 *
 * $Id: session_id.cpp 1711 2006-04-25 14:47:51Z stud-matfried $
 * $HeadURL: https://svn.ipv6.tm.uka.de/nsis/natfw-nslp/branches/20091014-autotools/test/session_id.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <ext/hash_map>
#include "protlib_types.h"
#include "session.h"

using namespace mnslp;


class SessionIdTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( SessionIdTest );

	CPPUNIT_TEST( testComparison );
	CPPUNIT_TEST( testAssignment );

	CPPUNIT_TEST_SUITE_END();

  public:
	void testComparison() {
		session_id id1 = session_id();
		session_id id2 = session_id();

		CPPUNIT_ASSERT( id1 == id1 );
		CPPUNIT_ASSERT( id1 == id1 );
		CPPUNIT_ASSERT( id1 != id2 );

		uint128 val = id1.get_id();
		session_id id3 = session_id(val);

		CPPUNIT_ASSERT( id1 == id3 );
		CPPUNIT_ASSERT( id3 == id1 );
	}

	void testAssignment() {
		session_id id1 = session_id();
		session_id id2 = id1;

		CPPUNIT_ASSERT( id1 == id2 );
		CPPUNIT_ASSERT( &id1 != &id2 );
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( SessionIdTest );
