/*
 * Test the Information Code Object
 *
 * $Id: information_code.cpp 1743 2014-11-05 14:47:10  $
 * $HeadURL: https://./test/information_code.cpp $
 */
#include "generic_object_test.h"
#include "msg/msg_sequence_number.h"
#include "msg/mnslp_msg.h"

using namespace mnslp::msg;


class MsgSequenceNumberTest : public GenericObjectTest {

	CPPUNIT_TEST_SUITE( MsgSequenceNumberTest );

	MNSLP_OBJECT_DEFAULT_TESTS();
	CPPUNIT_TEST( testBasics );

	CPPUNIT_TEST_SUITE_END();

  public:
	virtual mnslp_object *createInstance1() const {
		return new msg_sequence_number();
	}

	virtual mnslp_object *createInstance2() const {
		return new msg_sequence_number(15);
	}

	virtual void mutate1(mnslp_object *o) const {
		msg_sequence_number *ic = dynamic_cast<msg_sequence_number *>(o);
		ic->set_value(14);
	}

	void testBasics() {
		msg_sequence_number msn(17);

		CPPUNIT_ASSERT( msn.get_value() == 17 );
		msg_sequence_number msn2 = msn;
		CPPUNIT_ASSERT( msn2.get_value() == 17 );
		msn2.set_value(18);
		CPPUNIT_ASSERT( msn2.get_value() == 18 );
		
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( MsgSequenceNumberTest );

// EOF
