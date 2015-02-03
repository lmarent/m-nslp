/*
 * Test the netmate_ipfix_metering_config class.
 *
 * $Id: netmate_ipfix_metering_config.cpp 1711 2015-01-24 8:58:00 amarentes $
 * $HeadURL: https://./test/netmate_ipfix_metering_config.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <ext/hash_map>
#include "netmate_ipfix_metering_config.h"
#include "msg/mnslp_ipfix_field.h"
#include <string>
#include <iostream>
#include <limits>

using namespace mnslp;


class NetmateMeteringConfigTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in mnslp_ipfix_fields.
 */
class netmate_metering_config_test : public netmate_ipfix_metering_config
{
  public:
  
	netmate_metering_config_test(): netmate_ipfix_metering_config() { }

	friend class NetmateMeteringConfigTest;
};



class NetmateMeteringConfigTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( NetmateMeteringConfigTest );

	CPPUNIT_TEST( general_test );

	CPPUNIT_TEST_SUITE_END();

  public:
  
	void setUp();
	void general_test(); 
	void tearDown();

  private:
	netmate_metering_config_test *netmate;

};

CPPUNIT_TEST_SUITE_REGISTRATION( NetmateMeteringConfigTest );

void NetmateMeteringConfigTest::setUp() 
{
	netmate = new netmate_metering_config_test();
}

void NetmateMeteringConfigTest::general_test() 
{
	// Verify that it is assigning the greater integer.
	CPPUNIT_ASSERT( netmate->get_priority() == std::numeric_limits<int>::max() );

	netmate_ipfix_metering_config *netmate2 = netmate->copy();
	CPPUNIT_ASSERT( netmate->is_equal(*netmate2) );
	
	netmate->set_priority(10);
	CPPUNIT_ASSERT( netmate->not_equal(*netmate2) );
	
	netmate2->set_priority(10);
	netmate->set_metering_procedure("jitter");
	CPPUNIT_ASSERT( netmate->not_equal(*netmate2) );
	
	netmate2->set_metering_procedure("jitter");
	netmate->set_export_directory("/tmp/");
	CPPUNIT_ASSERT( netmate->not_equal(*netmate2) );
	
	netmate2->set_export_directory("/tmp/");
	netmate->set_export_interval(10);    
	CPPUNIT_ASSERT( netmate->not_equal(*netmate2) );
	netmate2->set_export_interval(10);    
	netmate->set_export_procedure("file");
	
	CPPUNIT_ASSERT( netmate->not_equal(*netmate2) );
	netmate2->set_export_procedure("file");
	CPPUNIT_ASSERT( netmate->is_equal(*netmate2) );
	
	
	// Verify getters
	CPPUNIT_ASSERT( netmate->get_priority() == 10 );
	CPPUNIT_ASSERT( (netmate->get_metering_procedure().compare("jitter")) == 0 );
	CPPUNIT_ASSERT( (netmate->get_export_directory().compare("/tmp/")) == 0 );
	CPPUNIT_ASSERT( (netmate->get_export_interval()) == 10 );
	CPPUNIT_ASSERT( (netmate->get_export_procedure().compare("file")) == 0);
		
}

void NetmateMeteringConfigTest::tearDown() 
{
	delete(netmate);
}

