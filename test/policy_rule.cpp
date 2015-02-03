/*
 * Test the policy_rule class.
 *
 * $Id: policy_rule.cpp 1711 2015-01-26 14:50:00 amarentes $
 * $HeadURL: https://./test/policy_rule.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <iostream>
#include <list>

#include "policy_rule.h"
#include "msg/mnslp_ipfix_message.h"
#include "msg/mnslp_ipfix_field.h"
#include "ipfix_def_FOKUS.h"

using namespace mnslp;


class PolicyRuleTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in mnslp_ipfix_fields.
 */
class policy_rule_test : public mt_policy_rule
{
  public:
  
	policy_rule_test(): mt_policy_rule() { }

	friend class PolicyRuleTest;
};



class PolicyRuleTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( PolicyRuleTest );

	CPPUNIT_TEST( general_test );
	CPPUNIT_TEST_EXCEPTION( testFieldNotFoundException, msg::mnslp_ipfix_bad_argument );
	CPPUNIT_TEST_SUITE_END();

  public:
  
	void setUp();
	void general_test(); 
	void add_export_fields1();
	void add_export_fields2();
	void add_export_fields3();
	void testFieldNotFoundException();
	void add_ipfix_vendor_fields(msg::mnslp_ipfix_message *mess);
	void add_configuration_fields(msg::mnslp_ipfix_message *mess);
	void tearDown();

  private:
    
	msg::mnslp_ipfix_message *mess1;
	msg::mnslp_ipfix_message *mess2;
	msg::mnslp_ipfix_message *mess3;
	
	policy_rule_test * rule1;
    

};

CPPUNIT_TEST_SUITE_REGISTRATION( PolicyRuleTest );

void PolicyRuleTest::setUp() {

	int sourceid = 0x00000000;
	mess1 = new msg::mnslp_ipfix_message(sourceid, IPFIX_VERSION, true);
	mess2 = new msg::mnslp_ipfix_message(sourceid, IPFIX_VERSION, true);
	mess3 = new msg::mnslp_ipfix_message(sourceid, IPFIX_VERSION, true);
		
	add_ipfix_vendor_fields(mess1);
	add_ipfix_vendor_fields(mess2);
	add_ipfix_vendor_fields(mess3);
		
	add_export_fields1();
	add_export_fields2();
	add_export_fields3();
	
	add_configuration_fields(mess1);
	add_configuration_fields(mess2);
	add_configuration_fields(mess3);
	
}

void PolicyRuleTest::add_ipfix_vendor_fields(msg::mnslp_ipfix_message *mess)
{

	
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_REVOCTETDELTACOUNT,    8,  IPFIX_CODING_UINT, 
		  "revOctetDeltaCount",  "FOKUS reverse delta octet count" );
		 
	mess->add_vendor_information_element(  IPFIX_ENO_FOKUS,  IPFIX_FT_REVPACKETDELTACOUNT,   8,  IPFIX_CODING_UINT, 
	   "revPacketDeltaCount",  "FOKUS reverse delta packet count" );	  
	
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_RTTMEAN_USEC,          8,  IPFIX_CODING_UINT, 
	  "rttmean_usec",  "FOKUS mean rtt in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_RTTMIN_USEC,           8,  IPFIX_CODING_UINT, 
	  "rttmin_usec",  "FOKUS minimum rtt in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_RTTMAX_USEC,           8,  IPFIX_CODING_UINT, 
	  "rttmax_usec",  "FOKUS maximum rtt in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_IDENT,             65535,  IPFIX_CODING_STRING, 
	  "ident",  "FOKUS ident" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_LOSTPACKETS,           4,  IPFIX_CODING_INT, 
	  "lostPackets",  "FOKUS lost packets" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDVAR_USEC,           4,  IPFIX_CODING_INT, 
	  "owdvar_usec",  "FOKUS delay variation in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDVARMEAN_USEC,       4,  IPFIX_CODING_INT, 
	  "owdvarmean_usec",  "FOKUS mean dvar in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDVARMIN_USEC,        4,  IPFIX_CODING_INT, 
	  "owdvarmin_usec",  "FOKUS minimum dvar in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDVARMAX_USEC,        4,  IPFIX_CODING_INT, 
	  "owdvarmax_usec",  "FOKUS maximum dvar in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDSD_USEC,            8,  IPFIX_CODING_UINT, 
	  "owdsd_usec",  "FOKUS owd standard deviation" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWD_USEC,              8,  IPFIX_CODING_UINT, 
	  "owd_usec",  "FOKUS one-way-delay in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDMEAN_USEC,          8,  IPFIX_CODING_UINT, 
	  "owdmean_usec",  "FOKUS mean owd in us"  );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDMIN_USEC,           8,  IPFIX_CODING_UINT, 
	  "owdmin_usec",  "FOKUS minimum owd in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDMAX_USEC,           8,  IPFIX_CODING_UINT, 
	  "owdmax_usec",  "FOKUS maximum owd in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TASKID,                4,  IPFIX_CODING_UINT, 
	  "taskId",  "FOKUS task id" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TSTAMP_SEC,            4,  IPFIX_CODING_INT, 
	  "tstamp_sec",  "FOKUS tstamp" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TSTAMP_NSEC,           4,  IPFIX_CODING_INT, 
	  "tstamp_nsec",  "FOKUS tstamp nanosecond fraction" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PKTLENGTH,             4,  IPFIX_CODING_INT, 
	  "pktLength",  "FOKUS packet length" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PKTID,                 4,  IPFIX_CODING_UINT, 
	  "pktId",  "FOKUS packet id" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_STARTTIME,             4,  IPFIX_CODING_INT, 
	  "startTime",  "FOKUS interval start" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_ENDTIME,               4,  IPFIX_CODING_INT, 
	  "endTime",  "FOKUS interval end" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_RTT_USEC,              8,  IPFIX_CODING_UINT, 
	  "rtt_usec",  "FOKUS rtt in us" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_FLOWCREATIONTIMEUSEC,  4,  IPFIX_CODING_INT, 
	  "flowCreationTimeUsec",  "FOKUS flow start usec fraction" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_FLOWENDTIMEUSEC,       4,  IPFIX_CODING_INT, 
	  "flowEndTimeUsec",  "FOKUS flow end usec fraction" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TC_PACKETS,            4,  IPFIX_CODING_UINT, 
	  "tcPackets",  "DAIDALOS Packets seen" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TC_BYTES,              4,  IPFIX_CODING_UINT, 
	  "tcBytes",  "DAIDALOS Bytes seen" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TC_RATE_BPS,           4,  IPFIX_CODING_INT, 
	  "tcRate_bps",  "DAIDALOS Current bits/s (rate estimator)" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TC_RATE_PPS,           4,  IPFIX_CODING_INT, 
	  "tcRrate_pps",  "DAIDALOS Current packet/s (rate estimator)" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TC_QLEN,               4,  IPFIX_CODING_INT, 
	  "tc_qlen",  "DAIDALOS Queue length" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TC_BACKLOG,            4,  IPFIX_CODING_INT, 
	  "tcbacklog",  "DAIDALOS Backlog length" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TC_DROPS,              4,  IPFIX_CODING_INT, 
	  "tcDrops",  "DAIDALOS Packets dropped" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TC_REQUEUES,           4,  IPFIX_CODING_INT, 
	  "tcRequeues",  "DAIDALOS Number of requeues" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_TC_OVERLIMITS,         4,  IPFIX_CODING_INT, 
	  "tcOverlimits",  "DAIDALOS Number of overlimits" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDVARMEAN_NSEC,       4,  IPFIX_CODING_INT, 
	  "owdvarmean_nsec",  "FOKUS mean owd variace in ns" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDVARMIN_NSEC,        4,  IPFIX_CODING_INT, 
	  "owdvarmin_nsec",  "FOKUS minimum owd variance in ns" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OWDVARMAX_NSEC,        4,  IPFIX_CODING_INT, 
	  "owdvarmax_nsec",  "FOKUS maximum ow variance in ns" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_SOURCEIPV4FANOUT,      4,  IPFIX_CODING_UINT, 
	 "sourceIPv4FanOut",  "FOKUS IPv4 fanout" );	  
	 
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_DESTINATIONIPV4FANIN,  4,  IPFIX_CODING_UINT, 
	 "destinationIPv4FanIn",  "FOKUS IPv4 fanin" );	  
	 
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PACKETARRIVALMEAN,     8,  IPFIX_CODING_UINT, 
	 "packetArrivalMean",  "FOKUS IPv4 interpacket arrival time. mean in ms" );	  
	 
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PACKETARRIVALVAR,      8,  IPFIX_CODING_UINT, 
	 "packetArrivalVar",  "FOKUS IPv4 interpacket arrival time.  variance in ms" );	  
	 
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PR_SESSIONID, 	4,  IPFIX_CODING_UINT, 
	  "sessionId",  "PRISM Session ID" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PR_TRANSACTIONID,  4,  IPFIX_CODING_UINT, 
	  "transactionId",  "PRISM Transaction ID" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PR_AES128ENCRYPTEDDATA,  65535,  IPFIX_CODING_STRING, 
	  "encryptedData128",  "PRISM encrypted data 128bits" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PR_AES256ENCRYPTEDDATA,  65535,  IPFIX_CODING_STRING, 
	  "encryptedData128",  "PRISM encrypted data 256bits" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PR_DECRYPTIONKEY,     65535,  IPFIX_CODING_STRING, 
	  "decryptionKey",  "PRISM decryption key" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PR_AES128KEYSHARE,       16,  IPFIX_CODING_STRING, 
	  "keyShare128",  "PRISM key share 128bits" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PR_AES256KEYSHARE,       36,  IPFIX_CODING_STRING, 
	  "keyShare256",  "PRISM key share 256bits" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PR_KEYSHAREADP,          32,  IPFIX_CODING_STRING, 
	  "keyShareAdp",  "PRISM key share ADP" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PR_KEYSHAREINITVECTOR,   16,  IPFIX_CODING_STRING, 
	  "cryptoInitVector",  "PRISM crypto init vector" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_SYSTEM_CPU_IDLE,    4,  IPFIX_CODING_FLOAT, 
	  "sysCpuIdle",   "PT system CPU idle %" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_SYSTEM_MEM_FREE,    8,  IPFIX_CODING_UINT, 
	   "sysMemFree",   "PT system free memory in kilobytes" );	  
	 
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_PROCESS_CPU_USER,   4,  IPFIX_CODING_FLOAT, 
	  "procCpuUser",  "PT percentage of CPU used in user level (application)" );	  
	  	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_PROCESS_CPU_SYS,    4,  IPFIX_CODING_FLOAT, 
	  "procCpuSys",   "PT percentage of CPU used in system level (kernel)" );	  
	
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_PROCESS_MEM_VZS,    8,  IPFIX_CODING_UINT, 
	   "procMemVzs",   "PT the process virtual memory used in bytes" );	  
	   
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_PROCESS_MEM_RSS,    8,  IPFIX_CODING_UINT, 
	   "procMemRss",   "PT the process resident set size in bytes" );	  
	   
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_PCAPSTAT_RECV,      4,  IPFIX_CODING_UINT, 
	   "pcapRecv",     "PT number of packets received by pcap" );	  
	   
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_PCAPSTAT_DROP,      4,  IPFIX_CODING_UINT, 
	   "pcapDrop",     "PT number of packets dropped by pcap" );	  
	   
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_MESSAGE_ID,         4,  IPFIX_CODING_UINT, 
	   "msgId",        "PT message id" );	  
	   
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_MESSAGE_VALUE,      4,  IPFIX_CODING_UINT, 
	   "msgValue",      "PT generic value" );	  
	   
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_MESSAGE,       65535,  IPFIX_CODING_STRING, 
	  "msg",      "generic message" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_INTERFACE_NAME,              65535,  IPFIX_CODING_STRING,
	  "interfaceName",         "PT Interface Name" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_PT_INTERFACE_DESCRIPTION,       65535,  IPFIX_CODING_STRING, 
	  "interfaceDescripton",   "PT Interface Description" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_SYNC_QUEUE_FILL_LEVEL,              4,  IPFIX_CODING_FLOAT, 
	  "queueFillLevel",  "SYNC QueueFillLevel" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_SYNC_BOTTLENECK,                    4,  IPFIX_CODING_FLOAT, 
	  "bottleneck",      "SYNC Bottleneck" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_SYNC_FREQ,                          4,  IPFIX_CODING_FLOAT, 
	  "freq",            "SYNC Frequency" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_ORsignalBandwidth,  4,  IPFIX_CODING_UINT, 
	  "ORsignalBandwidth",  "signal bandwidth" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_ORsignalPower,  2,  IPFIX_CODING_UINT, 
	  "ORsignalPower",  "ERIP" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_ORsymbolRate,  2,  IPFIX_CODING_UINT, 
	  "ORsymbolRate",  "symbol rate" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_ORmodulationOrder,  1,  IPFIX_CODING_UINT, 
	  "ORmodulationOrder",  "number of levels" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_ORrolloffFactor,  2,  IPFIX_CODING_UINT, 
	  "ORrolloffFactor",  "roll of factor" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_sensing_value,  2,  IPFIX_CODING_UINT, 
	  "sensing_value",  "Cost function output" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_sensing_threshold,  2,  IPFIX_CODING_UINT, 
	  "sensing_threshold",  "Decision threshold" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OR_terminal_id,  1,  IPFIX_CODING_UINT, 
	  "OR_terminal_id",  "terminal identifier" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_OR_terminal_id_list,  65535,  IPFIX_CODING_STRING, 
	  "OR_terminal_id_list",  "terminal identifier list" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_Infrastructure_network_id,  1,  IPFIX_CODING_UINT, 
	  "Infrastructure_network_id",  "network identifier" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_Spectral_allocation_vector,  1,  IPFIX_CODING_UINT, 
	  "Spectral_allocation_vector",  "binary vector to indicate whether a band is free 1 bit 0 or not 1 bit 1" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_Spectral_allocation_profile,  2,  IPFIX_CODING_UINT, 
	  "Spectral_allocation_profile",  "received power spectral density vs. frequency to indicate spectral activity in the band of interest (8-16 bits per discrete frequency value)" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_Center_frequency,  2,  IPFIX_CODING_UINT, 
	  "Center_frequency",  "Center frequency of the sensed band" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_Bandwidth_of_CAP,  2,  IPFIX_CODING_UINT, 
	  "Bandwidth_of_CAP",  "Bandwidth of the spectral allocation profile" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_ORmodulation,  1,  IPFIX_CODING_UINT, 
	  "ORmodulation",  "CREST factor" );	  
    
}
 

void PolicyRuleTest::add_export_fields1()
 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess1->new_data_template( nfields );
	
	mess1->add_field(templatedataid, 0, IPFIX_FT_OCTETDELTACOUNT, 8);
	
}

void PolicyRuleTest::add_export_fields2() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess2->new_data_template( nfields );
	
	mess2->add_field(templatedataid, 0, IPFIX_FT_PACKETDELTACOUNT, 8);	
}

void PolicyRuleTest::add_export_fields3() 
{
	uint16_t templatedataid = 0;
	int nfields = 2;
	
	templatedataid = mess3->new_data_template( nfields );
	
	mess3->add_field(templatedataid, 0, IPFIX_FT_MINIMUMIPTOTALLENGTH, 8);	
	mess3->add_field(templatedataid, IPFIX_ENO_FOKUS, IPFIX_FT_OWDMIN_USEC, 8);	
}

void PolicyRuleTest::testFieldNotFoundException() 
{
	uint16_t templatedataid = 0;
	int nfields = 1;
	
	templatedataid = mess3->new_data_template( nfields );
	mess3->add_field(templatedataid, IPFIX_ENO_FOKUS, 1000, 8); 	
}


void PolicyRuleTest::add_configuration_fields(msg::mnslp_ipfix_message * mess) 
{

	uint16_t templateoptionid = 0;
	int nfields = 7;
	uint8_t buf[5]  = { 10, 0, 2, 15 };
	uint16_t destination_port = 80;
	uint8_t protocol = 8;
	uint64_t flow_indicator = 0; 
	uint8_t ipfix_ver = IPFIX_VERSION;
	
	// Add filter fields
	templateoptionid = mess->new_option_template( nfields );
	mess->add_field(templateoptionid, 0, IPFIX_FT_FLOWKEYINDICATOR, 8);
	mess->add_field(templateoptionid, 0, IPFIX_FT_COLLECTORIPV4ADDRESS, 4);
	mess->add_field(templateoptionid, 0, IPFIX_FT_COLLECTORPROTOCOLVERSION, 1);	
	mess->add_field(templateoptionid, 0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL, 1);
	mess->add_field(templateoptionid, 0, IPFIX_FT_SOURCEIPV4ADDRESS, 4);
	mess->add_field(templateoptionid, 0, IPFIX_FT_TCPDESTINATIONPORT, 2);
	mess->add_field(templateoptionid, 0, IPFIX_FT_PROTOCOLIDENTIFIER, 1);

	msg::mnslp_ipfix_field field1 = mess->get_field_definition( 0, IPFIX_FT_FLOWKEYINDICATOR );
	msg::mnslp_ipfix_value_field fvalue1 = field1.get_ipfix_value_field(flow_indicator);

	msg::mnslp_ipfix_field field2 = mess->get_field_definition( 0, IPFIX_FT_COLLECTORIPV4ADDRESS );
	msg::mnslp_ipfix_value_field fvalue2 = field2.get_ipfix_value_field( (uint8_t *) buf, 4);

	msg::mnslp_ipfix_field field3 = mess->get_field_definition( 0, IPFIX_FT_COLLECTORPROTOCOLVERSION );
	msg::mnslp_ipfix_value_field fvalue3 = field3.get_ipfix_value_field(ipfix_ver);
	
	msg::mnslp_ipfix_field field4 = mess->get_field_definition( 0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL );
	msg::mnslp_ipfix_value_field fvalue4 = field4.get_ipfix_value_field( protocol);

	// In this case just the last three fields are used for filter purposes.
	
	msg::mnslp_ipfix_field field5 = mess->get_field_definition( 0, IPFIX_FT_SOURCEIPV4ADDRESS );
	msg::mnslp_ipfix_value_field fvalue5 = field5.get_ipfix_value_field( (uint8_t *) buf, 4);

	msg::mnslp_ipfix_field field6 = mess->get_field_definition( 0, IPFIX_FT_TCPDESTINATIONPORT );
	msg::mnslp_ipfix_value_field fvalue6 = field6.get_ipfix_value_field(destination_port);

	msg::mnslp_ipfix_field field7 = mess->get_field_definition( 0, IPFIX_FT_PROTOCOLIDENTIFIER );
	msg::mnslp_ipfix_value_field fvalue7 = field7.get_ipfix_value_field( protocol );
	
	msg::mnslp_ipfix_data_record data(templateoptionid);
	data.insert_field(0, IPFIX_FT_FLOWKEYINDICATOR, fvalue1);
	data.insert_field(0, IPFIX_FT_COLLECTORIPV4ADDRESS, fvalue2);
	data.insert_field(0, IPFIX_FT_COLLECTORPROTOCOLVERSION, fvalue3);
	data.insert_field(0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL, fvalue4);
	data.insert_field(0, IPFIX_FT_SOURCEIPV4ADDRESS, fvalue5);
	data.insert_field(0, IPFIX_FT_TCPDESTINATIONPORT, fvalue6);
	data.insert_field(0, IPFIX_FT_PROTOCOLIDENTIFIER, fvalue7);
	mess->include_data(templateoptionid, data);
	
}

void PolicyRuleTest::tearDown() 
{
	
}

void PolicyRuleTest::general_test()
{
	
	mt_policy_rule rule1, rule4;
	
	mspec_rule_key key1 = rule1.set_object(mess1);
	std::string key1_a = key1.to_string();
	key1_a.append("_A");
	std::string key1_b = key1.to_string();
	key1_b.append("_B");
	std::string key1_c = key1.to_string();
	key1_c.append("_C");
	
	std::vector<std::string> rule_keys;
	rule_keys.push_back(key1_a);
	rule_keys.push_back(key1_b);
	rule_keys.push_back(key1_c);
	
	rule1.set_commands(key1, rule_keys);
	
	// Test default copy constructor
	mt_policy_rule rule2(rule1);
	
	CPPUNIT_ASSERT( rule1 == rule2 );
	
	mspec_rule_key key2 = rule2.set_object(mess2);
	
	CPPUNIT_ASSERT( rule1 != rule2 );
	
	mt_policy_rule *rule3 = rule2.copy();
	mt_policy_rule *rule5 = rule2.copy();
	
	CPPUNIT_ASSERT( rule3 != rule5 );
	
	CPPUNIT_ASSERT( rule2 == *rule3 );
	mspec_rule_key key3 = rule3->set_object(mess3);
	
	CPPUNIT_ASSERT( rule2 != *rule3 );
	
	rule4 = *rule3;
	
	CPPUNIT_ASSERT( rule4 == *rule3 );
	delete(rule3);
	delete(rule5);
	 
}
