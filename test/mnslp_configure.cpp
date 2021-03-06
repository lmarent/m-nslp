/*
 * Test the mnslp_configure class.
 *
 * $Id: mnslp_configure.cpp 1730 2014-11-05 12:57:33Z  $
 * $HeadURL: https://./test/mnslp_configure.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "network_message.h"

#include "msg/mnslp_configure.h"
#include "msg/mnslp_object.h"

#include "msg/mnslp_ie.h"
#include "msg/mnslp_msg.h"
#include "msg/mnslp_ipfix_message.h"
#include "msg/mnslp_ipfix_field.h"
#include "msg/mnslp_ipfix_data_record.h"




using namespace mnslp::msg;


class MNslpConfigureTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( MNslpConfigureTest );

	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testComparison );
	CPPUNIT_TEST( testCopying );
	CPPUNIT_TEST( testManager01 ); // Only one ipfix message included
	CPPUNIT_TEST( testManager02 ); // More than one ipfix message included
	CPPUNIT_TEST( testManager03 );

	CPPUNIT_TEST_SUITE_END();

  public:
	void testBasics();
	void testComparison();
	void testCopying();
	void testManager01();
	void testManager02();
	void testManager03();
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( MNslpConfigureTest );


void MNslpConfigureTest::testBasics() {
	mnslp_configure c1;

	c1.set_session_lifetime(27);
	CPPUNIT_ASSERT( c1.get_session_lifetime() == 27 );

	c1.set_msg_sequence_number(42000);
	CPPUNIT_ASSERT( c1.get_msg_sequence_number() == 42000 );

	c1.set_selection_metering_entities(selection_metering_entities::sme_all);
	CPPUNIT_ASSERT( c1.get_selection_metering_entities() == selection_metering_entities::sme_all );

	c1.set_message_hop_count(20);
	CPPUNIT_ASSERT( c1.get_message_hop_count() == 20 );

}


void MNslpConfigureTest::testComparison() {
	mnslp_configure *c1 = new mnslp_configure();
	mnslp_configure *c2 = new mnslp_configure();
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

void MNslpConfigureTest::testCopying() {
	mnslp_configure *c1 = new mnslp_configure();

	mnslp_configure *c2 = c1->copy();
	CPPUNIT_ASSERT( c1 != c2 );
	CPPUNIT_ASSERT( *c1 == *c2 );

	delete c2;
	delete c1;
}


void MNslpConfigureTest::testManager01() {
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
     * Builds the configure message
     */
	mnslp_configure *m1 = new mnslp_configure();
	m1->set_session_lifetime(30);
	m1->set_msg_sequence_number(47);
	m1->set_selection_metering_entities(selection_metering_entities::sme_all);
	m1->set_message_hop_count(20);
    m1->set_mspec_object(mess);
    
    
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
    CPPUNIT_ASSERT( *ie == *m1);

	std::cout << "--------------------------------------- finish:" << num_read << std::endl;	
    
	delete m1;
	delete ie;
	mgr->clear();
    
}


void MNslpConfigureTest::testManager02() {
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
	
	mnslp_ipfix_message *mess2 = new mnslp_ipfix_message(*mess);	
	
       /* 
        * Builds the configure message
        */
	mnslp_configure *m1 = new mnslp_configure();
	m1->set_session_lifetime(30);
	m1->set_msg_sequence_number(47);
	m1->set_selection_metering_entities(selection_metering_entities::sme_all);
	m1->set_message_hop_count(20);
	m1->set_mspec_object(mess);
	m1->set_mspec_object(mess2);
    
    
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
	//if (mess != NULL)
	//	delete mess;
    
}

void MNslpConfigureTest::testManager03() {
	MNSLP_IEManager::clear();
	MNSLP_IEManager::register_known_ies();
	MNSLP_IEManager *mgr = MNSLP_IEManager::instance();

	mnslp_ipfix_message *mess1;

	// Variables for exporting
    uint16_t templatedataid = 0;
	int nExportfields = 1;
	
        // Variables for filtering
	uint16_t templateoptionid = 0;
	int nFilterfields = 7;
	uint8_t buf[5]  = { 192, 168, 1, 11 };
	uint16_t destination_port = 80;
	uint8_t protocol = 8;
	uint64_t flow_indicator = 0; 
	uint8_t ipfix_ver = IPFIX_VERSION;
	int sourceid = 0x00000000;
	
	mess1 = new mnslp_ipfix_message(sourceid, IPFIX_VERSION);
	std::cout << "output performed 1" << std::endl;

    // Add export fields
	templatedataid = mess1->new_data_template( nExportfields );
	// In this case all filter fields should be included for reporting; 
	// however, because netmate does not export in ipfix we cannot used it.
	mess1->add_field(templatedataid, 0, IPFIX_FT_OCTETDELTACOUNT, 8);

	// Add filter fields
	templateoptionid = mess1->new_option_template( nFilterfields );
	mess1->add_field(templateoptionid, 0, IPFIX_FT_FLOWKEYINDICATOR, 8);
	mess1->add_field(templateoptionid, 0, IPFIX_FT_COLLECTORIPV4ADDRESS, 4);
	mess1->add_field(templateoptionid, 0, IPFIX_FT_COLLECTORPROTOCOLVERSION, 1);	
	mess1->add_field(templateoptionid, 0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL, 1);
	mess1->add_field(templateoptionid, 0, IPFIX_FT_SOURCEIPV4ADDRESS, 4);
	mess1->add_field(templateoptionid, 0, IPFIX_FT_TCPDESTINATIONPORT, 2);
	mess1->add_field(templateoptionid, 0, IPFIX_FT_PROTOCOLIDENTIFIER, 1);

	std::cout << "output performed 2" << std::endl;
	
	mnslp_ipfix_field field1 = mess1->get_field_definition( 0, IPFIX_FT_FLOWKEYINDICATOR );
	mnslp_ipfix_value_field fvalue1 = field1.get_ipfix_value_field(flow_indicator);
	
	mnslp_ipfix_field field2 = mess1->get_field_definition( 0, IPFIX_FT_COLLECTORIPV4ADDRESS );
	mnslp_ipfix_value_field fvalue2 = field2.get_ipfix_value_field( (uint8_t *) buf, 4);

	mnslp_ipfix_field field3 = mess1->get_field_definition( 0, IPFIX_FT_COLLECTORPROTOCOLVERSION );
	mnslp_ipfix_value_field fvalue3 = field3.get_ipfix_value_field(ipfix_ver);
	
	mnslp_ipfix_field field4 = mess1->get_field_definition( 0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL );
	mnslp_ipfix_value_field fvalue4 = field4.get_ipfix_value_field( protocol);

	// In this case just the last three fields are used for filter purposes.
	std::cout << "output performed 3" << std::endl;
	
	mnslp_ipfix_field field5 = mess1->get_field_definition( 0, IPFIX_FT_SOURCEIPV4ADDRESS );
	mnslp_ipfix_value_field fvalue5 = field5.get_ipfix_value_field( (uint8_t *) buf, 4);

	mnslp_ipfix_field field6 = mess1->get_field_definition( 0, IPFIX_FT_TCPDESTINATIONPORT );
	mnslp_ipfix_value_field fvalue6 = field6.get_ipfix_value_field(destination_port);

	mnslp_ipfix_field field7 = mess1->get_field_definition( 0, IPFIX_FT_PROTOCOLIDENTIFIER );
	mnslp_ipfix_value_field fvalue7 = field7.get_ipfix_value_field( protocol );
	
	mnslp_ipfix_data_record data(templateoptionid);
	data.insert_field(0, IPFIX_FT_FLOWKEYINDICATOR, fvalue1);
	data.insert_field(0, IPFIX_FT_COLLECTORIPV4ADDRESS, fvalue2);
	data.insert_field(0, IPFIX_FT_COLLECTORPROTOCOLVERSION, fvalue3);
	data.insert_field(0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL, fvalue4);
	data.insert_field(0, IPFIX_FT_SOURCEIPV4ADDRESS, fvalue5);
	data.insert_field(0, IPFIX_FT_TCPDESTINATIONPORT, fvalue6);
	data.insert_field(0, IPFIX_FT_PROTOCOLIDENTIFIER, fvalue7);
	mess1->include_data(templateoptionid, data);
	
	mess1->output();	
	std::cout <<  "serialize size" << mess1->get_serialized_size(IE::protocol_v1) << std::endl;
	
	std::cout << "output performed" << std::endl;
    /* 
     * Builds the configure message
     */
	mnslp_configure *m1 = new mnslp_configure();
	m1->set_session_lifetime(30);
	m1->set_msg_sequence_number(47);
	m1->set_selection_metering_entities(selection_metering_entities::sme_all);
	m1->set_message_hop_count(20);
    m1->set_mspec_object(mess1);   
    
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
	//if (mess != NULL)
	//	delete mess;
    
}



// EOF
