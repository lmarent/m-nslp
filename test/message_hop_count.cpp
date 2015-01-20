/*
 * Test the Message Hop Count Object
 *
 * $Id: information_code.cpp 1743 2014-11-05 14:47:10  $
 * $HeadURL: https://./test/information_code.cpp $
 */
#include "generic_object_test.h"
#include "msg/message_hop_count.h"
#include "msg/mnslp_msg.h"

using namespace mnslp::msg;

class MessageHopCountTest : public GenericObjectTest {

	CPPUNIT_TEST_SUITE( MessageHopCountTest );

	MNSLP_OBJECT_DEFAULT_TESTS();
	CPPUNIT_TEST( testBasics );

	CPPUNIT_TEST_SUITE_END();

  public:
	virtual mnslp_object *createInstance1() const {
		return new message_hop_count();
	}

	virtual mnslp_object *createInstance2() const {
		return new message_hop_count(10);
	}

	virtual void mutate1(mnslp_object *o) const {
		message_hop_count *mhc = dynamic_cast<message_hop_count *>(o);
		mhc->set_value(10);
	}

	void testBasics() {
		message_hop_count ic(12);
		CPPUNIT_ASSERT( ic.get_value() == 12 );
		ic.set_value(13);
		CPPUNIT_ASSERT( ic.get_value() == 13 );
		message_hop_count ic2 = ic;
		CPPUNIT_ASSERT( ic2.get_value() == 13 );
					
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( MessageHopCountTest );

// EOF
