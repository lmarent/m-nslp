/// ----------------------------------------*- mode: C++; -*--
/// @file msnlp_daemon.cpp
/// The mnslp_daemon class.
/// ----------------------------------------------------------
/// $Id: mnslp_daemon.cpp 4118 2014-11-08 09:24:00 amarentes $
/// $HeadURL: https://./src/msnlp_daemon.cpp $
// ===========================================================
//                      
// Copyright (C) 2012-2014, all rights reserved by
// - System and Computing Engineering, Universidad de los Andes
//
// More information and contact:
// https://www.uniandes.edu.co/
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
#include "logfile.h"
#include "threads.h"
#include "threadsafe_db.h"
#include "queuemanager.h"

#include "apimessage.h" // from NTLP

#include "mnslp_config.h"
#include "msg/mnslp_ie.h"
#include "msg/mnslp_msg.h"
#include "dispatcher.h"
#include "mnslp_daemon.h"
#include "benchmark_journal.h"

#include <openssl/ssl.h>

using namespace protlib;
using namespace protlib::log;
using namespace mnslp;
using namespace mnslp::msg;


#define LogError(msg) ERRLog("mnslp_daemon", msg)
#define LogWarn(msg) WLog("mnslp_daemon", msg)
#define LogInfo(msg) ILog("mnslp_daemon", msg)
#define LogDebug(msg) DLog("mnslp_daemon", msg)


#ifdef BENCHMARK
  extern benchmark_journal journal;
#endif


/**
 * Constructor.
 */
mnslp_daemon::mnslp_daemon(const mnslp_daemon_param &param)
		: Thread(param), config(param.config),
		  session_mgr(&config), rule_installer(NULL), 
		  ntlp_starter(NULL) {

	startup();
}


/**
 * Destructor.
 */
mnslp_daemon::~mnslp_daemon() {
	shutdown();
}


/**
 * Called exactly once before the first thread executes main_loop.
 */
void mnslp_daemon::startup() {
	LogInfo("starting MNSLP daemon ...");

	/*
	 * Instantiate an operating system dependent policy rule installer.
	 * We use the iptables policy rule installer only on NF nodes which
	 * have it enabled in the configuration. NIs and NRs don't have to
	 * install policy rules.
	 */
	if ( config.get_ms_install_policy_rules() == true
			&& (config.is_ms_meter() ) )
		rule_installer = new netmate_ipfix_policy_rule_installer(&config);
	else
		rule_installer = new nop_policy_rule_installer(&config);

	try {
		rule_installer->setup();
	}
	catch ( policy_rule_installer_error &e ) {
		LogError("unable to setup the policy rule installer: " << e);
	}

    AddressList *addresses = new AddressList();

    hostaddresslist_t& ntlpv4addr= ntlp::gconf.getparref< hostaddresslist_t >(ntlp::gistconf_localaddrv4);
    hostaddresslist_t& ntlpv6addr= ntlp::gconf.getparref< hostaddresslist_t >(ntlp::gistconf_localaddrv6);

    if (ntlpv4addr.size() == 0 && ntlpv6addr.size() == 0) {
         addresses->add_host_prop(NULL, AddressList::ConfiguredAddr_P);
         addresses->add_host_prop(NULL, AddressList::ConfiguredAddr_P);
         addresses->add_host_prop(NULL, AddressList::ConfiguredAddr_P);
    }

    // fill in configured IPv4 addresses
    if (ntlpv4addr.size() != 0) {
         hostaddresslist_t::const_iterator it;
         for (it = ntlpv4addr.begin(); it != ntlpv4addr.end(); it++) {
            netaddress na((*it));
            addresses->add_property(na);
         }
    }

    // fill in configured IPv6 addresses
    if (ntlpv6addr.size() != 0) {
         hostaddresslist_t::const_iterator it;
         for (it = ntlpv6addr.begin(); it != ntlpv6addr.end(); it++) {
            netaddress na((*it));
            addresses->add_property(na);
         }
   }

   // MOBILITY (mobility extension): configure net prefix of home network
   const netaddress& na= ntlp::gconf.getparref<netaddress>(ntlp::gistconf_home_netprefix);
   if (!na.is_ip_unspec()) {
        addresses->add_property(na, AddressList::HomeNet_P);
   }

   // MOBILITY: configure home address
   const hostaddress& homeaddr= ntlp::gconf.getparref< hostaddress >(ntlp::gistconf_home_address);
   if (!homeaddr.is_ip_unspec()){
       const netaddress na(homeaddr);
       addresses->add_property(na, AddressList::HomeAddr_P);
       addresses->add_property(na, AddressList::ConfiguredAddr_P);
   }

    // MOBILITY: care-of interfaces
    const string& coa_iface= ntlp::gconf.getparref< string >(ntlp::gistconf_coa_interfaces);
    if (!coa_iface.empty()){
       std::stringstream in(coa_iface);
       while (in) {
         std::string token;
         in >> token;
         addresses->ignore_locals();
         addresses->by_interface(true);
         // XXX: memleak
         addresses->add_interface(strdup(token.c_str()));
       }
    }

    // MOBILITY: home agent address
    const hostaddress& homeagent_addr= ntlp::gconf.getparref< hostaddress >(ntlp::gistconf_homeagent_address);
    if (!homeagent_addr.is_ip_unspec()){
        const netaddress na(homeagent_addr);
        addresses->add_property(na, AddressList::HAAddr_P);
        addresses->add_property(na, AddressList::ConfiguredAddr_P);
    }
    // MOBILITY: home agent address
    const hostaddress& alt_homeagent_addr= ntlp::gconf.getparref< hostaddress  >(ntlp::gistconf_homeagent_address_alt);
    if (!alt_homeagent_addr.is_ip_unspec()){
        const netaddress na(alt_homeagent_addr);
        addresses->add_property(na, AddressList::AltHAAddr_P);
        addresses->add_property(na, AddressList::ConfiguredAddr_P);
    }


	/*
	 * Start the GIST thread.
	 */
	NTLPStarterParam ntlpparam;
	ntlpparam.addresses = addresses;	
	ntlp_starter= new ThreadStarter<NTLPStarter, NTLPStarterParam>(1, ntlpparam);
	ntlp_starter->start_processing();


	/*
	 * Register our input queue with the queue manager.
	 */
	QueueManager::instance()->register_queue(
		get_fqueue(), mnslp_config::INPUT_QUEUE_ADDRESS);

	/*
	 * Register the queue created above with the NTLP. The NTLP will then
	 * append all MNSLP messages it receives to our input queue.
	 */
	ntlp::APIMsg *api_msg = new ntlp::APIMsg();
	api_msg->set_source(mnslp_config::INPUT_QUEUE_ADDRESS);
	api_msg->set_register(mnslp_config::NSLP_ID, 0); // NSLPID, RAO

	/*
	 * TODO: We have no way to find out if the NTLP thread is up and has
	 * already registered an input queue. Because of this, we try to send
	 * our registration message until there is a queue to accept it.
	 */
	bool success;
	do {
		success = api_msg->send_to(mnslp_config::OUTPUT_QUEUE_ADDRESS);
		sleep(1);
	}
	while ( ! success );
	assert( success );


	LogDebug("MNSLP daemon startup complete");
}


/**
 * Called exactly once after the last thread executes main_loop.
 *
 * TODO: at least I hope there are no active threads left!
 */
void mnslp_daemon::shutdown() {
	LogDebug("MNSLP daemon shutting down ...");

	try {
		rule_installer->remove_all();
	}
	catch ( policy_rule_installer_error &e ) {
		LogError("unable to remove the installed policy rules: " << e);
		LogError("You have to remove them manually!");
	}

	// Shut down the NTLP threads.
	ntlp_starter->stop_processing();
	ntlp_starter->wait_until_stopped();

	delete ntlp_starter;

	delete rule_installer;

	QueueManager::instance()->unregister_queue(
			mnslp_config::INPUT_QUEUE_ADDRESS);

	LogInfo("MNSLP deamon shutdown complete");
}


/**
 * The implementation of the main routine of a worker thread.
 */
void mnslp_daemon::main_loop(uint32 thread_id) {

	/* 
	 * The dispatcher handles incoming messages. It is the top-level state
	 * machine which delegates work to the state machines on session level.
	 *
	 * For each main_loop, and thus POSIX thread, there is a dispatcher.
	 */
	dispatcher disp(&session_mgr, rule_installer, &config);
	gistka_mapper mapper;


	/*
	 * Wait for messages in the input queue and process them.
	 */
	LogInfo("dispatcher thread #"
			<< thread_id << " waiting for incoming messages ...");

	FastQueue *mnslp_input = get_fqueue();

	while ( get_state() == Thread::STATE_RUN ) {
		// A timeout makes sure the loop condition is checked regularly.
		message *msg = mnslp_input->dequeue_timedwait(1000);

		if ( msg == NULL )
			continue;	// no message in the queue

		LogDebug("dispatcher thread #" << thread_id
			<< " processing received message #" << msg->get_id());

		MP(benchmark_journal::PRE_PROCESSING);

		// Analyze message and create an event from it.
		event *evt = mapper.map_to_event(msg);

		// Then feed the event to the dispatcher.
		if ( evt != NULL ) {
			MP(benchmark_journal::PRE_DISPATCHER);
			disp.process(evt);
			MP(benchmark_journal::POST_DISPATCHER);
			delete evt;
		}

		delete msg;

		MP(benchmark_journal::POST_PROCESSING);
	}
}


void mnslp::init_framework() {
	/*
	 * Initialize libraries.
	 */
	tsdb::init();
	SSL_library_init();		// TODO: seed random generator
	OpenSSL_add_ssl_algorithms();
	SSL_load_error_strings();
	MNSLP_IEManager::register_known_ies();
}


void mnslp::cleanup_framework() {
	QueueManager::clear();
	MNSLP_IEManager::clear();
	//tsdb::end();
	LogError("mnslp::cleanup_framework() doesn't call tsdb::end()!");
}

// EOF
