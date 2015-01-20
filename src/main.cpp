/// ----------------------------------------*- mode: C++; -*--
/// @file main.cpp
/// main file for mnslp program
/// ----------------------------------------------------------
/// $Id: main.cpp 4118 2014-11-11 09:03:18 amarentes $
/// $HeadURL: https://./src/main.cpp $
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
#include <unistd.h>	// for getopt
#include <openssl/ssl.h>

#include "logfile.h"
#include "threads.h"

#include "gist_conf.h"
#include "configfile.h"

#include "mnslp_config.h"
#include "mnslp_daemon.h"


using namespace protlib;
using namespace protlib::log;
using namespace mnslp;

namespace ntlp {
// configuration class
gistconf gconf;
}

mnslp_config conf;

using namespace ntlp;

logfile commonlog("mnslp.log", mnslp_config::USE_COLOURS);
logfile &protlib::log::DefaultLog(commonlog);


std::string config_filename;

void parse_commandline(int argc, char *argv[]) {
	std::string usage("usage: mnslp -c config_file\n");

	/*
	 * Parse command line options.
	 */
	while ( true ) {
		int c = getopt(argc, argv, "c:");

		if ( c == -1 )
			break;

		switch ( c ) {
			case 'c':
				conf.getparref<string>(mnslpconf_conffilename) = optarg;
				break;
			default:
				std::cerr << usage;
				exit(1);
		}
	}

	if ( conf.getparref<string>(mnslpconf_conffilename) == "" ) {
		std::cerr << usage;
		exit(1);
	}

	argv += optind;
	argc -= optind;

	/*
	 * Only non-option arguments are left. The first is in argv[0].
	 */
	if ( argc > 0 ) {
		std::cerr << usage;
		exit(1);
	}
}


int main(int argc, char *argv[]) {
	
	// create the global configuration parameter repository 
	conf.repository_init();

	// register all MNSLP configuration parameters at the registry
	conf.setRepository();

	// register all GIST configuration parameters at the registry
	ntlp::gconf.setRepository();

	/*
	 * Initialize protlib, openssl, etc.
	 */
	parse_commandline(argc, argv);
	init_framework();

	// read all config values from config file
	configfile cfgfile(configpar_repository::instance());

	try {
		cfgfile.load(conf.getparref<string>(mnslpconf_conffilename));
	}
	catch(configParException& cfgerr)
	{
		ERRLog("mnslp", "Error occurred while reading the configuration file: " << cfgerr.what());
		cerr << cfgerr.what() << endl << "Exiting." << endl;
		return 1;
	}

	/*
	 * Start the MNSLP daemon thread. It will in turn start the other
	 * threads it requires.
	 */
	mnslp_daemon_param param("mnslp", conf);
	ThreadStarter<mnslp_daemon, mnslp_daemon_param> mnslp_thread(
		conf.get_num_dispatcher_threads(), param);
	
	mnslp_thread.start_processing();

	// wait until all threads have been terminated
	mnslp_thread.wait_until_stopped();

	/*
	 * Free all resources.
	 */
	cleanup_framework();
}

// EOF
