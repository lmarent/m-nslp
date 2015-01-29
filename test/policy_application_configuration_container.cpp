/*
 * Test the policy_application_configuration_container class.
 *
 * $Id: policy_application_configuration_container.cpp 1711 2015-01-21 18:24:00 amarentes $
 * $HeadURL: https://./test/policy_application_configuration_container.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <iostream>

#include "policy_application_configuration_container.h"
#include "msg/mnslp_ipfix_field.h"


using namespace mnslp;


class PolicyApplicationConfigurationContainerTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in mnslp_ipfix_fields.
 */
class policy_application_configuration_container_test : public policy_application_configuration_container
{
  public:
  
	policy_application_configuration_container_test(): policy_application_configuration_container() { }

	friend class PolicyApplicationConfigurationContainerTest;
};



class PolicyApplicationConfigurationContainerTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( PolicyApplicationConfigurationContainerTest );

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
    
    policy_field_mapping *ptrFieldMapping; 
    policy_application_configuration_container_test *ptrAppContainer;

};

CPPUNIT_TEST_SUITE_REGISTRATION( PolicyApplicationConfigurationContainerTest );

void PolicyApplicationConfigurationContainerTest::setUp() {

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
	
	ptrFieldMapping = new policy_field_mapping();
	ptrAppContainer = new policy_application_configuration_container_test();
			
}


void PolicyApplicationConfigurationContainerTest::tearDown() 
{
	delete(ptrField1);
	delete(ptrField2);
	delete(ptrField3);
	delete(ptrField4);
	delete(ptrFieldMapping);
	delete(ptrAppContainer);
}

void PolicyApplicationConfigurationContainerTest::general_test() 
{
	application_field_mapping field_mapping1 ("false","false","true","packets"); 
	ptrFieldMapping->set_field(ptrField1->get_field_key(), field_mapping1);
	
	application_field_mapping field_mapping2 ("false","true","true","IPToS"); 
	ptrFieldMapping->set_field(ptrField2->get_field_key(), field_mapping2);
	ptrFieldMapping->set_metering_application("netmate");
	
	
	// Verify equal operator
	policy_field_mapping *ptrFieldMapping2 = new policy_field_mapping();
	application_field_mapping field_mapping3 ("false","false","true","packets");
	ptrFieldMapping2->set_field(ptrField3->get_field_key(), field_mapping3);
	
	application_field_mapping field_mapping4 ("false","true","true","Proto"); 
	ptrFieldMapping2->set_field(ptrField4->get_field_key(), field_mapping4);
	
	ptrFieldMapping2->set_metering_application("netmate2");

	ptrAppContainer->set_application(ptrFieldMapping->get_metering_application(), 
						           *ptrFieldMapping);

	ptrAppContainer->set_application(ptrFieldMapping2->get_metering_application(), 
						           *ptrFieldMapping2);
	
	policy_application_configuration_container *ptrAppContainer2 = 
					   new policy_application_configuration_container(*ptrAppContainer);
					   
	CPPUNIT_ASSERT( *ptrAppContainer2 == *ptrAppContainer );

	CPPUNIT_ASSERT( ptrAppContainer->check_field_availability("netmate", *ptrField1) == true );
	
	CPPUNIT_ASSERT( ptrAppContainer->check_field_availability("netmate", *ptrField4) == false );

	CPPUNIT_ASSERT( ptrAppContainer->check_field_availability("netmate2", *ptrField4) == true );

	CPPUNIT_ASSERT( (ptrAppContainer->get_field_traslate("netmate2", *ptrField4)).compare("Proto") == 0 );
	
	delete(ptrFieldMapping2);
	delete(ptrAppContainer2);
}
