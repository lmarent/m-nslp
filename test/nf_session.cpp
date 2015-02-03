/*
 * Test the nf_session class.
 *
 * $Id: nf_session.cpp 1936 2015-01-08 15:35:00Z amarentes $
 * $HeadURL: https://./test/nf_session.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "events.h"
#include "session.h"
#include "dispatcher.h"
#include "policy_rule.h"
#include "msg/selection_metering_entities.h"

#include "utils.h" // custom assertions

using namespace mnslp;
using namespace mnslp::msg;

class ForwarderTest;

/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in nf_session.
 */
class nf_session_test : public nf_session {
  public:
	nf_session_test(state_t state, mnslp_config *conf)
		: nf_session(state, conf) { }

	friend class ForwarderTest;
};


class ForwarderTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( ForwarderTest );

	CPPUNIT_TEST( testClose );
	CPPUNIT_TEST( testPending );
	CPPUNIT_TEST( testMetering );
	CPPUNIT_TEST( testIntegratedStateMachine );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();

	void add_export_fields1();
	void add_export_fields2();
	void add_export_fields3();
	void add_configuration_fields(msg::mnslp_ipfix_message *mess);	

	void testClose();
	void testPending();
	void testMetering();
	void testIntegratedStateMachine();

  private:
	void process(nf_session_test &s, event *evt);

	static const uint32 START_MSN = 77;
	static const uint32 SESSION_LIFETIME = 30;

	msg::ntlp_msg *create_mnslp_configure(uint32 msn=START_MSN,
				uint32 session_lifetime=SESSION_LIFETIME) const;

	msg::ntlp_msg *create_mnslp_response(
		uint8 severity, uint8 response_code, uint16 msg_type,
		uint32 msn=START_MSN) const;

	msg::ntlp_msg *create_mnslp_refresh(
		uint32 msn, uint32 lt) const;
		
	mock_mnslp_config *conf;
	policy_rule_installer *rule_installer;
	mock_dispatcher *d;
	hostaddress destination;
	
	msg::mnslp_ipfix_message *mess1;
	msg::mnslp_ipfix_message *mess2;
	msg::mnslp_ipfix_message *mess3;
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( ForwarderTest );


void ForwarderTest::add_export_fields1() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess1->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess1->add_field(templatedataid, 0, IPFIX_FT_OCTETDELTACOUNT, 8);
	
}

void ForwarderTest::add_export_fields2() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess2->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess2->add_field(templatedataid, 0, IPFIX_FT_PACKETDELTACOUNT, 8);	
}

void ForwarderTest::add_export_fields3() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess3->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess3->add_field(templatedataid, 0, IPFIX_FT_MINIMUMIPTOTALLENGTH, 8);	
}

void ForwarderTest::add_configuration_fields(msg::mnslp_ipfix_message * mess) 
{

	uint16_t templateoptionid = 0;
	int nfields = 7;
	uint8_t buf[5]  = { 10, 0, 2, 15 };
	uint16_t destination_port = 80;
	uint8_t protocol = 8;
	uint64_t flow_indicator = 0; 
	uint8_t ipfix_ver = IPFIX_VERSION;
	
	// Add filter fields
	templateoptionid = mess->new_option_template( nfields );
	mess->add_field(templateoptionid, 0, IPFIX_FT_FLOWKEYINDICATOR, 8);
	mess->add_field(templateoptionid, 0, IPFIX_FT_COLLECTORIPV4ADDRESS, 4);
	mess->add_field(templateoptionid, 0, IPFIX_FT_COLLECTORPROTOCOLVERSION, 1);	
	mess->add_field(templateoptionid, 0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL, 1);
	mess->add_field(templateoptionid, 0, IPFIX_FT_SOURCEIPV4ADDRESS, 4);
	mess->add_field(templateoptionid, 0, IPFIX_FT_TCPDESTINATIONPORT, 2);
	mess->add_field(templateoptionid, 0, IPFIX_FT_PROTOCOLIDENTIFIER, 1);

	msg::mnslp_ipfix_field field1 = mess->get_field_definition( 0, IPFIX_FT_FLOWKEYINDICATOR );
	msg::mnslp_ipfix_value_field fvalue1 = field1.get_ipfix_value_field(flow_indicator);

	msg::mnslp_ipfix_field field2 = mess->get_field_definition( 0, IPFIX_FT_COLLECTORIPV4ADDRESS );
	msg::mnslp_ipfix_value_field fvalue2 = field2.get_ipfix_value_field( (uint8_t *) buf, 4);

	msg::mnslp_ipfix_field field3 = mess->get_field_definition( 0, IPFIX_FT_COLLECTORPROTOCOLVERSION );
	msg::mnslp_ipfix_value_field fvalue3 = field3.get_ipfix_value_field(ipfix_ver);
	
	msg::mnslp_ipfix_field field4 = mess->get_field_definition( 0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL );
	msg::mnslp_ipfix_value_field fvalue4 = field4.get_ipfix_value_field( protocol);

	// In this case just the last three fields are used for filter purposes.
	
	msg::mnslp_ipfix_field field5 = mess->get_field_definition( 0, IPFIX_FT_SOURCEIPV4ADDRESS );
	msg::mnslp_ipfix_value_field fvalue5 = field5.get_ipfix_value_field( (uint8_t *) buf, 4);

	msg::mnslp_ipfix_field field6 = mess->get_field_definition( 0, IPFIX_FT_TCPDESTINATIONPORT );
	msg::mnslp_ipfix_value_field fvalue6 = field6.get_ipfix_value_field(destination_port);

	msg::mnslp_ipfix_field field7 = mess->get_field_definition( 0, IPFIX_FT_PROTOCOLIDENTIFIER );
	msg::mnslp_ipfix_value_field fvalue7 = field7.get_ipfix_value_field( protocol );
	
	msg::mnslp_ipfix_data_record data(templateoptionid);
	data.insert_field(0, IPFIX_FT_FLOWKEYINDICATOR, fvalue1);
	data.insert_field(0, IPFIX_FT_COLLECTORIPV4ADDRESS, fvalue2);
	data.insert_field(0, IPFIX_FT_COLLECTORPROTOCOLVERSION, fvalue3);
	data.insert_field(0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL, fvalue4);
	data.insert_field(0, IPFIX_FT_SOURCEIPV4ADDRESS, fvalue5);
	data.insert_field(0, IPFIX_FT_TCPDESTINATIONPORT, fvalue6);
	data.insert_field(0, IPFIX_FT_PROTOCOLIDENTIFIER, fvalue7);
	mess->include_data(templateoptionid, data);
	
}


/*
 * Call the event handler and delete the event afterwards.
 */
void ForwarderTest::process(nf_session_test &s, event *evt) {
	d->clear();
	s.process(d, evt);
	delete evt;
}

void ForwarderTest::setUp() {
	conf = new mock_mnslp_config();
	rule_installer = new nop_policy_rule_installer(conf);
	d = new mock_dispatcher(NULL, rule_installer, conf);
	destination = hostaddress("157.253.203.5");
	
	int sourceid = 0x00000000;
	mess1 = new msg::mnslp_ipfix_message(sourceid, IPFIX_VERSION, true);
	mess2 = new msg::mnslp_ipfix_message(sourceid, IPFIX_VERSION, true);
	mess3 = new msg::mnslp_ipfix_message(sourceid, IPFIX_VERSION, true);
	
	add_export_fields1();
	add_export_fields2();
	add_export_fields3();
	add_configuration_fields(mess1);
	add_configuration_fields(mess2);
	add_configuration_fields(mess3);	
}

void ForwarderTest::tearDown() 
{
	delete d;
	delete rule_installer;
	delete conf;
	delete mess1;
	delete mess2;
	delete mess3;
}


msg::ntlp_msg *
ForwarderTest::create_mnslp_configure(uint32 msn, uint32 lt) const 
{

	msg::mnslp_configure *configure = new mnslp_configure();
	configure->set_msg_sequence_number(msn);
	configure->set_session_lifetime(lt);
	configure->set_selection_metering_entities(selection_metering_entities::sme_any);
	configure->set_mspec_object(mess1->copy());
	configure->set_mspec_object(mess2->copy());
	configure->set_mspec_object(mess3->copy());
	
	ntlp::mri *ntlp_mri = new ntlp::mri_pathcoupled(
		hostaddress("192.168.0.4"), 32, 0,
		hostaddress("192.168.0.5"), 32, 0,
		"tcp", 0, 0, 0, true
	);

	return new msg::ntlp_msg(session_id(), configure, ntlp_mri, 0);
}


msg::ntlp_msg *
ForwarderTest::create_mnslp_response(uint8 severity,
		uint8 response_code, uint16 msg_type, uint32 msn) const 
{

	mnslp_response *resp = new mnslp_response();
	resp->set_information_code(severity, response_code, msg_type);
	resp->set_msg_sequence_number(msn);

	ntlp::mri *ntlp_mri = new ntlp::mri_pathcoupled(
		hostaddress("192.168.0.4"), 32, 0,
		hostaddress("192.168.0.5"), 32, 0,
		"tcp", 0, 0, 0, true
	);

	return new msg::ntlp_msg(session_id(), resp, ntlp_mri, 0);
}

msg::ntlp_msg *
ForwarderTest::create_mnslp_refresh(uint32 msn, uint32 lt) const 
{

	msg::mnslp_refresh *refresh = new mnslp_refresh();
	refresh->set_session_lifetime(lt);
	refresh->set_msg_sequence_number(msn);

	msg::ntlp_msg *msg = new msg::ntlp_msg(
		session_id(), refresh,
		new ntlp::mri_pathcoupled(), 0
	);

	return msg;
}


void ForwarderTest::testClose() {
	/*
	 * STATE_CLOSE ---[rx_CONFIGURE && CONFIGURE(Lifetime>0) ]---> STATE_PENDING
	 */
	nf_session_test s1(nf_session::STATE_CLOSE, conf);
	event *e1 = new msg_event(new session_id(s1.get_id()),
		create_mnslp_configure());

	process(s1, e1);
	ASSERT_STATE(s1, nf_session::STATE_PENDING);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	/*
	 * STATE_CLOSE ---[rx_CONFIGURE && CONFIGURE(Lifetime > MAX) ]---> STATE_PENDING
	 */
	nf_session_test s2(nf_session::STATE_CLOSE, conf);
	event *e2 = new msg_event(new session_id(s2.get_id()),
		create_mnslp_configure(START_MSN, 1000000)); // more than allowed

	process(s2, e2);
	ASSERT_STATE(s2, nf_session::STATE_PENDING);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());
}


void ForwarderTest::testPending() {
	
	/*
	 * STATE_PENDING ---[rx_RESPONSE(SUCCESS,CONFIGURE)]---> STATE_METERING
	 */
	nf_session_test s1(nf_session::STATE_PENDING, conf);
	s1.set_last_configure_message(create_mnslp_configure());

	ntlp_msg *resp1 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN);

	event *e1 = new msg_event(new session_id(s1.get_id()), resp1);

	process(s1, e1);
	ASSERT_STATE(s1, nf_session::STATE_METERING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	/*
	 * STATE_PENDING ---[rx_RESPONSE(ERROR,CONFIGURE)]---> STATE_CLOSE
	 */
	nf_session_test s2(nf_session::STATE_PENDING, conf);
	s2.set_last_configure_message(create_mnslp_configure());

	ntlp_msg *resp2 = create_mnslp_response(
		information_code::sc_permanent_failure, 0,
		information_code::obj_none, START_MSN);

	event *e2 = new msg_event(new session_id(s2.get_id()), resp2);

	process(s2, e2);
	ASSERT_STATE(s2, nf_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_permanent_failure);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_PENDING ---[rx_CONFIGURE && CONFIGURE(Lifetime == 0) ]---> STATE_CLOSE
	 */
	nf_session_test s3(nf_session::STATE_PENDING, conf);
	s3.set_last_configure_message(create_mnslp_configure());

	event *e3 = new msg_event(new session_id(s3.get_id()),
			create_mnslp_configure(START_MSN+1, 0));

	// We must to wait for a response successful response message. 
	process(s3, e3);
	ASSERT_STATE(s3, nf_session::STATE_CLOSE);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_PENDING ---[rx_CONFIGURE && CONFIGURE(Lifetime > 0) ]---> STATE_PENDING
	 */
	nf_session_test s4(nf_session::STATE_PENDING, conf);
	s4.set_last_configure_message(create_mnslp_configure());

	event *e4 = new msg_event(new session_id(s4.get_id()),
			create_mnslp_configure(START_MSN, 10));

	// We must to wait for a response successful response message. 
	process(s4, e4);
	ASSERT_STATE(s4, nf_session::STATE_PENDING);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s4.get_state_timer());

	/*
	 * STATE_PENDING ---[rx_CONFIGURE && CONFIGURE(Lifetime > 0) ]---> STATE_PENDING
	 * State changes from participating to forward because it is assumed any 
	 * selection metering entities.
	 */
	nf_session_test s5(nf_session::STATE_PENDING, conf);
	s5.set_last_configure_message(create_mnslp_configure());

	event *e5 = new msg_event(new session_id(s5.get_id()),
			create_mnslp_configure(START_MSN+1, 10));

	// We must to wait for a response successful response message. 
	process(s5, e5);
	ASSERT_STATE(s5, nf_session::STATE_PENDING);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s5.get_state_timer());
	
	/*
	 * STATE_PENDING ---[STATE_TIMEOUT]---> STATE_CLOSE
	 */
	nf_session_test s6(nf_session::STATE_PENDING, conf);

	s6.get_state_timer().set_id(47);
	s6.set_last_configure_message(create_mnslp_configure());
	timer_event *e6 = new timer_event(NULL, 47);

	process(s6, e6);
	ASSERT_STATE(s6, nf_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_permanent_failure);
	ASSERT_NO_TIMER(d);
	
}

void ForwarderTest::testMetering() {
	

	/*
	 * STATE_METERING ---[RESPONSE_TIMEOUT]---> STATE_CLOSE
	 */
	nf_session_test s2(nf_session::STATE_METERING, conf);

	s2.set_last_refresh_message(create_mnslp_refresh(START_MSN+1, 10));
	s2.get_response_timer().set_id(47);

	timer_event *e2 = new timer_event(NULL, 47);

	process(s2, e2);
	ASSERT_STATE(s2, nf_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_permanent_failure);
	ASSERT_NO_TIMER(d);
		
    /*
     * STATE_METERING ---[ && REFRESH(Lifetime == 0) ]---> STATE_METERING
	 */
	nf_session_test s3(nf_session::STATE_METERING, conf);
	s3.set_last_configure_message(create_mnslp_configure());

	event *e3 = new msg_event(new session_id(s3.get_id()),
		create_mnslp_refresh(START_MSN+1, 0));

	process(s3, e3);
	ASSERT_STATE(s3, nf_session::STATE_METERING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());
	
	/*
	 * STATE_METERING ---[rx_REFRESH && REFRESH(Lifetime == 0) ]---> STATE_METERING
	 */
	nf_session_test s4(nf_session::STATE_METERING, conf);
	s4.set_last_refresh_message(create_mnslp_refresh(START_MSN+1, 10));

	event *e4 = new msg_event(new session_id(s4.get_id()),
		create_mnslp_refresh(START_MSN+1, 0));

	process(s4, e4);
	ASSERT_STATE(s4, nf_session::STATE_METERING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s4.get_response_timer());
	
	/*
	 * STATE_METERING ---[rx_REFRESH, STATE_TIMEOUT]---> STATE_CLOSE
	 */
	nf_session_test s5(nf_session::STATE_METERING, conf);

	s5.set_last_refresh_message(create_mnslp_refresh(START_MSN+1, 10));
	s5.get_state_timer().set_id(47);

	timer_event *e5 = new timer_event(NULL, 47);
	
	process(s5, e5);
		
	ASSERT_STATE(s5, nf_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
	/*
	 * STATE_METERING ---[rx_configure, STATE_TIMEOUT]---> STATE_CLOSE
	 */
	nf_session_test s6(nf_session::STATE_METERING, conf);

	s6.set_last_configure_message(create_mnslp_configure());
	s6.get_state_timer().set_id(47);

	timer_event *e6 = new timer_event(NULL, 47);

	process(s6, e6);
	ASSERT_STATE(s6, nf_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
	/*
	 * STATE_METERING ---[rx_REFRESH , RESPONSE=success, lifetime >0 ]---> STATE_METERING
	 */
	nf_session_test s7(nf_session::STATE_METERING, conf);

	s7.set_last_refresh_message(create_mnslp_refresh(START_MSN+1, 10));
	s7.set_lifetime(10);

	ntlp_msg *resp1 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN+1);

	event *e7 = new msg_event(new session_id(s7.get_id()), resp1);

	process(s7, e7);
	ASSERT_STATE(s7, nf_session::STATE_METERING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s7.get_state_timer());

	/*
	 * STATE_METERING ---[rx_REFRESH , RESPONSE=success, lifetime =0 ]---> STATE_CLOSE
	 */
	nf_session_test s8(nf_session::STATE_METERING, conf);

	s8.set_last_refresh_message(create_mnslp_refresh(START_MSN+1, 0));
	s8.set_lifetime(0);

	ntlp_msg *resp2 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN+1);

	event *e8 = new msg_event(new session_id(s8.get_id()), resp2);

	process(s8, e8);
	ASSERT_STATE(s8, nf_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s8.get_state_timer());
	
}

void 
ForwarderTest::testIntegratedStateMachine()
{

	/*
	 * STATE_CLOSE ---[rx_CONFIGURE && CONFIGURE(Lifetime>0) ]---> STATE_PENDING
	 */
	nf_session_test s1(nf_session::STATE_CLOSE, conf);
	event *e1 = new msg_event(new session_id(s1.get_id()),
		create_mnslp_configure());

	process(s1, e1);
	ASSERT_STATE(s1, nf_session::STATE_PENDING);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());


	/*
	 * STATE_PENDING ---[rx_RESPONSE(SUCCESS,CONFIGURE)]---> STATE_METERING
	 */
	ntlp_msg *resp = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN);

	event *e2 = new msg_event(new session_id(s1.get_id()), resp);

	process(s1, e2);
	ASSERT_STATE(s1, nf_session::STATE_METERING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

    /*
     * STATE_METERING ---[ && REFRESH(Lifetime == 0) ]---> STATE_METERING
	 */
	event *e3 = new msg_event(new session_id(s1.get_id()),
							create_mnslp_refresh(START_MSN+1, 0));

	process(s1, e3);
	ASSERT_STATE(s1, nf_session::STATE_METERING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_response_timer());

	/*
	 * STATE_METERING ---[rx_REFRESH, lifetime =0, RESPONSE=success ]---> STATE_CLOSE
	 */

	ntlp_msg *resp2 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN+1);

	event *e4 = new msg_event(new session_id(s1.get_id()), resp2);

	process(s1, e4);
	ASSERT_STATE(s1, nf_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	
}

// EOF
