/*
 * Test the policy_action_mapping class.
 *
 * $Id: policy_action_mapping.cpp 1711 2015-01-24 8:58:00 amarentes $
 * $HeadURL: https://./test/policy_action_mapping.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <ext/hash_map>
#include "netmate_policy_action_mapping.h"
#include "netmate_metering_config.h"
#include "msg/mnslp_ipfix_field.h"
#include <string>
#include <iostream>

using namespace mnslp;


class PolicyActionMappingTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in mnslp_ipfix_fields.
 */
class policy_action_mapping_test : public netmate_policy_action_mapping 
{
  public:
  
	policy_action_mapping_test(): netmate_policy_action_mapping() { }

	friend class PolicyActionMappingTest;
};



class PolicyActionMappingTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( PolicyActionMappingTest );

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
    
    netmate_metering_config *netmate;
    policy_action_mapping_test *ptrActionMapping; 

};

CPPUNIT_TEST_SUITE_REGISTRATION( PolicyActionMappingTest );

void PolicyActionMappingTest::setUp() {

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
	
	ptrActionMapping = new policy_action_mapping_test();
	
	netmate = new netmate_metering_config();
	netmate->set_priority(10);
	netmate->set_metering_procedure("jitter");
	netmate->set_export_directory("/tmp/");
	netmate->set_export_interval(10);    
	netmate->set_export_procedure("file");

			
}


void PolicyActionMappingTest::tearDown() 
{
	delete(ptrField1);
	delete(ptrField2);
	delete(ptrField3);
	delete(ptrField4);
	delete(ptrActionMapping);
}

void PolicyActionMappingTest::general_test() 
{
	application_field_mapping field_mapping1 ("false","false","true","packets"); 
	ptrActionMapping->set_field(ptrField1->get_field_key(), field_mapping1);
	
	application_field_mapping field_mapping2 ("false","true","true","IPToS"); 
	ptrActionMapping->set_field(ptrField2->get_field_key(), field_mapping2);

	application_field_mapping packets = ptrActionMapping->get_field(ptrField1->get_field_key());
	CPPUNIT_ASSERT( packets == field_mapping1 );
	
	application_field_mapping Iptos = ptrActionMapping->get_field(ptrField2->get_field_key());
	CPPUNIT_ASSERT( Iptos == field_mapping2 );
	
	policy_action_mapping_test *ptrActionMapping2 = new policy_action_mapping_test();
	
	// Verify equal operator
	
	ptrActionMapping2->set_field(ptrField1->get_field_key(), field_mapping1);
	ptrActionMapping2->set_field(ptrField2->get_field_key(), field_mapping2);
	CPPUNIT_ASSERT( *ptrActionMapping == *ptrActionMapping2 );
	
	ptrActionMapping->set_metering_configuration(netmate);
	
	netmate_policy_action_mapping * ptrActionMapping3 = 
						new netmate_policy_action_mapping(*ptrActionMapping2);
	
	CPPUNIT_ASSERT( *ptrActionMapping3 == *ptrActionMapping );
	
	delete(ptrActionMapping3);		 	
	delete(ptrActionMapping2);
}
