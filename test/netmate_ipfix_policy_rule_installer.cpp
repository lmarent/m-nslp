/*
 * Test the netmate_ipfix_policy_rule_installer class.
 *
 * $Id: netmate_ipfix_policy_rule_installer.cpp 1711 2015-01-21 18:24:00 amarentes $
 * $HeadURL: https://./test/netmate_ipfix_policy_rule_installer.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


#include <string>
#include <iostream>

#include "netmate_ipfix_policy_rule_installer.h"
#include "msg/mnslp_ipfix_field.h"
#include "utils.h"

using namespace mnslp;


class NetmateIpfixPolicyRuleInstallerTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public.
 */
class netmate_ipfix_policy_rule_installer_test : public netmate_ipfix_policy_rule_installer
{
  public:
  
	netmate_ipfix_policy_rule_installer_test(mnslp_config *conf): netmate_ipfix_policy_rule_installer(conf) { }

	friend class NetmateIpfixPolicyRuleInstallerTest;
};



class NetmateIpfixPolicyRuleInstallerTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( NetmateIpfixPolicyRuleInstallerTest );

	CPPUNIT_TEST( general_test );

	CPPUNIT_TEST_SUITE_END();

  public:
  
	void setUp();
	void add_export_fields1();
	void add_export_fields2();
	void add_export_fields3();
	void add_configuration_fields(msg::mnslp_ipfix_message *mess);
	void general_test();
	void tearDown();

  private:
    mock_mnslp_config *conf;
	msg::mnslp_ipfix_message *mess1;
	msg::mnslp_ipfix_message *mess2;
	msg::mnslp_ipfix_message *mess3;
	mt_policy_rule *rule1;	
	netmate_ipfix_policy_rule_installer_test *netmate;
	

};

CPPUNIT_TEST_SUITE_REGISTRATION( NetmateIpfixPolicyRuleInstallerTest );

void NetmateIpfixPolicyRuleInstallerTest::setUp() {

	conf = new mock_mnslp_config();
	netmate = new netmate_ipfix_policy_rule_installer_test(conf);
	netmate->setup();
	
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
	
	rule1 = new mt_policy_rule();
	// std::cout << "Finishing setup " << std::endl;
	
}

void NetmateIpfixPolicyRuleInstallerTest::add_export_fields1() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess1->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess1->add_field(templatedataid, 0, IPFIX_FT_OCTETDELTACOUNT, 8);
	
}

void NetmateIpfixPolicyRuleInstallerTest::add_export_fields2() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess2->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess2->add_field(templatedataid, 0, IPFIX_FT_PACKETDELTACOUNT, 8);	
}

void NetmateIpfixPolicyRuleInstallerTest::add_export_fields3() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess3->new_data_template( nfields );
	// In this case all these fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	
	mess3->add_field(templatedataid, 0, IPFIX_FT_MINIMUMIPTOTALLENGTH, 8);	
}


void NetmateIpfixPolicyRuleInstallerTest::add_configuration_fields(msg::mnslp_ipfix_message * mess) 
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


void NetmateIpfixPolicyRuleInstallerTest::tearDown() 
{
	delete(netmate);
	delete(rule1);

}

void NetmateIpfixPolicyRuleInstallerTest::general_test() 
{
	try
	{
		netmate->check(mess1);
		rule1->set_object(mess1);
	}
	catch (policy_rule_installer_error &e)
	{
		std::cout << "Mspec object not possible to include in the policy rule " << std::endl;
	}
	try{
		netmate->check(mess2);
		rule1->set_object(mess2);
	}	
	catch (policy_rule_installer_error &e)
	{
		std::cout << "Mspec object not possible to include in the policy rule " << std::endl;
	}
	try
	{
		netmate->check(mess3);
		rule1->set_object(mess3);
	}
	catch (policy_rule_installer_error &e)
	{
		std::cout << "Mspec object not possible to include in the policy rule " << std::endl;
	}
	
	netmate->install(rule1);

}
