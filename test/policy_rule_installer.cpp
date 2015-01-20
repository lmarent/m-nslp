/*
 * Test the policy_rule_installer class.
 *
 * $Id: policy_rule_installer.cpp 1936 2006-07-25 15:13:45Z stud-matfried $
 * $HeadURL: https://./test/policy_rule_installer.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "address.h"

#include "policy_rule_installer.h"
#include "nop_policy_rule_installer.h"
#include "events.h"

#include "utils.h"

using namespace mnslp;
using namespace protlib;


class Policy_Rule_Installer_Test : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( Policy_Rule_Installer_Test );

	CPPUNIT_TEST( testBasic );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void testBasic();
	void tearDown();

	mock_mnslp_config *conf;
	policy_rule_installer *installer;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Policy_Rule_Installer_Test );

void Policy_Rule_Installer_Test::setUp() {
	conf = new mock_mnslp_config();
	installer = new nop_policy_rule_installer(conf);
}

void Policy_Rule_Installer_Test::tearDown() 
{
	delete installer;
	delete conf;
}

void Policy_Rule_Installer_Test::testBasic() 
{
	try
	{
		std::cout << "Before setup:" << std::endl;
		installer->setup();
		std::cout << "After setup:" << std::endl;
		const policy_action_container actions_tmp = installer->get_action_container();
		std::cout << "bring the action container:" << std::endl;

		for (policy_action_container::const_iterator i = actions_tmp.begin(); i != actions_tmp.end(); i++ ){
			const std::string key = i->first; 
			std::cout << "action:" << key << std::endl;
		}
	}
	catch (policy_rule_installer_error &e)
	{
		std::cout << "Exception in the policy rule setup found" << e <<  std::endl;
	}

}

// EOF
