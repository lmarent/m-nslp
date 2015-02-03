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
#include "msg/selection_metering_entities.h"
#include "nop_policy_rule_installer.h"
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
	static const uint32 START_MSN = 77;

	void process(ni_session_test &s, event *evt);

	msg::ntlp_msg *create_mnslp_configure() const;

	msg::ntlp_msg *create_mnslp_response(
		uint8 severity, uint8 response_code, uint16 msg_type,
		uint32 msn=START_MSN) const;

	msg::ntlp_msg *create_mnslp_refresh() const;

	mock_mnslp_config *conf;
	nop_policy_rule_installer *policy_installer;
	mock_dispatcher *d;
	hostaddress source;
	hostaddress destination;
	
	msg::mnslp_ipfix_message *mess1;
	msg::mnslp_ipfix_message *mess2;
	msg::mnslp_ipfix_message *mess3;
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( InitiatorTest );



void InitiatorTest::add_export_fields1() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess1->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess1->add_field(templatedataid, 0, IPFIX_FT_OCTETDELTACOUNT, 8);
	
}

void InitiatorTest::add_export_fields2() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess2->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess2->add_field(templatedataid, 0, IPFIX_FT_PACKETDELTACOUNT, 8);	
}

void InitiatorTest::add_export_fields3() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess3->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess3->add_field(templatedataid, 0, IPFIX_FT_MINIMUMIPTOTALLENGTH, 8);	
}


void InitiatorTest::add_configuration_fields(msg::mnslp_ipfix_message * mess) 
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
void InitiatorTest::process(ni_session_test &s, event *evt) {
	d->clear();	
	s.process(d, evt);
	delete evt;
}

void InitiatorTest::setUp() {
	conf = new mock_mnslp_config();
	policy_installer = new nop_policy_rule_installer(conf);
	d = new mock_dispatcher(NULL, policy_installer, conf);
	source = hostaddress("10.0.2.15");
	destination = hostaddress("173.194.37.80");
	
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

void InitiatorTest::tearDown() 
{
	delete d;
	delete policy_installer;	
	delete conf;
	delete mess1;
	delete mess2;
	delete mess3;
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




void InitiatorTest::testClose() {
	/*
	 * CLOSE ---[tg_CONF]---> PENDING
	 */
	ni_session_test s1(ni_session::STATE_CLOSE);
	vector<msg::mnslp_mspec_object *> mspec_objects;
	mspec_objects.push_back(mess1->copy());
	mspec_objects.push_back(mess2->copy());
	mspec_objects.push_back(mess3->copy());
	
	event *e1 = new api_configure_event(source, destination);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_PENDING);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_response_timer());

	event *e2 = new api_configure_event(source,destination,(protlib::uint16) 0, 
									   (protlib::uint16) 0, (protlib::uint8) 0,
									   (protlib::uint32) 20, START_MSN,  
										mspec_objects, selection_metering_entities::sme_any,
										conf->get_ni_session_lifetime(), NULL);

	ni_session_test s2(ni_session::STATE_CLOSE);
	process(s2, e2);
	ASSERT_STATE(s2, ni_session::STATE_PENDING);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s2.get_response_timer());

}


void InitiatorTest::testPending() {
	/*
	 * STATE_PENDING ---[rx_RESPONSE(SUCCESS,CONFIGURE)]---> STATE_METERING
	 */
	ni_session_test s1(ni_session::STATE_PENDING);
	s1.set_last_configure_message(create_mnslp_configure());

	ntlp_msg *resp1 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	event *e1 = new msg_event(NULL, resp1);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_METERING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());


	/*
	 * STATE_PENDING ---[rx_RESPONSE(ERROR)]---> CLOSE
	 */
	ni_session_test s2(ni_session::STATE_PENDING);
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
	 * STATE_PENDING ---[RESPONSE_TIMEOUT, retry]---> STATE_PENDING
	 */
	ni_session_test s3(ni_session::STATE_PENDING);
	// fake a previously sent Configure message
	s3.set_last_configure_message(create_mnslp_configure());
	s3.get_response_timer().set_id(47);

	timer_event *e3 = new timer_event(NULL, 47);

	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_PENDING);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());


	/*
	 * STATE_PENDING ---[RESPONSE_TIMEOUT, no retry]---> CLOSE
	 */
	ni_session_test s4(ni_session::STATE_PENDING);
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
	 * STATE_PENDING ---[tg_TEARDOWN]---> CLOSE
	 */
	ni_session_test s5(ni_session::STATE_PENDING);
	s5.set_last_configure_message(create_mnslp_configure());
	event *e5 = new api_teardown_event(NULL);

	process(s5, e5);
	ASSERT_STATE(s5, ni_session::STATE_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_PENDING ---[tg_TEARDOWN]---> CLOSE
	 */
	ni_session_test s6(ni_session::STATE_PENDING);
	s6.set_last_configure_message(create_mnslp_configure());
	event *e6 = new no_next_node_found_event(NULL);

	process(s6, e6);
	ASSERT_STATE(s6, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
}

void InitiatorTest::testMetering() {
	/*
	 * STATE_METERING ---[rx_RESPONSE(SUCCESS,REFRESH)]---> STATE_METERING
	 */
	ni_session_test s1(ni_session::STATE_METERING);
	s1.set_last_refresh_message(create_mnslp_refresh());

	ntlp_msg *resp1 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	event *e1 = new msg_event(NULL, resp1);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_METERING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());
	
	/*
	 * STATE_METERING ---[rx_RESPONSE(ERROR,REFRESH)]---> CLOSE
	 */
	ni_session_test s2(ni_session::STATE_METERING);
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
	 * STATE_METERING ---[RESPONSE_TIMEOUT, retry]---> STATE_METERING
	 */
	ni_session_test s3(ni_session::STATE_METERING);
	s3.set_last_refresh_message(create_mnslp_refresh());
	s3.get_response_timer().set_id(47);

	timer_event *e3 = new timer_event(NULL, 47);

	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_METERING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());


	/*
	 * STATE_METERING ---[RESPONSE_TIMEOUT, no retry]---> STATE_METERING
	 */
	ni_session_test s4(ni_session::STATE_METERING);
	s4.set_last_refresh_message(create_mnslp_refresh());	
	s4.set_refresh_counter(1000);
	s4.get_response_timer().set_id(47);
	timer_event *e4 = new timer_event(NULL, 47);

	process(s4, e4);
	ASSERT_STATE(s4, ni_session::STATE_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING ---[tg_TEARDOWN]---> STATE_CLOSE
	 */
	ni_session_test s5(ni_session::STATE_METERING);
	s5.set_last_refresh_message(create_mnslp_refresh());
	event *e5 = new api_teardown_event(NULL);

	process(s5, e5);
	ASSERT_STATE(s5, ni_session::STATE_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_METERING ---[REFRESH_TIMEOUT]---> STATE_METERING
	 */
	ni_session_test s6(ni_session::STATE_METERING);
	s6.set_last_refresh_message(create_mnslp_refresh());
	s6.get_refresh_timer().set_id(0xABCD);
	event *e6 = new timer_event(NULL, 0xABCD);

	process(s6, e6);
	ASSERT_STATE(s6, ni_session::STATE_METERING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s6.get_response_timer());
}

void InitiatorTest::testIntegratedStateMachine()
{

	vector<msg::mnslp_mspec_object *> mspec_objects;
	mspec_objects.push_back(mess1->copy());
	mspec_objects.push_back(mess2->copy());
	mspec_objects.push_back(mess3->copy());

	event *e1 = new api_configure_event(source,destination,
										(protlib::uint16) 0, //Srcport 
									    (protlib::uint16) 0, //Dstport
									    (protlib::uint8) 0, // Protocol
										START_MSN,			// Msg Seq Number		
										(protlib::uint32) 20, // Hop Count  
										mspec_objects, // Mspec Objects
										selection_metering_entities::sme_any, 
										conf->get_ni_session_lifetime(), 
										NULL);

	ni_session_test s1(ni_session::STATE_CLOSE);
	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_PENDING);
	ASSERT_CONFIGURE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_response_timer());

	
	/*
	 * STATE_PENDING ---[rx_RESPONSE(SUCCESS,CONFIGURE)]---> STATE_METERING
	 */
	ntlp_msg *resp1 = create_mnslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN);

	event *e2 = new msg_event(NULL, resp1);

	process(s1, e2);
	ASSERT_STATE(s1, ni_session::STATE_METERING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());

	
	/*
	 * STATE_METERING ---[tg_TEARDOWN]---> STATE_CLOSE
	 */
	event *e3 = new api_teardown_event(NULL);

	process(s1, e3);
	ASSERT_STATE(s1, ni_session::STATE_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


}

// EOF
