/// ----------------------------------------*- mode: C++; -*--
/// @file test_client.cpp
/// An interactive test client to imitate an NSIS Initiator or Responder.
/// ----------------------------------------------------------
/// $Id: test_client.cpp 4118 2015-02-23 20:23:10 amarentes $
/// $HeadURL: https://./src/test_client.cpp $
// ===========================================================
//                      
// Copyright (C) 2014-2015, all rights reserved by
// - Universidad de los Andes
//
// More information and contact:
// https://www.uniandes.edu.co
//                      
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ===========================================================
#include <unistd.h>

#include "logfile.h"
#include <string>
#include <iostream>
#include <list>

#include "mnslp_config.h"
#include "mnslp_daemon.h"
#include "events.h"

#include "gist_conf.h"
#include "configfile.h"

#include "policy_rule.h"
#include "msg/mnslp_ipfix_message.h"
#include "msg/mnslp_ipfix_field.h"
#include "ipfix_def_FOKUS.h"
#include <iostream>
#include <limits>


namespace ntlp {
// configuration class
gistconf gconf;
}

using namespace protlib;
using namespace protlib::log;
using namespace ntlp;
using namespace mnslp;

logfile commonlog("test_client.log", mnslp_config::USE_COLOURS);
logfile &protlib::log::DefaultLog(commonlog);

static mnslp_daemon *mnslpd;
static session_id sid;

static msg::mnslp_ipfix_message *mess1;

void add_ipfix_vendor_fields(msg::mnslp_ipfix_message *mess)
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
	  "Spectral_allocation_profile",  
          "received power spectral density vs. frequency to indicate spectral activity in the band of interest (8-16 bits per discrete frequency value)" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_Center_frequency,  2,  IPFIX_CODING_UINT, 
	  "Center_frequency",  "Center frequency of the sensed band" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_Bandwidth_of_CAP,  2,  IPFIX_CODING_UINT, 
	  "Bandwidth_of_CAP",  "Bandwidth of the spectral allocation profile" );	  
	  
	mess->add_vendor_information_element( IPFIX_ENO_FOKUS,  IPFIX_FT_ORmodulation,  1,  IPFIX_CODING_UINT, 
	  "ORmodulation",  "CREST factor" );	  
    
}
 

void create_ipfix_message()
{

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

        // Ad export fields
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

	msg::mnslp_ipfix_field field1 = mess1->get_field_definition( 0, IPFIX_FT_FLOWKEYINDICATOR );
	msg::mnslp_ipfix_value_field fvalue1 = field1.get_ipfix_value_field(flow_indicator);

	msg::mnslp_ipfix_field field2 = mess1->get_field_definition( 0, IPFIX_FT_COLLECTORIPV4ADDRESS );
	msg::mnslp_ipfix_value_field fvalue2 = field2.get_ipfix_value_field( (uint8_t *) buf, 4);

	msg::mnslp_ipfix_field field3 = mess1->get_field_definition( 0, IPFIX_FT_COLLECTORPROTOCOLVERSION );
	msg::mnslp_ipfix_value_field fvalue3 = field3.get_ipfix_value_field(ipfix_ver);
	
	msg::mnslp_ipfix_field field4 = mess1->get_field_definition( 0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL );
	msg::mnslp_ipfix_value_field fvalue4 = field4.get_ipfix_value_field( protocol);

	// In this case just the last three fields are used for filter purposes.
	
	msg::mnslp_ipfix_field field5 = mess1->get_field_definition( 0, IPFIX_FT_SOURCEIPV4ADDRESS );
	msg::mnslp_ipfix_value_field fvalue5 = field5.get_ipfix_value_field( (uint8_t *) buf, 4);

	msg::mnslp_ipfix_field field6 = mess1->get_field_definition( 0, IPFIX_FT_TCPDESTINATIONPORT );
	msg::mnslp_ipfix_value_field fvalue6 = field6.get_ipfix_value_field(destination_port);

	msg::mnslp_ipfix_field field7 = mess1->get_field_definition( 0, IPFIX_FT_PROTOCOLIDENTIFIER );
	msg::mnslp_ipfix_value_field fvalue7 = field7.get_ipfix_value_field( protocol );
	
	msg::mnslp_ipfix_data_record data(templateoptionid);
	data.insert_field(0, IPFIX_FT_FLOWKEYINDICATOR, fvalue1);
	data.insert_field(0, IPFIX_FT_COLLECTORIPV4ADDRESS, fvalue2);
	data.insert_field(0, IPFIX_FT_COLLECTORPROTOCOLVERSION, fvalue3);
	data.insert_field(0, IPFIX_FT_COLLECTORTRANSPORTPROTOCOL, fvalue4);
	data.insert_field(0, IPFIX_FT_SOURCEIPV4ADDRESS, fvalue5);
	data.insert_field(0, IPFIX_FT_TCPDESTINATIONPORT, fvalue6);
	data.insert_field(0, IPFIX_FT_PROTOCOLIDENTIFIER, fvalue7);
	mess1->include_data(templateoptionid, data);
	mess1->output();

}

void setup() 
{

     int sourceid = 0x00000000;
     mess1 = new msg::mnslp_ipfix_message(sourceid, IPFIX_VERSION);

     add_ipfix_vendor_fields(mess1);
     create_ipfix_message();

}

static std::string config_filename;
static hostaddress sender_addr;
static hostaddress receiver_addr;
static uint16 sender_port = 0;
static uint16 receiver_port = 0;
static uint8 protocol = 0;
static uint32 lifetime = 30;
static bool proxy_mode = false;


void parse_commandline(int argc, char *argv[]) {
	using namespace std;

	string usage("usage: test_client [-l lifetime] -c config_file "
			"ds_ip dr_ip [[[ds_port] dr_port] protocol]\n");

	/*
	 * Parse command line options.
	 */
	while ( true ) {
		int c = getopt(argc, argv, "l:c:p");

		if ( c == -1 )
			break;

		switch ( c ) {
			case 'c':
				config_filename = optarg;
				break;
			case 'l':
				lifetime = atoi(optarg);
				break;
			default:
				cerr << usage;
				exit(1);
		}
	}

	if ( config_filename == "" ) {
		cerr << usage;
		exit(1);
	}

	argv += optind;
	argc -= optind;

	/*
	 * Only non-option arguments are left. The first is in argv[0].
	 */
	if ( argc < 2 || argc > 5 || sender_addr.set_ip(argv[0]) == false
			|| receiver_addr.set_ip(argv[1]) == false ) {
		cerr << usage;
		exit(1);
	}

	if ( argc > 2 )
		sender_port = atoi(argv[2]);

	if ( argc > 3 )
		receiver_port = atoi(argv[3]);

	if ( argc > 4 )
		protocol = atoi(argv[4]);
}



void tg_configuration( const hostaddress &source_addr, 
					   const hostaddress &destination_addr,
                       uint16 source_port, uint16 dest_port, 
					   uint8 protocol, uint32 session_lifetime )
{
    setup();
    
    FastQueue ret;
	    
    // Build the vector of objects to be configured.
    vector<msg::mnslp_mspec_object *> mspec_objects;
    mspec_objects.push_back(mess1->copy());

    // Create a new event for launching the configure event.
    event *e = new api_configure_event(source_addr,destination_addr,source_port, 
   				       dest_port, protocol, mspec_objects, 
				       session_lifetime, 
				       selection_metering_entities::sme_any, 
				       &ret);

    mnslp_event_msg *msg = new mnslp_event_msg(session_id(), e);

    mnslpd->get_fqueue()->enqueue(msg);
	
    message *ret_msg = ret.dequeue_timedwait(10000);

    mnslp_event_msg *r = dynamic_cast<mnslp_event_msg *>(ret_msg);
	
    sid = r->get_session_id();
    delete r;
    cout << "session configured, id = " << sid << "\n";

}

static void tg_teardown(session_id id) {
	cout << "tg_TEARDOWN(session=" << sid << ")" << endl;

	event *e = new api_teardown_event(new session_id(id));

	mnslp_event_msg *msg = new mnslp_event_msg(session_id(), e);
	mnslpd->get_fqueue()->enqueue(msg);
}

void tearDown() 
{
       delete mess1;
}

int main(int argc, char *argv[]) 
{
	using namespace std;

	hostaddress source;

	parse_commandline(argc, argv);
	
	init_framework();

	mnslp_config conf;
	
	// create the global configuration parameter repository 
	conf.repository_init();

	// register all NATFW configuration parameters at the registry
	conf.setRepository();

	// register all GIST configuration parameters at the registry
	ntlp::gconf.setRepository();

	// read all config values from config file
	configfile cfgfile(configpar_repository::instance());
	conf.getparref<string>(mnslpconf_conffilename) = config_filename;

	try {
		cfgfile.load(conf.getparref<string>(mnslpconf_conffilename));
	}
	catch(configParException& cfgerr)
	{
		ERRLog("msnlpd", "Error occurred while reading the configuration file: " << cfgerr.what());
		cerr << cfgerr.what() << endl << "Exiting." << endl;
		return 1;
	}

	/*
	 * Start the MNSLP daemon thread. It will in turn start the other
	 * threads it requires.
	 */
	mnslp_daemon_param param("mnslp", conf);
	ThreadStarter<mnslp_daemon, mnslp_daemon_param> starter(1, param);
	
	// returns after all threads have been started
	starter.start_processing();

	mnslpd = starter.get_thread_object();

	// initialize the global session_id, the only one we store
	uint128 raw_id(0, 0, 0, 0);
	sid = session_id(raw_id);

	cout << "config file        : " << config_filename << endl;
	cout << "data sender        : " << sender_addr
		<< ", port " << sender_port << endl;
	cout << "data receiver      : " << receiver_addr
		<< ", port " << receiver_port << endl;
	cout << "IP protocol        : " << int(protocol) << endl;
	cout << "session lifetime   : " << lifetime << " sec" << endl;

	int input;
        std::cin.clear();
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
	do {
		cout << endl;
		cout << "       Current session: " << sid << endl;
		cout << endl;
		cout << "  (1)  tg_CONFIGURE" << endl;
		cout << "  (2)  tg_TEARDOWN for current session " << endl;
		cout << "  (3)  quit" << endl;

		cout << "Input: ";
		std::cin >> input;
		cout << endl;

		if ( input == 1 )
			tg_configuration(sender_addr, receiver_addr,
				sender_port, receiver_port, protocol,
				lifetime);
		else if ( input == 2 )
	      	    tg_teardown(sid);
                std::cin.clear();
		std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	while ( input != 3 );

	cout << "Stopping MNSLP thread ..." << endl;

	// shutdown mnslp thread
	starter.stop_processing();
	starter.wait_until_stopped();

	cleanup_framework();
}


