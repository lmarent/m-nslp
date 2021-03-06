/*
 * Test the mnslp_configure class.
 *
 * $Id: mnslp_configure.cpp 1730 2014-11-05 12:57:33Z  $
 * $HeadURL: https://./test/mnslp_configure.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "network_message.h"

#include "msg/mnslp_response.h"
#include "msg/mnslp_object.h"

#include "msg/mnslp_ie.h"
#include "msg/mnslp_msg.h"
#include "msg/mnslp_ipfix_message.h"
#include "msg/mnslp_ipfix_field.h"
#include "msg/mnslp_ipfix_data_record.h"




using namespace mnslp::msg;


class MnslpResponseTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( MnslpResponseTest );

	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testComparison );
	CPPUNIT_TEST( testCopying );
	CPPUNIT_TEST( testManager01 );  // Message without ipfix object
	CPPUNIT_TEST( testManager02 );  // Message with ipfix object

	CPPUNIT_TEST_SUITE_END();

  public:
	void testBasics();
	void testComparison();
	void testCopying();
	void testManager01();
	void testManager02();
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( MnslpResponseTest );


void MnslpResponseTest::testBasics() {
	mnslp_response c1;

	c1.set_session_lifetime(27);
	CPPUNIT_ASSERT( c1.get_session_lifetime() == 27 );

	c1.set_msg_sequence_number(42000);
	CPPUNIT_ASSERT( c1.get_msg_sequence_number() == 42000 );

	c1.set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);

	CPPUNIT_ASSERT( c1.get_severity_class() == information_code::sc_success );
	CPPUNIT_ASSERT( c1.get_response_code() == information_code::suc_successfully_processed );
	CPPUNIT_ASSERT( c1.get_response_object_type() == information_code::obj_none );

}


void MnslpResponseTest::testComparison() {
	mnslp_response *c1 = new mnslp_response();
	mnslp_response *c2 = new mnslp_response();
	c2->set_msg_sequence_number(7777);

	CPPUNIT_ASSERT( *c1 == *c1 );
	CPPUNIT_ASSERT( *c2 == *c2 );
	CPPUNIT_ASSERT( *c1 != *c2 );
	CPPUNIT_ASSERT( *c2 != *c1 );

	c1->set_msg_sequence_number(7777);
	CPPUNIT_ASSERT( *c1 == *c2 );
	CPPUNIT_ASSERT( *c2 == *c1 );

	delete c2;
	delete c1;
}

void MnslpResponseTest::testCopying() {
	mnslp_response *c1 = new mnslp_response();

	c1->set_session_lifetime(27);
	CPPUNIT_ASSERT( c1->get_session_lifetime() == 27 );

	c1->set_msg_sequence_number(42000);
	CPPUNIT_ASSERT( c1->get_msg_sequence_number() == 42000 );

	c1->set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);


	mnslp_response *c2 = c1->copy();
	CPPUNIT_ASSERT( c1 != c2 );
	CPPUNIT_ASSERT( *c1 == *c2 );

	delete c2;
	delete c1;
}


void MnslpResponseTest::testManager01() {
	MNSLP_IEManager::clear();
	MNSLP_IEManager::register_known_ies();
	MNSLP_IEManager *mgr = MNSLP_IEManager::instance();
	
    /* 
     * Builds the response message without any ipfix message.
     */
	mnslp_response *m1 = new mnslp_response();
	m1->set_session_lifetime(30);
	m1->set_msg_sequence_number(47);
    
    
	NetMsg msg( m1->get_serialized_size(IE::protocol_v1) );
	uint32 bytes_written;
	m1->serialize(msg, IE::protocol_v1, bytes_written);

	/*
	 * Now read the serialized object.
	 */
	
	std::cout << "----------------------------------------bytes written:" << m1->get_serialized_size(IE::protocol_v1) << std::endl;
	
	msg.set_pos(0);
	IEErrorList errlist;
	uint32 num_read;

	IE *ie = mgr->deserialize(msg, cat_mnslp_msg, IE::protocol_v1, errlist,
			num_read, false);
			
	std::cout << "--------------------------------------- bytes read:" << num_read << std::endl;	
	CPPUNIT_ASSERT( ie != NULL );
	CPPUNIT_ASSERT( errlist.is_empty() );
	CPPUNIT_ASSERT( num_read == ie->get_serialized_size(IE::protocol_v1) );
	CPPUNIT_ASSERT( ie != NULL );
    CPPUNIT_ASSERT( *m1 == *ie );
    
	delete m1;
	delete ie;
	mgr->clear();
    
}


void MnslpResponseTest::testManager02() {
	MNSLP_IEManager::clear();
	MNSLP_IEManager::register_known_ies();
	MNSLP_IEManager *mgr = MNSLP_IEManager::instance();


    /* 
     * Builds the ipfix message
     */
	int sourceid = 12345;
	mnslp_ipfix_message *mess;
	uint16_t templatedataid = 0;
	uint64_t octdel = 100;
	uint64_t octdel2 = 200;
	uint64_t packdel = 300;
	uint64_t packdel2 = 400;
	
	int nfields = 3;
	uint8_t   buf[5]  = { 1, 2, 3, 4 };
	uint8_t   buf2[5]  = { 5, 6, 7, 8 };

    mess = new mnslp_ipfix_message(sourceid, IPFIX_VERSION);	
	mess->delete_all_templates();

	templatedataid = mess->new_data_template( nfields );
	mess->add_field(templatedataid, 0, IPFIX_FT_SOURCEIPV4ADDRESS, 4);
	mess->add_field(templatedataid, 0, IPFIX_FT_OCTETDELTACOUNT, 8);
	mess->add_field(templatedataid, 0, IPFIX_FT_PACKETDELTACOUNT, 8);

	mnslp_ipfix_field field1 = mess->get_field_definition( 0, IPFIX_FT_SOURCEIPV4ADDRESS );
	mnslp_ipfix_value_field fvalue1 = field1.get_ipfix_value_field( (uint8_t *) buf, 4);
	mnslp_ipfix_value_field fvalue1a = field1.get_ipfix_value_field( (uint8_t *) buf2, 4);

	mnslp_ipfix_field field2 = mess->get_field_definition( 0, IPFIX_FT_OCTETDELTACOUNT );
	mnslp_ipfix_value_field fvalue2 = field2.get_ipfix_value_field( octdel);
	mnslp_ipfix_value_field fvalue2a = field2.get_ipfix_value_field( octdel2);

	mnslp_ipfix_field field3 = mess->get_field_definition( 0, IPFIX_FT_PACKETDELTACOUNT );
	mnslp_ipfix_value_field fvalue3 = field3.get_ipfix_value_field( packdel );
	mnslp_ipfix_value_field fvalue3a = field3.get_ipfix_value_field( packdel2 );

	mnslp_ipfix_data_record data(templatedataid);
	data.insert_field(0, IPFIX_FT_SOURCEIPV4ADDRESS, fvalue1);
	data.insert_field(0, IPFIX_FT_OCTETDELTACOUNT, fvalue2);
	data.insert_field(0, IPFIX_FT_PACKETDELTACOUNT, fvalue3);
	mess->include_data(templatedataid, data);

	mnslp_ipfix_data_record data2(templatedataid);
	data2.insert_field(0, IPFIX_FT_SOURCEIPV4ADDRESS, fvalue1a);
	data2.insert_field(0, IPFIX_FT_OCTETDELTACOUNT, fvalue2a);
	data2.insert_field(0, IPFIX_FT_PACKETDELTACOUNT, fvalue3a);
	mess->include_data(templatedataid, data2);
	mess->output();

    /* 
     * Builds the refresh message without any ipfix message.
     */
	mnslp_response *m1 = new mnslp_response();
	m1->set_session_lifetime(30);
	m1->set_msg_sequence_number(47);
	m1->set_ipfix_message(mess);
    
    
	NetMsg msg( m1->get_serialized_size(IE::protocol_v1) );
	uint32 bytes_written;
	m1->serialize(msg, IE::protocol_v1, bytes_written);

	/*
	 * Now read the serialized object.
	 */
	
	std::cout << "----------------------------------------bytes written:" << m1->get_serialized_size(IE::protocol_v1) << std::endl;
	
	msg.set_pos(0);
	IEErrorList errlist;
	uint32 num_read;

	IE *ie = mgr->deserialize(msg, cat_mnslp_msg, IE::protocol_v1, errlist,
			num_read, false);
			
	std::cout << "--------------------------------------- bytes read:" << num_read << std::endl;	
	CPPUNIT_ASSERT( ie != NULL );
	CPPUNIT_ASSERT( errlist.is_empty() );
	CPPUNIT_ASSERT( num_read == ie->get_serialized_size(IE::protocol_v1) );
	CPPUNIT_ASSERT( ie != NULL );
    CPPUNIT_ASSERT( *m1 == *ie );
    
	delete m1;
	delete ie;
	mgr->clear();
    
}


// EOF
