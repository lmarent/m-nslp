/*
 * Test the nr_session_test class.
 *
 * $Id: nr_session.cpp 2133 2006-10-04 08:14:10Z stud-matfried $
 * $HeadURL: https://svn.ipv6.tm.uka.de/nsis/natfw-nslp/branches/20091014-autotools/test/nr_session.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "mri.h"	// from NTLP

#include "events.h"
#include "session.h"
#include "dispatcher.h"
#include "msg/selection_metering_entities.h"

#include "utils.h" // custom assertions

using namespace mnslp;
using namespace mnslp::msg;


class ResponderTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in nr_session.
 */
class nr_session_test : public nr_session {
  public:
	nr_session_test(state_t state=STATE_CLOSE, uint32 msn=0)
		: nr_session(state, msn) { }

	friend class ResponderTest;
};


class ResponderTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( ResponderTest );

	CPPUNIT_TEST( testClose );
	CPPUNIT_TEST( testMeteringPart );
	CPPUNIT_TEST( testMeteringForward );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();

	void testClose();
	void testMeteringPart();
	void testMeteringForward();

  private:
	static const uint32 START_MSN = 77;
	static const uint32 SESSION_LIFETIME = 30;

	void process(nr_session_test &s, event *evt);

	msg::ntlp_msg *create_mnslp_configure(uint32 msn=START_MSN,
		uint32 session_lifetime=SESSION_LIFETIME) const;
	
	msg::ntlp_msg *create_mnslp_refresh(uint32 msn, uint32 lt) const;

	mock_dispatcher *d;
	hostaddress destination;
};

CPPUNIT_TEST_SUITE_REGISTRATION( ResponderTest );


/*
 * Call the event handler and delete the event afterwards.
 */
void ResponderTest::process(nr_session_test &s, event *evt) {
	d->clear();
	s.process(d, evt);
	delete evt;
}

void ResponderTest::setUp() {
	d = new mock_dispatcher();
}

void ResponderTest::tearDown() {
	delete d;
}


msg::ntlp_msg *ResponderTest::create_mnslp_configure(uint32 msn, uint32 lt) const {

	msg::mnslp_configure *configure = new mnslp_configure();
	configure->set_msg_sequence_number(msn);
	configure->set_session_lifetime(lt);
	configure->set_selection_metering_entities(selection_metering_entities::sme_any);
	
	ntlp::mri *ntlp_mri = new ntlp::mri_pathcoupled(
		hostaddress("192.168.0.4"), 32, 0,
		hostaddress("192.168.0.5"), 32, 0,
		"tcp", 0, 0, 0, false
	);

	return new msg::ntlp_msg(session_id(), configure, ntlp_mri, 0);
}


msg::ntlp_msg *ResponderTest::create_mnslp_refresh(uint32 msn, uint32 lt) const {

	msg::mnslp_refresh *refresh = new mnslp_refresh();
	refresh->set_session_lifetime(lt);
	refresh->set_msg_sequence_number(msn);

	msg::ntlp_msg *msg = new msg::ntlp_msg(
		session_id(), refresh,
		new ntlp::mri_pathcoupled(), 0
	);

	return msg;
}


void ResponderTest::testClose() {
	/*
	 * CLOSE ---[rx_CONFIGURE && CHECK_AA && CONFIGURE(Lifetime>0) ]---> METERING
	 */
	nr_session_test s1(nr_session::STATE_CLOSE);
	event *e1 = new msg_event(NULL, create_mnslp_configure(), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_METERING_PART);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	/*
	 * CLOSE ---[rx_CONFIGURE && CONFIGURE(Lifetime > MAX) ]---> METERING
	 */
	nr_session_test s2(nr_session::STATE_CLOSE);
	event *e2 = new msg_event(NULL,
		create_mnslp_configure(START_MSN+1, 100000), true);
	
	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_METERING_PART);
	ASSERT_RESPONSE_MESSAGE_SENT(d,
		information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());
}


void ResponderTest::testMeteringPart() {
	/*
	 * STATE_METERING_PART ---[rx_REFRESH && CHECK_AA && REFRESH(Lifetime>0) ]---> STATE_METERING_PART
	 */
	nr_session_test s1(nr_session::STATE_METERING_PART, START_MSN);
	event *e1 = new msg_event(NULL, create_mnslp_refresh(START_MSN+1, 10), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_METERING_PART);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());


	/*
	 * STATE_METERING_PART ---[rx_REFRESH && REFRESH(Lifetime > MAX) ]---> STATE_METERING_PART
	 */
	nr_session_test s2(nr_session::STATE_METERING_PART, START_MSN);
	event *e2 = new msg_event(NULL,
		create_mnslp_refresh(START_MSN+1, 10), true);

	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_METERING_PART);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());


	/*
	 * STATE_METERING_PART ---[rx_REFRESH && REFRESH(Lifetime == 0) ]--->STATE_CLOSE
	 */
	nr_session_test s3(nr_session::STATE_METERING_PART, START_MSN);
	event *e3 = new msg_event(NULL,
			create_mnslp_refresh(START_MSN+1, 0), true);

	process(s3, e3);
	ASSERT_STATE(s3, nr_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING_PART ---[rx_REFRESH && MSN too low ]---> STATE_METERING_PART
	 */
	nr_session_test s4(nr_session::STATE_METERING_PART, START_MSN);
	event *e4 = new msg_event(NULL, create_mnslp_refresh(START_MSN, 10), true);

	process(s4, e4);
	ASSERT_STATE(s4, nr_session::STATE_METERING_PART);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
}


void ResponderTest::testMeteringForward() {
	/*
	 * STATE_METERING_FORW ---[rx_REFRESH && CHECK_AA && REFRESH(Lifetime>0) ]---> STATE_METERING_FORW
	 */
	nr_session_test s1(nr_session::STATE_METERING_FORW, START_MSN);
	event *e1 = new msg_event(NULL, create_mnslp_refresh(START_MSN+1, 10), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_METERING_FORW);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());


	/*
	 * STATE_METERING_FORW ---[rx_REFRESH && REFRESH(Lifetime > MAX) ]---> STATE_METERING_FORW
	 */
	nr_session_test s2(nr_session::STATE_METERING_FORW, START_MSN);
	event *e2 = new msg_event(NULL,
		create_mnslp_refresh(START_MSN+1, 10), true);

	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_METERING_FORW);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());


	/*
	 * STATE_METERING_FORW ---[rx_REFRESH && REFRESH(Lifetime == 0) ]--->STATE_CLOSE
	 */
	nr_session_test s3(nr_session::STATE_METERING_FORW, START_MSN);
	event *e3 = new msg_event(NULL,
			create_mnslp_refresh(START_MSN+1, 0), true);

	process(s3, e3);
	ASSERT_STATE(s3, nr_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING_FORW ---[rx_REFRESH && MSN too low ]---> STATE_METERING_FORW
	 */
	nr_session_test s4(nr_session::STATE_METERING_FORW, START_MSN);
	event *e4 = new msg_event(NULL, create_mnslp_refresh(START_MSN, 10), true);

	process(s4, e4);
	ASSERT_STATE(s4, nr_session::STATE_METERING_FORW);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
}

// EOF
