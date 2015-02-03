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
	void testMetering();
	void testIntegratedStateMachine();

  private:
	static const uint32 START_MSN = 77;
	static const uint32 SESSION_LIFETIME = 30;

	void process(nr_session_test &s, event *evt);

	msg::ntlp_msg *create_mnslp_configure(uint32 msn=START_MSN,
		uint32 session_lifetime=SESSION_LIFETIME) const;
	
	msg::ntlp_msg *create_mnslp_refresh(uint32 msn, uint32 lt) const;

	mock_mnslp_config *conf;
	nop_policy_rule_installer *policy_installer;
	mock_dispatcher *d;
	hostaddress destination;
	
	msg::mnslp_ipfix_message *mess1;
	msg::mnslp_ipfix_message *mess2;
	msg::mnslp_ipfix_message *mess3;
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( ResponderTest );


void ResponderTest::add_export_fields1() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess1->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess1->add_field(templatedataid, 0, IPFIX_FT_OCTETDELTACOUNT, 8);
	
}

void ResponderTest::add_export_fields2() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess2->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess2->add_field(templatedataid, 0, IPFIX_FT_PACKETDELTACOUNT, 8);	
}

void ResponderTest::add_export_fields3() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess3->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess3->add_field(templatedataid, 0, IPFIX_FT_MINIMUMIPTOTALLENGTH, 8);	
}

void ResponderTest::add_configuration_fields(msg::mnslp_ipfix_message * mess) 
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
void ResponderTest::process(nr_session_test &s, event *evt) {
	d->clear();
	s.process(d, evt);
	delete evt;
}

void ResponderTest::setUp() 
{
	conf = new mock_mnslp_config();
	policy_installer = new nop_policy_rule_installer(conf);
	d = new mock_dispatcher(NULL, policy_installer, conf);
	
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

void ResponderTest::tearDown() 
{
	delete d;
	delete policy_installer;	
	delete conf;
	delete mess1;
	delete mess2;
	delete mess3;
}


msg::ntlp_msg *
ResponderTest::create_mnslp_configure(uint32 msn, uint32 lt) const {

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
		"tcp", 0, 0, 0, false
	);

	return new msg::ntlp_msg(session_id(), configure, ntlp_mri, 0);
}


msg::ntlp_msg *
ResponderTest::create_mnslp_refresh(uint32 msn, uint32 lt) const {

	msg::mnslp_refresh *refresh = new mnslp_refresh();
	refresh->set_session_lifetime(lt);
	refresh->set_msg_sequence_number(msn);

	msg::ntlp_msg *msg = new msg::ntlp_msg(
		session_id(), refresh,
		new ntlp::mri_pathcoupled(), 0
	);

	return msg;
}


void 
ResponderTest::testClose() {
	
	std::cout << "testClose 1" << std::endl;
	
	/*
	 * CLOSE ---[rx_CONFIGURE && CHECK_AA && CONFIGURE(Lifetime>0) ]---> METERING
	 */
	nr_session_test s1(nr_session::STATE_CLOSE);
	event *e1 = new msg_event(NULL, create_mnslp_configure(), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_METERING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	std::cout << "testClose 2" << std::endl;
	
	/*
	 * CLOSE ---[rx_CONFIGURE && CONFIGURE(Lifetime > MAX) ]---> METERING
	 */
	nr_session_test s2(nr_session::STATE_CLOSE);
	event *e2 = new msg_event(NULL,
		create_mnslp_configure(START_MSN+1, 100000), true);
	
	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_METERING);
	ASSERT_RESPONSE_MESSAGE_SENT(d,
		information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());
	
	std::cout << "testClose 3" << std::endl;
}


void 
ResponderTest::testMetering() {
	
	std::cout << "Test Metering 1" << std::endl;
	
	/*
	 * STATE_METERING ---[rx_REFRESH && CHECK_AA && REFRESH(Lifetime>0) ]---> STATE_METERING
	 */
	nr_session_test s1(nr_session::STATE_METERING, START_MSN);
	event *e1 = new msg_event(NULL, create_mnslp_refresh(START_MSN+1, 10), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_METERING);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	std::cout << "Test Metering 2" << std::endl;

	/*
	 * STATE_METERING ---[rx_REFRESH && REFRESH(Lifetime > MAX) ]---> STATE_METERING
	 */
	nr_session_test s2(nr_session::STATE_METERING, START_MSN);
	event *e2 = new msg_event(NULL,
		create_mnslp_refresh(START_MSN+1, 10), true);

	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_METERING);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());

	std::cout << "Test Metering 3" << std::endl;

	/*
	 * STATE_METERING ---[rx_REFRESH && REFRESH(Lifetime == 0) ]--->STATE_CLOSE
	 */
	nr_session_test s3(nr_session::STATE_METERING, START_MSN);
	event *e3 = new msg_event(NULL,
			create_mnslp_refresh(START_MSN+1, 0), true);

	process(s3, e3);
	ASSERT_STATE(s3, nr_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_NO_TIMER(d);

	std::cout << "Test Metering 4" << std::endl;

	/*
	 * STATE_METERING_PART ---[rx_REFRESH && MSN too low ]---> STATE_METERING_PART
	 */
	nr_session_test s4(nr_session::STATE_METERING, START_MSN);
	event *e4 = new msg_event(NULL, create_mnslp_refresh(START_MSN, 10), true);

	process(s4, e4);
	ASSERT_STATE(s4, nr_session::STATE_METERING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
	std::cout << "Test Metering 5" << std::endl;
}

void 
ResponderTest::testIntegratedStateMachine()
{

	/*
	 * CLOSE ---[rx_CONFIGURE && CHECK_AA && CONFIGURE(Lifetime>0) ]---> METERING
	 */
	nr_session_test s1(nr_session::STATE_CLOSE);
	event *e1 = new msg_event(NULL, create_mnslp_configure(), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_METERING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());


	/*
	 * STATE_METERING ---[rx_REFRESH && REFRESH(Lifetime == 0) ]--->STATE_CLOSE
	 */
	event *e2 = new msg_event(NULL,
			create_mnslp_refresh(START_MSN+1, 0), true);

	process(s1, e2);
	ASSERT_STATE(s1, nr_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_NO_TIMER(d);


}

// EOF
