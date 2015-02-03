/*
 * Test the policy_action_container class.
 *
 * $Id: policy_action_container.cpp 1711 2015-01-24 8:58:00 amarentes $
 * $HeadURL: https://./test/policy_action_container.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <ext/hash_map>
#include "netmate_ipfix_policy_action_mapping.h"
#include "policy_action_container.h"
#include "msg/mnslp_ipfix_field.h"
#include "policy_action.h"
#include "ipfix_def_FOKUS.h"
#include <string>
#include <iostream>

using namespace mnslp;


class PolicyActionContainerTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in mnslp_ipfix_fields.
 */
class policy_action_container_test : public policy_action_container
{
  public:
  
	policy_action_container_test(): policy_action_container() { }

	friend class PolicyActionContainerTest;
};



class PolicyActionContainerTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( PolicyActionContainerTest );

	CPPUNIT_TEST( general_test );

	CPPUNIT_TEST_SUITE_END();

  public:
  
	void setUp();
	void general_test(); 
	void tearDown();

  private:

    ipfix_field_type_t packetDeltaCount;
    ipfix_field_type_t DiffMin;
    ipfix_field_type_t DiffAvg;
    ipfix_field_type_t DiffMax;
    ipfix_field_type_t DiffVar;
    ipfix_field_type_t Bytes;
    ipfix_field_type_t timeval_first;
    ipfix_field_type_t timeval_last;
    ipfix_field_type_t bandwidth;
    
    msg::mnslp_ipfix_field *ptrPacketDeltaCount;
    msg::mnslp_ipfix_field *ptrDiffMin;
    msg::mnslp_ipfix_field *ptrDiffAvg;
    msg::mnslp_ipfix_field *ptrDiffMax;
    msg::mnslp_ipfix_field *ptrDiffVar;
    msg::mnslp_ipfix_field *ptrBytes;
    msg::mnslp_ipfix_field *ptrTimeval_first;
    msg::mnslp_ipfix_field *ptrTimeval_last;
    msg::mnslp_ipfix_field *ptrBandwidth;
     
    netmate_ipfix_metering_config *netmate1;
    netmate_ipfix_metering_config *netmate2;
    netmate_ipfix_policy_action_mapping *ptrActionMapping1; 
    netmate_ipfix_policy_action_mapping *ptrActionMapping2;
   	policy_action *action_jitter;
	policy_action *action_bandwidth; 
    policy_action_container_test * ptrPolicyActionContainer;
    
};

CPPUNIT_TEST_SUITE_REGISTRATION( PolicyActionContainerTest );

void PolicyActionContainerTest::setUp() {

	packetDeltaCount.eno = 0;  
	packetDeltaCount.ftype = IPFIX_FT_PACKETDELTACOUNT;
	packetDeltaCount.length = 8; 
	packetDeltaCount.coding =  IPFIX_CODING_UINT;
	packetDeltaCount.name = "packetDeltaCount";
	packetDeltaCount.documentation = "";

	DiffMin.eno = IPFIX_ENO_FOKUS;  
	DiffMin.ftype = IPFIX_FT_OWDMIN_USEC;
	DiffMin.length = 8; 
	DiffMin.coding =  IPFIX_CODING_UINT;
	DiffMin.name = "owdmin_usec";
	DiffMin.documentation = "FOKUS minimum owd in us";

	DiffAvg.eno = IPFIX_ENO_FOKUS;  
	DiffAvg.ftype = IPFIX_FT_OWDMEAN_USEC;
	DiffAvg.length = 8; 
	DiffAvg.coding =  IPFIX_CODING_UINT;
	DiffAvg.name = "owdmean_usec";
	DiffAvg.documentation = "FOKUS mean owd in us";

	DiffMax.eno = IPFIX_ENO_FOKUS;  
	DiffMax.ftype = IPFIX_FT_OWDMAX_USEC;
	DiffMax.length = 8; 
	DiffMax.coding =  IPFIX_CODING_UINT;
	DiffMax.name = "owdmax_usec";
	DiffMax.documentation = "FOKUS maximum owd in us";

	DiffVar.eno = IPFIX_ENO_FOKUS;  
	DiffVar.ftype = IPFIX_FT_OWDVAR_USEC;
	DiffVar.length = 4; 
	DiffVar.coding =  IPFIX_CODING_UINT;
	DiffVar.name = "owdvar_usec";
	DiffVar.documentation = "FOKUS delay variation in us";

	Bytes.eno = 0;  
	Bytes.ftype = IPFIX_FT_OCTETDELTACOUNT;
	Bytes.length = 8; 
	Bytes.coding =  IPFIX_CODING_UINT;
	Bytes.name = "octetDeltaCount";
	Bytes.documentation = "";

	timeval_first.eno = 0;  
	timeval_first.ftype = IPFIX_FT_FLOWSTARTMILLISECONDS;
	timeval_first.length = 8; 
	timeval_first.coding =  IPFIX_CODING_UINT;
	timeval_first.name = "flowStartMilliseconds";
	timeval_first.documentation = "";

	timeval_last.eno = 0;  
	timeval_last.ftype = IPFIX_FT_FLOWENDMILLISECONDS;
	timeval_last.length = 8; 
	timeval_last.coding =  IPFIX_CODING_UINT;
	timeval_last.name = "flowEndMilliseconds";
	timeval_last.documentation = "";

	bandwidth.eno = IPFIX_ENO_FOKUS;  
	bandwidth.ftype = IPFIX_FT_Bandwidth_of_CAP;
	bandwidth.length = 2; 
	bandwidth.coding =  IPFIX_CODING_UINT;
	bandwidth.name = "Bandwidth_of_CAP";
	bandwidth.documentation = "Bandwidth of the spectral allocation profile";


    ptrPacketDeltaCount = new msg::mnslp_ipfix_field(packetDeltaCount);
    ptrDiffMin = new msg::mnslp_ipfix_field(DiffMin);
    ptrDiffAvg = new msg::mnslp_ipfix_field(DiffAvg);
    ptrDiffMax = new msg::mnslp_ipfix_field(DiffMax);
    ptrDiffVar = new msg::mnslp_ipfix_field(DiffVar);
    ptrBytes = new msg::mnslp_ipfix_field(Bytes);
    ptrTimeval_first = new msg::mnslp_ipfix_field(timeval_first);
    ptrTimeval_last = new msg::mnslp_ipfix_field(timeval_last);
    ptrBandwidth = new msg::mnslp_ipfix_field(bandwidth);


	netmate1 = new netmate_ipfix_metering_config();
	netmate1->set_priority(1);
	netmate1->set_metering_procedure("jitter");
	netmate1->set_export_directory("/tmp/");
	netmate1->set_export_interval(10);    
	netmate1->set_export_procedure("file");

	netmate2 = new netmate_ipfix_metering_config();
	netmate2->set_priority(1);
	netmate2->set_metering_procedure("bandwidth");
	netmate2->set_export_directory("/tmp/");
	netmate2->set_export_interval(5);    
	netmate2->set_export_procedure("file");
	
	ptrActionMapping1 = new netmate_ipfix_policy_action_mapping();
	ptrActionMapping2 = new netmate_ipfix_policy_action_mapping();
	
	action_jitter = new policy_action();
	action_bandwidth = new policy_action();
		
	ptrPolicyActionContainer = new policy_action_container_test();
	
}


void PolicyActionContainerTest::tearDown() 
{

    delete(ptrPacketDeltaCount);
    delete(ptrDiffMin);
    delete(ptrDiffAvg);
    delete(ptrDiffMax);
    delete(ptrDiffVar);
    delete(ptrBytes);
    delete(ptrTimeval_first);
    delete(ptrTimeval_last);
    delete(ptrBandwidth);
		
	delete(ptrPolicyActionContainer);
}

void PolicyActionContainerTest::general_test() 
{
   

	application_field_mapping packet_delta_count1 ("false","false","true","packets_1"); 
	application_field_mapping packet_delta_count2 ("false","false","true","packets_2"); 
	application_field_mapping diff_min ("false","false","true","DiffMin"); 
	application_field_mapping diff_avg ("false","false","true","DiffAvg"); 
	application_field_mapping diff_max ("false","false","true","DiffMax"); 
	application_field_mapping diff_var ("false","false","true","DiffVar"); 
	application_field_mapping bytes ("false","false","true","bytes"); 
	application_field_mapping timeval_first ("false","false","true","timeval_first"); 
	application_field_mapping timeval_last ("false","false","true","timeval_last"); 
	application_field_mapping bandwidth ("false","false","true","bandwidth"); 
	
	ptrActionMapping1->set_field(ptrPacketDeltaCount->get_field_key(), packet_delta_count1);
	ptrActionMapping1->set_field(ptrDiffMin->get_field_key(), diff_min);
	ptrActionMapping1->set_field(ptrDiffAvg->get_field_key(), diff_avg);
	ptrActionMapping1->set_field(ptrDiffMax->get_field_key(), diff_max);
	ptrActionMapping1->set_field(ptrDiffVar->get_field_key(), diff_var);
	ptrActionMapping1->set_metering_application("netmate");
	ptrActionMapping1->set_metering_configuration(netmate1);
	
					
	ptrActionMapping2->set_field(ptrPacketDeltaCount->get_field_key(), packet_delta_count2);
	ptrActionMapping2->set_field(ptrBytes->get_field_key(), bytes);
	ptrActionMapping2->set_field(ptrTimeval_first->get_field_key(), timeval_first);
	ptrActionMapping2->set_field(ptrTimeval_last->get_field_key(), timeval_last);
	ptrActionMapping2->set_field(ptrBandwidth->get_field_key(), bandwidth);
	ptrActionMapping2->set_metering_application("netmate");
	ptrActionMapping2->set_metering_configuration(netmate2);
		
	// The key must be the proc_name, the appplication shoud be equal in general.
	
	action_jitter->set_action_mapping(ptrActionMapping1->get_key(), ptrActionMapping1);
	action_jitter->set_action("Jitter");
	action_jitter->set_priority(3);
	
	action_bandwidth->set_action_mapping(ptrActionMapping2->get_key(), ptrActionMapping2);	
	action_bandwidth->set_action("BandWidth");	
	action_bandwidth->set_priority(2);
	
	ptrPolicyActionContainer->set_policy_action(action_jitter->get_action(), *action_jitter);
	ptrPolicyActionContainer->set_policy_action(action_bandwidth->get_action(), *action_bandwidth);
	
	policy_action_container *ptrPolicyActionContainer2 = new policy_action_container(*ptrPolicyActionContainer);
	
	
	CPPUNIT_ASSERT( *ptrPolicyActionContainer2 == *ptrPolicyActionContainer );

	CPPUNIT_ASSERT( ptrPolicyActionContainer->check_field_availability(
						"netmate",*ptrPacketDeltaCount) == true );
			
	CPPUNIT_ASSERT( (ptrPolicyActionContainer2->get_field_traslate(
						"netmate",*ptrPacketDeltaCount)).compare("packets_2") == 0 );

	const netmate_ipfix_metering_config *tmp = dynamic_cast< const netmate_ipfix_metering_config* > (ptrPolicyActionContainer2->get_package(
						"netmate",*ptrPacketDeltaCount));

	CPPUNIT_ASSERT( (tmp->get_metering_procedure()).compare("bandwidth") == 0 );

	delete(ptrPolicyActionContainer2);
	 
}
