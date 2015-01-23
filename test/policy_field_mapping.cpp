/*
 * Test the mspec_rule_key class.
 *
 * $Id: mspec_rule_key.cpp 1711 2015-01-21 18:24:00 amarentes $
 * $HeadURL: https://./test/mspec_rule_key.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <ext/hash_map>
#include "policy_field_mapping.h"
#include "msg/mnslp_ipfix_field.h"
#include <string>
#include <iostream>

using namespace mnslp;


class PolicyFieldMappingTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in mnslp_ipfix_fields.
 */
class mnslp_field_mapping_test : public policy_field_mapping 
{
  public:
  
	mnslp_field_mapping_test(): policy_field_mapping() { }

	friend class PolicyFieldMappingTest;
};



class PolicyFieldMappingTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( PolicyFieldMappingTest );

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
    
    mnslp_field_mapping_test *ptrFieldMapping; 

};

CPPUNIT_TEST_SUITE_REGISTRATION( PolicyFieldMappingTest );

void PolicyFieldMappingTest::setUp() {

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
	
	ptrFieldMapping = new mnslp_field_mapping_test();
			
}


void PolicyFieldMappingTest::tearDown() 
{
	delete(ptrField1);
	delete(ptrField2);
	delete(ptrField3);
	delete(ptrField4);
	delete(ptrFieldMapping);
}

void PolicyFieldMappingTest::general_test() 
{
	ptrFieldMapping->set_field(ptrField1->get_field_key(), "packets");
	ptrFieldMapping->set_field(ptrField2->get_field_key(), "IPToS");

	std::string packets = ptrFieldMapping->get_field(ptrField1->get_field_key());
	CPPUNIT_ASSERT( packets.compare("packets") == 0 );
	
	std::string Iptos = ptrFieldMapping->get_field(ptrField2->get_field_key());
	CPPUNIT_ASSERT( Iptos.compare("IPToS") == 0 );
	
	
	mnslp_field_mapping_test *ptrFieldMapping2 = new mnslp_field_mapping_test();
	
	// Verify equal operator
	ptrFieldMapping2->set_field(ptrField1->get_field_key(), "packets");
	ptrFieldMapping2->set_field(ptrField2->get_field_key(), "IPToS");
	CPPUNIT_ASSERT( *ptrFieldMapping2 == *ptrFieldMapping );
	
	// Verify that inserting the same field does not generate duplicates
	ptrFieldMapping2->set_field(ptrField3->get_field_key(), "packets");
	CPPUNIT_ASSERT( *ptrFieldMapping2 == *ptrFieldMapping );
	
	// Verify the different operator
	ptrFieldMapping2->set_field(ptrField4->get_field_key(), "Proto");
	CPPUNIT_ASSERT( *ptrFieldMapping2 != *ptrFieldMapping );
	
	// In this case the function must return true
	CPPUNIT_ASSERT( ptrFieldMapping2->check_field_availability(*ptrField3) == true);

	// In this case the function must return true
	CPPUNIT_ASSERT( ptrFieldMapping->check_field_availability(*ptrField4) == false);
	
	std::string Iptos2 = ptrFieldMapping2->get_field_traslate(*ptrField2);
	CPPUNIT_ASSERT( Iptos2.compare("IPToS") == 0 );
	
	msg::mnslp_ipfix_field *ptrField5 = const_cast <msg::mnslp_ipfix_field *>(ptrField2);
	std::string Iptos3 = ptrFieldMapping2->get_field_traslate(*ptrField5);
	CPPUNIT_ASSERT( Iptos3.compare("IPToS") == 0 );
	 
	
	delete(ptrFieldMapping2);
}
