/*
 * Test the dispatcher class.
 *
 * $Id: dispatcher.cpp 1936 2006-07-25 15:13:45Z stud-matfried $
 * $HeadURL: https://svn.ipv6.tm.uka.de/nsis/natfw-nslp/branches/20091014-autotools/test/dispatcher.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "address.h"

#include "dispatcher.h"
#include "events.h"

#include "utils.h"

using namespace mnslp;
using namespace protlib;


class DispatcherTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( DispatcherTest );

	CPPUNIT_TEST( testBasic );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void testBasic();
	void tearDown();

	mock_mnslp_config *conf;
	session_manager *mgr;
	mock_dispatcher *d;
};

CPPUNIT_TEST_SUITE_REGISTRATION( DispatcherTest );

void DispatcherTest::setUp() {
	conf = new mock_mnslp_config();
	mgr = new session_manager(conf);
	d = new mock_dispatcher(mgr, nat_mgr, NULL, conf);
}

void DispatcherTest::tearDown() {
	delete d;
	delete mgr;
	delete conf;
}

void DispatcherTest::testBasic() {
	event *e = new api_create_event(hostaddress("141.3.70.4"),
									hostaddress("141.3.70.5"), 1234, 4321, 6,
									std::list<uint8>(), 40);
									
	CPPUNIT_ASSERT( e->get_session_id() == NULL );

	d->process(e);
	
	ASSERT_CREATE_MESSAGE_SENT(d);
}

// EOF
