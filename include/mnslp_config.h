/// ----------------------------------------*- mode: C++; -*--
/// @file mnslp_config.h
/// This file defines constants used throughout this implementation.
/// ----------------------------------------------------------
/// $Id: mnslp_config.h 4118 2014-11-07 17:03:00Z bless $
/// $HeadURL: https://./include/natfw_config.h $
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
#ifndef MNSLP__MNSLP_CONFIG_H
#define MNSLP__MNSLP_CONFIG_H

#include "messages.h"
#include "address.h"

#include "configpar.h"
#include "configpar_repository.h"

// since we re-use some GIST parameter, we need to define them here
#include "gist_conf.h"

namespace mnslp {
  using namespace protlib;

  // 0 = global realm, 1 = protlib_realm, 2 = gist_realm 3 = qos_nslp_realm 
  // 4 = natfw_realm 5= mnslp_realm

  const realm_id_t mnslp_realm = 5;


  enum mnslp_configpar_id_t {
    mnslpconf_invalid,
    mnslpconf_conffilename,
    mnslpconf_dispatcher_threads,
    mnslpconf_ms_is_meter,
    mnslpconf_ms_install_policy_rules,    
    mnslpconf_ms_export_config_file,
    mnslpconf_ms_filter_config_file,
    
    /* Metering application parameters */
    mnslpconf_ms_metering_application,
	mnslpcong_ms_metering_user,
	mnslpcong_ms_metering_password,
	mnslpcong_ms_metering_server,
	mnslpcong_ms_metering_def_xsl,
	mnslpcong_ms_metering_port,
    
    /* NI  */
    mnslpconf_ni_session_lifetime,
    mnslpconf_ni_response_timeout,
    mnslpconf_ni_max_session_lifetime,
    mnslpconf_ni_max_retries,
    /* NF  */
    mnslpconf_nf_max_session_lifetime,
    mnslpconf_nf_response_timeout,
    /* NR  */
    mnslpconf_nr_max_session_lifetime,
    mnslpconf_nr_ext_max_retries,
    mnslpconf_nr_ext_response_timeout,
    mnslpconf_maxparno
  };


/**
 * The central configuration point for a MNSLP instance.
 */
class mnslp_config {

  public:
	mnslp_config(configpar_repository *cfpgar_rep= NULL) : cfgpar_rep(cfpgar_rep) {};
	
	void repository_init();

	void setRepository(configpar_repository* cfp_rep= configpar_repository::instance());

	/// register copy of configuration parameter instance
	void registerPar(const configparBase& configparid) { cfgpar_rep->registerPar(configparid); }
	
	/// register instance configuration parameter directly
	void registerPar(configparBase* configparid) { cfgpar_rep->registerPar(configparid);  }

	// these are just convenience functions
	template <class T> void setpar(mnslp_configpar_id_t configparid, const T& value);
	
	template <class T> T getpar(mnslp_configpar_id_t configparid) const;
	
	template <class T> T& getparref(mnslp_configpar_id_t configparid) const;
	
	string getparname(mnslp_configpar_id_t configparid);

    bool has_ipv4_address() const { 
		return ntlp::gconf.getparref<hostaddresslist_t>(ntlp::gistconf_localaddrv4).size() > 0; }
    
	const hostaddresslist_t &get_ipv4_addresses() const {
		return ntlp::gconf.getparref<hostaddresslist_t>(ntlp::gistconf_localaddrv4); }

    bool has_ipv6_address() const { 
		return ntlp::gconf.getparref<hostaddresslist_t>(ntlp::gistconf_localaddrv6).size() > 0; }
    
	const hostaddresslist_t &get_ipv6_addresses() const {
		return ntlp::gconf.getparref<hostaddresslist_t>(ntlp::gistconf_localaddrv6); }

    uint16 get_gist_port_udp() const { 
		return ntlp::gconf.getpar<uint16>(ntlp::gistconf_udpport); }
    
	uint16 get_gist_port_tcp() const { 
		return ntlp::gconf.getpar<uint16>(ntlp::gistconf_tcpport); }
	
	uint16 get_gist_port_tls() const { 
		return ntlp::gconf.getpar<uint16>(ntlp::gistconf_tlsport); }

	uint32 get_num_dispatcher_threads() const {
		return getpar<uint32>(mnslpconf_dispatcher_threads); }

	bool is_ms_meter() const { return getpar<bool>(mnslpconf_ms_is_meter); }
	
	string get_metering_application() const { 
		return getpar<string>(mnslpconf_ms_metering_application); }
		
	string get_export_config_file() const { 
		return getpar<string>(mnslpconf_ms_export_config_file); }
	
	string get_filter_config_file() const { 
		return getpar<string>(mnslpconf_ms_filter_config_file); }

	string get_user() const {
		return getpar<string>(mnslpcong_ms_metering_user); }
	
	string get_password() const {
		return getpar<string>(mnslpcong_ms_metering_password); }

	string get_metering_server() const {
		return getpar<string>(mnslpcong_ms_metering_server); }

	string get_metering_xsl() const {
		return getpar<string>(mnslpcong_ms_metering_def_xsl); }

	string get_metering_port() const {
		return getpar<string>(mnslpcong_ms_metering_port); }

    uint32 get_ni_session_lifetime() const { 
		return getpar<uint32>(mnslpconf_ni_max_session_lifetime); }
        
	uint32 get_ni_max_retries() const { 
		return getpar<uint32>(mnslpconf_ni_max_retries); }
    
    uint32 get_ni_response_timeout() const { 
		return getpar<uint32>(mnslpconf_ni_response_timeout); }

	uint32 get_nf_max_session_lifetime() const { 
	  return getpar<uint32>(mnslpconf_nf_max_session_lifetime); }
	  
	uint32 get_nf_response_timeout() const { 
	  return getpar<uint32>(mnslpconf_nf_response_timeout); }

	bool get_ms_install_policy_rules() const {
		return getpar<bool>(mnslpconf_ms_install_policy_rules); }

	uint32 get_nr_max_session_lifetime() const {
		return getpar<uint32>(mnslpconf_nr_max_session_lifetime); }

	uint32 get_nr_ext_max_retries() const { 
		return getpar<uint32>(mnslpconf_nr_ext_max_retries); }
		
	uint32 get_nr_ext_response_timeout() const {
		return getpar<uint32>(mnslpconf_nr_ext_response_timeout); }

	/// The ID of the queue that receives messages from the NTLP.
	static const message::qaddr_t INPUT_QUEUE_ADDRESS
		= message::qaddr_api_2;

	/// The ID of the queue inside the NTLP that we send messages to.
	static const message::qaddr_t OUTPUT_QUEUE_ADDRESS
		= message::qaddr_coordination;

	/// The timer module's queue address.
	static const message::qaddr_t TIMER_MODULE_QUEUE_ADDRESS
		= message::qaddr_timer;

	/// A non reserved protocol id identified was used.
	static const uint16 NSLP_ID				    = 34;

	static const uint32 SEND_MSG_TIMEOUT		= 5;
	static const uint16 SEND_MSG_IP_TTL			= 100;
	static const uint32 SEND_MSG_GIST_HOP_COUNT	= 10;

	/// Whether to use colours in the logging messages.
	static const bool USE_COLOURS				= true;

  protected:
  
	configpar_repository* cfgpar_rep;

	hostaddress get_hostaddress(const std::string &key);

	void registerAllPars();
};


// this is just a convenience function
template <class T>
void 
mnslp_config::setpar(mnslp_configpar_id_t configparid, const T& value)
{
	cfgpar_rep->setPar(mnslp_realm, configparid, value);
}


// this is just a convenience function
template <class T> T
mnslp_config::getpar(mnslp_configpar_id_t configparid) const
{
	return cfgpar_rep->getPar<T>(mnslp_realm, configparid);
}


// this is just a convenience function
template <class T> T&
mnslp_config::getparref(mnslp_configpar_id_t configparid) const
{
	return cfgpar_rep->getParRef<T>(mnslp_realm, configparid);
}


// this is just a convenience function
inline
string
mnslp_config::getparname(mnslp_configpar_id_t configparid)
{
	// reference to the config repository singleton
	return cfgpar_rep->getConfigPar(mnslp_realm, configparid)->getName();
}


} // namespace mnslp

#endif // MNSLP__MNSLP_CONFIG_H
