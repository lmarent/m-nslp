/*
 * Test the policy_action class.
 *
 * $Id: policy_action.cpp 1711 2015-01-24 8:58:00 amarentes $
 * $HeadURL: https://./test/policy_action.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <ext/hash_map>
#include "policy_action.h"
#include "netmate_policy_action_mapping.h"
#include "msg/mnslp_ipfix_field.h"
#include <string>
#include <iostream>

using namespace mnslp;


class PolicyActionTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in mnslp_ipfix_fields.
 */
class policy_action_test : public policy_action
{
  public:
  
	policy_action_test(): policy_action() { }

	friend class PolicyActionTest;
};



class PolicyActionTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( PolicyActionTest );

	CPPUNIT_TEST( general_test );

	CPPUNIT_TEST_SUITE_END();

  public:
  
	void setUp();
	void general_test(); 
	void tearDown();

  private:
    ipfix_field_type_t field1;
    ipfix_field_type_t field2;
    ipfix_field_type_t field3;
    ipfix_field_type_t field4;
    
    msg::mnslp_ipfix_field *ptrField1;
    msg::mnslp_ipfix_field *ptrField2;
    msg::mnslp_ipfix_field *ptrField3;
    msg::mnslp_ipfix_field *ptrField4;
    
    netmate_metering_config *netmate1;
    netmate_metering_config *netmate2;
    
    netmate_policy_action_mapping *ptrActionMapping1; 
    netmate_policy_action_mapping *ptrActionMapping2; 
    
    policy_action_test *ptrPolicyAction;
    

};

CPPUNIT_TEST_SUITE_REGISTRATION( PolicyActionTest );

void PolicyActionTest::setUp() {

	field1.eno = 0;  
	field1.ftype = IPFIX_FT_PACKETDELTACOUNT;
	field1.length = 8; 
	field1.coding =  IPFIX_CODING_UINT;
	field1.name = "packetDeltaCount";
	field1.documentation = "";

	field2.eno = 0;  
	field2.ftype = IPFIX_FT_IPCLASSOFSERVICE;
	field2.length = 1; 
	field2.coding =  IPFIX_CODING_UINT;
	field2.name = "ipClassOfService";
	field2.documentation = "";

	field3.eno = 0;  
	field3.ftype = IPFIX_FT_PACKETDELTACOUNT;
	field3.length = 8; 
	field3.coding =  IPFIX_CODING_UINT;
	field3.name = "packetDeltaCount";
	field3.documentation = "";

	field4.eno = 0;  
	field4.ftype = IPFIX_FT_PROTOCOLIDENTIFIER;
	field4.length = 1; 
	field4.coding =  IPFIX_CODING_UINT;
	field4.name = "protocolIdentifier";
	field4.documentation = "";

	ptrField1 = new msg::mnslp_ipfix_field(field1);
	ptrField2 = new msg::mnslp_ipfix_field(field2);
	ptrField3 = new msg::mnslp_ipfix_field(field3);
	ptrField4 = new msg::mnslp_ipfix_field(field4);

	netmate1 = new netmate_metering_config();
	netmate1->set_priority(1);
	netmate1->set_metering_procedure("jitter");
	netmate1->set_export_directory("/tmp/");
	netmate1->set_export_interval(10);    
	netmate1->set_export_procedure("file");

	netmate2 = new netmate_metering_config();
	netmate2->set_priority(2);
	netmate2->set_metering_procedure("BandWidth");
	netmate2->set_export_directory("/tmp/");
	netmate2->set_export_interval(5);    
	netmate2->set_export_procedure("file");

	
	ptrActionMapping1 = new netmate_policy_action_mapping();
	ptrActionMapping2 = new netmate_policy_action_mapping();
	
	ptrPolicyAction = new policy_action_test();
	
}


void PolicyActionTest::tearDown() 
{
	delete(ptrField1);
	delete(ptrField2);
	delete(ptrField3);
	delete(ptrField4);
	delete(ptrPolicyAction);
}

void PolicyActionTest::general_test() 
{

	application_field_mapping field_mapping1 ("false","false","true","packets_1"); 
	ptrActionMapping1->set_field(ptrField1->get_field_key(), field_mapping1);
	
	application_field_mapping field_mapping2 ("false","true","true","IPToS_1"); 
	ptrActionMapping1->set_field(ptrField2->get_field_key(), field_mapping2);
	ptrActionMapping1->set_metering_application("netmate");
	ptrActionMapping1->set_metering_configuration(netmate1);
					
	application_field_mapping field_mapping3 ("false","false","true","packets_2"); 
	ptrActionMapping2->set_field(ptrField1->get_field_key(), field_mapping3);

	application_field_mapping field_mapping4 ("false","true","true","IPToS_4"); 
	ptrActionMapping2->set_field(ptrField2->get_field_key(), field_mapping4);		
	ptrActionMapping2->set_metering_application("netmate");
	ptrActionMapping2->set_metering_configuration(netmate2);
	
	std::cout << "here I am:" << ptrActionMapping1->get_key() << std::endl;	
	// The key must be the proc_name, the appplication shoud be equal in general.
	ptrPolicyAction->set_action_mapping(ptrActionMapping1->get_key(), 
										ptrActionMapping1);
	
	std::cout << "here I am 2:" << ptrActionMapping2->get_key() << std::endl;									
	ptrPolicyAction->set_action_mapping(ptrActionMapping2->get_key(),
										ptrActionMapping2);
	std::cout << "here I am 3" << std::endl;
	/*
	policy_action * ptrPolicyAction2 = new policy_action(*ptrPolicyAction);
	CPPUNIT_ASSERT( *ptrPolicyAction2 == *ptrPolicyAction );
	
	ptrPolicyAction->set_action("packets1");
	ptrPolicyAction2->set_action("packets2");
	CPPUNIT_ASSERT( *ptrPolicyAction2 != *ptrPolicyAction );

	CPPUNIT_ASSERT( ptrPolicyAction->check_field_availability(
						"netmate",*ptrField1) == true );
	
	// Verifies priorities between packets in the same metering application
		
	CPPUNIT_ASSERT( (ptrPolicyAction->get_field_traslate(
						"netmate",*ptrField1)).compare("packets_2") == 0 );
	
	const netmate_metering_config *tmp = dynamic_cast< const netmate_metering_config* > (ptrPolicyAction->get_package(
						"netmate",*ptrField1));
	
	CPPUNIT_ASSERT( (tmp->get_metering_procedure()).compare("jitter") == 0 );

	delete(ptrPolicyAction2);
	*/ 
}
