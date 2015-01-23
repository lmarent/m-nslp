/*
 * Test the mspec_rule_key class.
 *
 * $Id: mspec_rule_key.cpp 1711 2015-01-21 18:24:00 amarentes $
 * $HeadURL: https://./test/mspec_rule_key.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <ext/hash_map>
#include "mspec_rule_key.h"
#include <string>
#include <iostream>

using namespace mnslp;


class MspecRuleKeyTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( MspecRuleKeyTest );

	CPPUNIT_TEST( general_test );

	CPPUNIT_TEST_SUITE_END();

  public:
  
	void general_test() 
	{
		std::string keyStr1, keyStr3;
		mspec_rule_key key1;
		mspec_rule_key key2(key1);
		mspec_rule_key key3;
		mspec_rule_key key4;
		
		CPPUNIT_ASSERT( key1 == key2 );
		
		CPPUNIT_ASSERT( key1 != key3 );
		
		key4 = key1;
		
		CPPUNIT_ASSERT( key1 == key4 );
		
		keyStr1 = key1.get_string_key();
		
		keyStr3 = key3.get_string_key();
				
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( MspecRuleKeyTest );
