/*
 * Test the Information Code Object
 *
 * $Id: information_code.cpp 1743 2014-11-05 14:47:10  $
 * $HeadURL: https://./test/information_code.cpp $
 */
#include "generic_object_test.h"
#include "msg/selection_metering_entities.h"
#include "msg/mnslp_msg.h"

using namespace mnslp::msg;


class SelectionMeteringEntitiesTest : public GenericObjectTest {

	CPPUNIT_TEST_SUITE( SelectionMeteringEntitiesTest );

	MNSLP_OBJECT_DEFAULT_TESTS();
	CPPUNIT_TEST( testBasics );

	CPPUNIT_TEST_SUITE_END();

  public:
	virtual mnslp_object *createInstance1() const {
		return new selection_metering_entities();
	}

	virtual mnslp_object *createInstance2() const {
		return new selection_metering_entities(selection_metering_entities::sme_any);
	}

	virtual void mutate1(mnslp_object *o) const {
		selection_metering_entities *sme = dynamic_cast<selection_metering_entities *>(o);
		sme->set_value(selection_metering_entities::sme_first);
	}

	void testBasics() {
		selection_metering_entities selectionme(selection_metering_entities::sme_all);
		CPPUNIT_ASSERT( selectionme.get_value() == selection_metering_entities::sme_all );
		
		selection_metering_entities selectionme2 = selectionme;
		selectionme2.set_value(selection_metering_entities::sme_first);
		CPPUNIT_ASSERT( selectionme2.get_value() == selection_metering_entities::sme_first ); 
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( SelectionMeteringEntitiesTest );

// EOF
