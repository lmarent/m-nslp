/*
 * Test the ni_session_test class.
 *
 * $Id: ni_session.cpp 1936 2015-01-06 8:55:00Z amarentes $
 * $HeadURL: https://./test/ni_session.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "events.h"
#include "session.h"
#include "dispatcher.h"
#include <vector>
#include "msg/information_code.h"
#include <iostream>

#include "utils.h" // custom assertions

using namespace mnslp;
using namespace mnslp::msg;


class InitiatorTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in ni_session.
 */
class ni_session_test : public ni_session {
  public:
	ni_session_test(state_t state=STATE_CLOSE) : ni_session(state) { }

	friend class InitiatorTest;
};


class InitiatorTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( InitiatorTest );

	CPPUNIT_TEST( testClose );
	CPPUNIT_TEST( testPendindForward );
	CPPUNIT_TEST( testPendingParticipation );
	CPPUNIT_TEST( testMeteringForward );
	CPPUNIT_TEST( testMeteringParticipation );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();

	void testClose();
	void testPendindForward();
	void testPendingParticipation();
	void testMeteringForward();
	void testMeteringParticipation();

  private:
	static const uint32 START_MSN = 77;

	void process(ni_session_test &s, event *evt);

	msg::ntlp_msg *create_mnslp_configure() const;

	msg::ntlp_msg *create_mnslp_response(
		uint8 severity, uint8 response_code, uint16 msg_type,
		uint32 msn=START_MSN) const;

	msg::ntlp_msg *create_mnslp_refresh() const;

	msg::mnslp_ipfix_message * create_mnslp_ipfix_message() const;

	mock_mnslp_config *conf;
	mock_dispatcher *d;
	hostaddress source;
	hostaddress destination;
};

CPPUNIT_TEST_SUITE_REGISTRATION( InitiatorTest );


/*
 * Call the event handler and delete the event afterwards.
 */
void InitiatorTest::process(ni_session_test &s, event *evt) {
	d->clear();	
	s.process(d, evt);
	delete evt;
}

void InitiatorTest::setUp() {
	conf = new mock_mnslp_config();
	d = new mock_dispatcher(NULL, NULL, conf);
	source = hostaddress("172.24.98.103");
	destination = hostaddress("172.24.98.104");
}

void InitiatorTest::tearDown() {
	delete d;
	delete conf;
}


msg::ntlp_msg *InitiatorTest::create_mnslp_configure() const {

	msg::mnslp_configure *configure = new mnslp_configure();
	configure->set_msg_sequence_number(START_MSN);

	msg::ntlp_msg *msg = new msg::ntlp_msg(
		session_id(), configure,
		new ntlp::mri_pathcoupled(), 0
	);

	return msg;
}


msg::ntlp_msg *InitiatorTest::create_mnslp_response(uint8 severity,
		uint8 response_code, uint16 msg_type, uint32 msn) const {

	mnslp_response *resp = new mnslp_response();
	resp->set_information_code(severity, response_code, 
							   information_code::obj_none);
	resp->set_msg_sequence_number(msn);

	ntlp::mri *ntlp_mri = new ntlp::mri_pathcoupled(
		hostaddress("192.168.0.4"), 32, 0,
		hostaddress("192.168.0.5"), 32, 0,
		"tcp", 0, 0, 0, true
	);

	return new msg::ntlp_msg(session_id(), resp, ntlp_mri, 0);
}

msg::ntlp_msg *InitiatorTest::create_mnslp_refresh() const {

	msg::mnslp_refresh *refresh = new mnslp_refresh();
	refresh->set_session_lifetime(10);
	refresh->set_msg_sequence_number(START_MSN);

	msg::ntlp_msg *msg = new msg::ntlp_msg(
		session_id(), refresh,
		new ntlp::mri_pathcoupled(), 0
	);

	return msg;
}


msg::mnslp_ipfix_message * InitiatorTest::create_mnslp_ipfix_message() const
{

    /* 
     * Builds the ipfix message
     */
	int sourceid = 12345;
	mnslp_ipfix_message *mess;
	uint16_t templatedataid = 0;
	uint64_t octdel = 100;
	uint64_t octdel2 = 200;
	uint64_t packdel = 300;
	uint64_t packdel2 = 400;
	
	int nfields = 3;
	uint8_t   buf[5]  = { 1, 2, 3, 4 };
	uint8_t   buf2[5]  = { 5, 6, 7, 8 };

    mess = new mnslp_ipfix_message(sourceid, IPFIX_VERSION);	
	mess->delete_all_templates();

	templatedataid = mess->new_data_template( nfields );
	mess->add_field(templatedataid, 0, IPFIX_FT_SOURCEIPV4ADDRESS, 4);
	mess->add_field(templatedataid, 0, IPFIX_FT_OCTETDELTACOUNT, 8);
	mess->add_field(templatedataid, 0, IPFIX_FT_PACKETDELTACOUNT, 8);

	mnslp_ipfix_field field1 = mess->get_field_definition( 0, IPFIX_FT_SOURCEIPV4ADDRESS );
	mnslp_ipfix_value_field fvalue1 = field1.get_ipfix_value_field( (uint8_t *) buf, 4);
	mnslp_ipfix_value_field fvalue1a = field1.get_ipfix_value_field( (uint8_t *) buf2, 4);
	
	mnslp_ipfix_field field2 = mess->get_field_definition( 0, IPFIX_FT_OCTETDELTACOUNT );
	mnslp_ipfix_value_field fvalue2 = field2.get_ipfix_value_field( octdel);
	mnslp_ipfix_value_field fvalue2a = field2.get_ipfix_value_field( octdel2);

	mnslp_ipfix_field field3 = mess->get_field_definition( 0, IPFIX_FT_PACKETDELTACOUNT );
	mnslp_ipfix_value_field fvalue3 = field3.get_ipfix_value_field( packdel );
	mnslp_ipfix_value_field fvalue3a = field3.get_ipfix_value_field( packdel2 );
	
	mnslp_ipfix_data_record data(templatedataid);
	data.insert_field(0, IPFIX_FT_SOURCEIPV4ADDRESS, fvalue1);
	data.insert_field(0, IPFIX_FT_OCTETDELTACOUNT, fvalue2);
	data.insert_field(0, IPFIX_FT_PACKETDELTACOUNT, fvalue3);
	mess->include_data(templatedataid, data);

	mnslp_ipfix_data_record data2(templatedataid);
	data2.insert_field(0, IPFIX_FT_SOURCEIPV4ADDRESS, fvalue1a);
	data2.insert_field(0, IPFIX_FT_OCTETDELTACOUNT, fvalue2a);
	data2.insert_field(0, IPFIX_FT_PACKETDELTACOUNT, fvalue3a);
	mess->include_data(templatedataid, data2);
	mess->output(templatedataid);

	return mess;

}

void InitiatorTest::testClose() {
	/*
	 * CLOSE ---[tg_CONF]---> PENDING
	 */
	ni_session_test s1(ni_session::STATE_CLOSE);
	vector<msg::mnslp_mspec_object> mspec_objects;
	msg::mnslp_ipfix_message *ipfix_msg = create_mnslp_ipfix_message();
	mspec_objects.push_back(*ipfix_msg);
	
	event *e1 = new api_configure_event(source, destination);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_PENDING_PART);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_response_timer());
}


void InitiatorTest::testPendingParticipation() {
	/*
	 * STATE_PENDING_PART ---[rx_RESPONSE(SUCCESS,CONFIGURE)]---> STATE_METERING_PART
	 */
	ni_session_test s1(ni_session::STATE_PENDING_PART);
	s1.set_last_configure_message(create_mnslp_configure());

	ntlp_msg *resp1 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	event *e1 = new msg_event(NULL, resp1);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_METERING_PART);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());


	/*
	 * STATE_PENDING_PART ---[rx_RESPONSE(ERROR)]---> CLOSE
	 */
	ni_session_test s2(ni_session::STATE_PENDING_PART);
	s2.set_last_configure_message(create_mnslp_configure());

	ntlp_msg *resp2 = create_mnslp_response(
		information_code::sc_permanent_failure, 0,
		information_code::obj_msnlp_ipfix_message);

	event *e2 = new msg_event(NULL, resp2);

	process(s2, e2);
	ASSERT_STATE(s2, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_PENDING_PART ---[RESPONSE_TIMEOUT, retry]---> STATE_PENDING_PART
	 */
	ni_session_test s3(ni_session::STATE_PENDING_PART);
	// fake a previously sent Configure message
	s3.set_last_configure_message(create_mnslp_configure());
	s3.get_response_timer().set_id(47);

	timer_event *e3 = new timer_event(NULL, 47);

	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_PENDING_PART);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());


	/*
	 * STATE_PENDING_PART ---[RESPONSE_TIMEOUT, no retry]---> CLOSE
	 */
	ni_session_test s4(ni_session::STATE_PENDING_PART);
	// fake a previously sent Configure message
	s4.set_last_configure_message(create_mnslp_configure());	
	s4.set_configure_counter(1000);
	s4.get_response_timer().set_id(47);
	timer_event *e4 = new timer_event(NULL, 47);

	process(s4, e4);
	ASSERT_STATE(s4, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_PENDING_PART ---[tg_TEARDOWN]---> CLOSE
	 */
	ni_session_test s5(ni_session::STATE_PENDING_PART);
	s5.set_last_configure_message(create_mnslp_configure());
	event *e5 = new api_teardown_event(NULL);

	process(s5, e5);
	ASSERT_STATE(s5, ni_session::STATE_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_PENDING_PART ---[tg_TEARDOWN]---> CLOSE
	 */
	ni_session_test s6(ni_session::STATE_PENDING_PART);
	s6.set_last_configure_message(create_mnslp_configure());
	event *e6 = new no_next_node_found_event(NULL);

	process(s6, e6);
	ASSERT_STATE(s6, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
}

void InitiatorTest::testPendindForward() {
	/*
	 * STATE_PENDING_FORW ---[rx_RESPONSE(SUCCESS,CONFIGURE)]---> STATE_METERING_PART
	 */
	ni_session_test s1(ni_session::STATE_PENDING_FORW);
	s1.set_last_configure_message(create_mnslp_configure());

	ntlp_msg *resp1 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	event *e1 = new msg_event(NULL, resp1);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_METERING_FORW);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());

	/*
	 * STATE_PENDING_FORW ---[rx_RESPONSE(ERROR)]---> CLOSE
	 */
	ni_session_test s2(ni_session::STATE_PENDING_FORW);
	s2.set_last_configure_message(create_mnslp_configure());

	ntlp_msg *resp2 = create_mnslp_response(
		information_code::sc_permanent_failure, 0,
		information_code::obj_msnlp_ipfix_message);

	event *e2 = new msg_event(NULL, resp2);

	process(s2, e2);
	ASSERT_STATE(s2, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_PENDING_FORW ---[RESPONSE_TIMEOUT, retry]---> STATE_PENDING_FORW
	 */
	ni_session_test s3(ni_session::STATE_PENDING_FORW);
	// fake a previously sent Configure message
	s3.set_last_configure_message(create_mnslp_configure());
	s3.get_response_timer().set_id(47);

	timer_event *e3 = new timer_event(NULL, 47);

	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_PENDING_FORW);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());

	/*
	 * STATE_PENDING_FORW ---[RESPONSE_TIMEOUT, no retry]---> CLOSE
	 */
	ni_session_test s4(ni_session::STATE_PENDING_FORW);
	// fake a previously sent Configure message
	s4.set_last_configure_message(create_mnslp_configure());
	s4.set_configure_counter(1000);
	s4.get_response_timer().set_id(47);
	timer_event *e4 = new timer_event(NULL, 47);

	process(s4, e4);
	ASSERT_STATE(s4, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_PENDING_FORW ---[tg_TEARDOWN]---> CLOSE
	 */
	ni_session_test s5(ni_session::STATE_PENDING_FORW);
	s5.set_last_configure_message(create_mnslp_configure());
	event *e5 = new api_teardown_event(NULL);

	process(s5, e5);
	ASSERT_STATE(s5, ni_session::STATE_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_PENDING_FORW ---[tg_TEARDOWN]---> CLOSE
	 */
	ni_session_test s6(ni_session::STATE_PENDING_FORW);
	s6.set_last_configure_message(create_mnslp_configure());
	event *e6 = new no_next_node_found_event(NULL);

	process(s6, e6);
	ASSERT_STATE(s6, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
}

void InitiatorTest::testMeteringForward() {
	/*
	 * STATE_METERING_FORW ---[rx_RESPONSE(SUCCESS,REFRESH)]---> STATE_METERING_FORW
	 */
	ni_session_test s1(ni_session::STATE_METERING_FORW);
	s1.set_last_refresh_message(create_mnslp_refresh());

	ntlp_msg *resp1 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	event *e1 = new msg_event(NULL, resp1);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_METERING_FORW);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());


	/*
	 * STATE_METERING_FORW ---[rx_RESPONSE(ERROR,REFRESH)]---> CLOSE
	 */
	ni_session_test s2(ni_session::STATE_METERING_FORW);
	s2.set_last_refresh_message(create_mnslp_refresh());

	ntlp_msg *resp2 = create_mnslp_response(
		information_code::sc_permanent_failure, 0,
		information_code::obj_msnlp_ipfix_message);

	event *e2 = new msg_event(NULL, resp2);

	process(s2, e2);
	ASSERT_STATE(s2, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING_FORW ---[RESPONSE_TIMEOUT, retry]---> STATE_METERING_FORW
	 */
	ni_session_test s3(ni_session::STATE_METERING_FORW);
	s3.set_last_refresh_message(create_mnslp_refresh());
	s3.get_response_timer().set_id(47);

	timer_event *e3 = new timer_event(NULL, 47);

	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_METERING_FORW);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());


	/*
	 * STATE_METERING_FORW ---[RESPONSE_TIMEOUT, no retry]---> STATE_METERING_FORW
	 */
	ni_session_test s4(ni_session::STATE_METERING_FORW);
	s4.set_last_refresh_message(create_mnslp_refresh());
	s4.set_refresh_counter(1000);
	s4.get_response_timer().set_id(47);
	timer_event *e4 = new timer_event(NULL, 47);

	process(s4, e4);
	ASSERT_STATE(s4, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING_FORW ---[tg_TEARDOWN]---> STATE_CLOSE
	 */
	ni_session_test s5(ni_session::STATE_METERING_FORW);
	s5.set_last_refresh_message(create_mnslp_refresh());
	event *e5 = new api_teardown_event(NULL);

	process(s5, e5);
	ASSERT_STATE(s5, ni_session::STATE_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING_FORW ---[REFRESH_TIMEOUT]---> STATE_METERING_FORW
	 */
	ni_session_test s6(ni_session::STATE_METERING_FORW);
	s6.set_last_refresh_message(create_mnslp_refresh());
	s6.get_refresh_timer().set_id(0xABCD);
	event *e6 = new timer_event(NULL, 0xABCD);

	process(s6, e6);
	ASSERT_STATE(s6, ni_session::STATE_METERING_FORW);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s6.get_response_timer());
}

void InitiatorTest::testMeteringParticipation() {
	/*
	 * STATE_METERING_PART ---[rx_RESPONSE(SUCCESS,REFRESH)]---> STATE_METERING_PART
	 */
	ni_session_test s1(ni_session::STATE_METERING_PART);
	s1.set_last_refresh_message(create_mnslp_refresh());

	ntlp_msg *resp1 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	event *e1 = new msg_event(NULL, resp1);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_METERING_PART);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());


	/*
	 * STATE_METERING_PART ---[rx_RESPONSE(ERROR,REFRESH)]---> CLOSE
	 */
	ni_session_test s2(ni_session::STATE_METERING_PART);
	s2.set_last_refresh_message(create_mnslp_refresh());

	ntlp_msg *resp2 = create_mnslp_response(
		information_code::sc_permanent_failure, 0,
		information_code::obj_msnlp_ipfix_message);

	event *e2 = new msg_event(NULL, resp2);

	process(s2, e2);
	ASSERT_STATE(s2, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING_PART ---[RESPONSE_TIMEOUT, retry]---> STATE_METERING_PART
	 */
	ni_session_test s3(ni_session::STATE_METERING_PART);
	s3.set_last_refresh_message(create_mnslp_refresh());
	s3.get_response_timer().set_id(47);

	timer_event *e3 = new timer_event(NULL, 47);

	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_METERING_PART);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());


	/*
	 * STATE_METERING_PART ---[RESPONSE_TIMEOUT, no retry]---> STATE_METERING_PART
	 */
	ni_session_test s4(ni_session::STATE_METERING_PART);
	s4.set_last_refresh_message(create_mnslp_refresh());	
	s4.set_refresh_counter(1000);
	s4.get_response_timer().set_id(47);
	timer_event *e4 = new timer_event(NULL, 47);

	process(s4, e4);
	ASSERT_STATE(s4, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING_PART ---[tg_TEARDOWN]---> STATE_CLOSE
	 */
	ni_session_test s5(ni_session::STATE_METERING_PART);
	s5.set_last_refresh_message(create_mnslp_refresh());
	event *e5 = new api_teardown_event(NULL);

	process(s5, e5);
	ASSERT_STATE(s5, ni_session::STATE_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING_PART ---[REFRESH_TIMEOUT]---> STATE_METERING_PART
	 */
	ni_session_test s6(ni_session::STATE_METERING_PART);
	s6.set_last_refresh_message(create_mnslp_refresh());
	s6.get_refresh_timer().set_id(0xABCD);
	event *e6 = new timer_event(NULL, 0xABCD);

	process(s6, e6);
	ASSERT_STATE(s6, ni_session::STATE_METERING_PART);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s6.get_response_timer());
}

// EOF
