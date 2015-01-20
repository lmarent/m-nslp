/*
 * A generic test case for MNSLP objects.
 *
 * $Id: generic_object_test.cpp 1733 2014-11-05 14:55:00 $
 * $HeadURL: https://./test/generic_object_test.cpp $
 */
#include "generic_object_test.h"

using namespace mnslp::msg;


// Test if the child classes implemented the abstract methods correctly.
void GenericObjectTest::testMyself() {
	mnslp_object *p1 = createInstance1();
	mnslp_object *p2 = createInstance2();

	CPPUNIT_ASSERT( *p1 != *p2 );

	mnslp_object *p3 = createInstance1();

	CPPUNIT_ASSERT( *p1 == *p3 );

	mutate1(p3);

	CPPUNIT_ASSERT( *p1 != *p3 );

	delete p1;
	delete p2;
	delete p3;
}


void GenericObjectTest::testComparison() {
	mnslp_object *p1 = createInstance1();
	mnslp_object *p2 = createInstance1();

	CPPUNIT_ASSERT( p1 != p2 );
	CPPUNIT_ASSERT( *p1 == *p2 && *p2 == *p1 );

	mutate1(p1);

	CPPUNIT_ASSERT( *p1 != *p2 && *p2 != *p1 );

	delete p1;
	delete p2;
}


void GenericObjectTest::testCopying() {
	mnslp_object *p1 = createInstance1();
	p1->set_treatment(mnslp_object::tr_optional);

	mnslp_object *p2 = p1->copy();

	CPPUNIT_ASSERT( *p1 == *p2 );
	CPPUNIT_ASSERT( typeid(p1) == typeid(p2) );

	p1->set_treatment(mnslp_object::tr_mandatory);

	CPPUNIT_ASSERT( *p1 != *p2 );

	delete p1;
	delete p2;
}


void GenericObjectTest::testSerialize() {
	/*
	 * Error: Object type larger than 12 bit.
	 */
	mnslp_object *p1 = createInstance1();
	p1->set_object_type(0xFABC);

	CPPUNIT_ASSERT( p1->check() == false );

	NetMsg msg1( p1->get_serialized_size(IE::protocol_v1) );
	uint32 bytes_written1 = 42;

	CPPUNIT_ASSERT_THROW(
		p1->serialize(msg1, IE::protocol_v1, bytes_written1), IEError);

	// Note: bytes_written1 is undefined
	delete p1;


	/*
	 * Error: Not enough space in NetMsg.
	 */
	mnslp_object *p2 = createInstance2();

	NetMsg msg2(5);
	uint32 bytes_written2 = 42;

	CPPUNIT_ASSERT_THROW(
		p2->serialize(msg2, IE::protocol_v1, bytes_written2), IEError);

	// Note: bytes_written2 is undefined
	delete p2;
}


void GenericObjectTest::testReadWrite() {
	mnslp_object *p1 = createInstance2();
	p1->set_treatment(mnslp_object::tr_optional);

	mnslp_object *p2 = createInstance1();

	tryReadWrite(p1, p2);

	delete p2;
	delete p1;
}


/**
 * Test a serialize/deserialize cycle.
 *
 * Serialize obj into a NetMsg, deserialize the NetMsg into the "blank"
 * object and compare them.
 */
void GenericObjectTest::tryReadWrite(mnslp_object *obj,
		mnslp_object *blank) {

	CPPUNIT_ASSERT( obj != blank );

	const IE::coding_t CODING = IE::protocol_v1;

	NetMsg msg( obj->get_serialized_size(CODING) );
	uint32 bytes_written;
	obj->serialize(msg, CODING, bytes_written);

	
	//std::cout << *obj << "\n";
	//std::cout << msg.hexdump(cout) << '\n';

	/*
	 * Now read the serialized object.
	 */
	msg.set_pos(0);
	IEErrorList errlist;
	uint32 num_read;

	IE *ie = blank->deserialize(msg, CODING, errlist, num_read, false);
	//std::cout << *ie << "\n";

	CPPUNIT_ASSERT( num_read == blank->get_serialized_size(CODING) );
	CPPUNIT_ASSERT( ie != NULL );

	CPPUNIT_ASSERT( *obj == *blank );
}


// EOF
