/// ----------------------------------------*- mode: C++; -*--
/// @file events.h
/// Event classes.
/// ----------------------------------------------------------
/// $Id: events.h 2856 2014-11-08 10:09:00 amarentes $
/// $HeadURL: https://./include/events.h $
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
#ifndef MNSLP__EVENTS_H
#define MNSLP__EVENTS_H

#include "address.h"
#include "protlib_types.h"
#include "timer_module.h"

#include "mri_pc.h"	// from NTLP
#include "mri_le.h"	// from NTLP

#include "messages.h"
#include "msg/ntlp_msg.h"
#include "session.h"
#include "policy_rule.h"
#include "mnslp_timers.h"
#include "msg/selection_metering_entities.h"
#include <vector>


namespace mnslp {
  using namespace msg;
  using protlib::uint32;
  using protlib::hostaddress;
  using std::ostream;


/**
 * An abstract event.
 */
class event {
	
  public:
  
	virtual ~event();
	
	session_id *get_session_id() const { return sid; }
	
	virtual ostream &print(ostream &out) const { return out << "[event]"; }

  protected:
  
	event(session_id *sid=NULL) : sid(sid) { };

  private:
  
	session_id *sid;
};

inline event::~event() {
	if ( sid != NULL )
		delete sid;
}


inline ostream &operator<<(ostream &out, const event &e) {
	return e.print(out);
}


/**
 * This event is triggered for GIST NetworkNotifications.
 *
 * Usually, GIST informs us that a new Messaging Association (MA) has been
 * created.
 */
class network_notification_event : public event {
	
  public:
  
	network_notification_event() : event() { }
	
	virtual ~network_notification_event() { }
	
	virtual ostream &print(ostream &out) const {
		return out << "[network_notification_event]"; }
		
};


/**
 * Sent if GIST detected that a route changed and is no longer valid.
 *
 * This event is triggered by a GIST NetworkNotification.
 */
class route_changed_bad_event : public event {
	
  public:
  
	route_changed_bad_event(session_id *sid) : event(sid) { }
	
	virtual ~route_changed_bad_event() { }
	
	virtual ostream &print(ostream &out) const {
		return out << "[route_changed_bad_event]"; }

};


/**
 * Sent if GIST can't establish a connection to the next node.
 *
 * This event is triggered for GIST MessageStatus notifications if no GIST
 * node was found.
 */
class no_next_node_found_event : public event {
	
  public:
  
	no_next_node_found_event(session_id *sid) : event(sid) { }
	
	virtual ~no_next_node_found_event() { }
	
	virtual ostream &print(ostream &out) const {
		return out << "[no_next_node_found_event]"; }
		
};


/**
 * This event is triggered for incoming NTLP messages without NSLP payload.
 *
 * In this case, the event handler has to decide whether the local NTLP
 * instance should establish a session with the source NTLP instance or not.
 */
class routing_state_check_event : public event {
  public:
	routing_state_check_event(session_id *sid,
		ntlp::mri *msg_routing_info=NULL)
		: event(sid), mri(msg_routing_info) { }
		
	virtual ~routing_state_check_event();

	ntlp::mri *get_mri() const { return mri; }

	virtual ostream &print(ostream &out) const {
		return out << "[routing_state_check_event]"; }

  private:
	ntlp::mri *mri;
};

inline routing_state_check_event::~routing_state_check_event() {
	if ( mri != NULL )
		delete mri;
}


class msg_event : public event {
  
  public:
	
	msg_event(session_id *sid, ntlp_msg *msg, bool for_this_node=false);
	virtual ~msg_event();

	bool is_for_this_node() const { return for_this_node; }
	ntlp_msg *get_ntlp_msg()  const { return msg; }
	ntlp::mri *get_mri() const { return msg->get_mri(); }
	uint32 get_sii_handle() const { return msg->get_sii_handle(); }

	mnslp_msg *get_mnslp_msg() const;
	mnslp_response *get_response() const;
	mnslp_configure *get_configure() const;
	mnslp_refresh *get_refresh() const;
	mnslp_options *get_options() const;
	mnslp_notify *get_notify() const;
	

	virtual ostream &print(ostream &out) const {
		return out << "[msg_event]"; }

  private:
	ntlp_msg *msg;
	bool for_this_node;
};

inline msg_event::msg_event(session_id *sid, ntlp_msg *msg, bool for_this_node)
		: event(sid), msg(msg), for_this_node(for_this_node) {

	// sid may be NULL for the test suite
	assert( msg != NULL );
}

inline msg_event::~msg_event() {
	if ( msg != NULL )
		delete msg;
}

inline mnslp_msg *msg_event::get_mnslp_msg() const {
	assert( msg != NULL );
	return msg->get_mnslp_msg();
}

inline mnslp_response *msg_event::get_response() const {
	assert( msg != NULL );
	return dynamic_cast<mnslp_response *>(msg->get_mnslp_msg());
}

inline mnslp_configure *msg_event::get_configure() const {
	assert( msg != NULL );
	return dynamic_cast<mnslp_configure *>(msg->get_mnslp_msg());
}

inline mnslp_refresh *msg_event::get_refresh() const {
	assert( msg != NULL );
	return dynamic_cast<mnslp_refresh *>(msg->get_mnslp_msg());
}

inline mnslp_options *msg_event::get_options() const {
	assert( msg != NULL );
	return dynamic_cast<mnslp_options *>(msg->get_mnslp_msg());
}

inline mnslp_notify *msg_event::get_notify() const {
	assert( msg != NULL );
	return dynamic_cast<mnslp_notify *>(msg->get_mnslp_msg());
}

class timer_event : public event {
	
  public:
  
	timer_event(session_id *sid, id_t id) : event(sid), id(id) { };
	
	virtual ~timer_event() { };

	inline id_t get_id() const { return id; }
	
	inline bool is_timer(timer t) const { return this->id == t.get_id(); }

	virtual ostream &print(ostream &out) const {
		return out << "[timer_event]"; }

  private:
	id_t id;
};


class api_event : public event {
	
  public:
  
	api_event(session_id *sid=NULL) : event(sid) { };
	
	virtual ~api_event() { };
};


/**
 * An API request to send a Configure Message.
 *
 * Conditions: 
 */
class api_configure_event : public api_event {
	
  public:
  
	api_configure_event(const hostaddress &source, const hostaddress &dest,
		uint16 source_port=0, uint16 dest_port=0, uint8 protocol=0,
		uint32 msg_seq_nbr=0, uint32 msg_hop_count=20,
		std::vector<msg::mnslp_mspec_object *> mspec_objects= std::vector<msg::mnslp_mspec_object *>(),
		selection_metering_entities::selection_metering_entities_t sel_met_entities = selection_metering_entities::sme_any, 
		uint32 lifetime=0, FastQueue *rq=NULL)
		: api_event(), source_addr(source), dest_addr(dest),
		  source_port(source_port), dest_port(dest_port), protocol(protocol), 
		  mes_sequence_number(msg_seq_nbr), mes_hop_count(msg_hop_count),
		  mspec_objects(mspec_objects), sel_met_entities(sel_met_entities),
		  session_lifetime(lifetime), return_queue(rq) { }

	virtual ~api_configure_event() { }

	inline hostaddress get_source_address() const { return source_addr; }
	inline uint16 get_source_port() const { return source_port; }

	inline hostaddress get_destination_address() const { return dest_addr; }
	inline uint16 get_destination_port() const { return dest_port; }

	inline uint8 get_protocol() const { return protocol; }
	
	inline uint32 get_session_lifetime() const { return session_lifetime; }
	
	inline uint32 get_selection_metering_entities()  const { return (uint32) sel_met_entities; } 

	inline uint32 get_message_hop_count()  const { return mes_hop_count; } 

	inline uint32 get_msg_sequence_number()  const { return mes_sequence_number; } 
	
	inline const std::vector<msg::mnslp_mspec_object *> &get_metering_objects() const {
		return mspec_objects; }

	inline FastQueue *get_return_queue() const { return return_queue; }

	virtual ostream &print(ostream &out) const {
		return out << "[api_configure_event]"; }

  private:
  
	hostaddress source_addr;
	hostaddress dest_addr;
	uint16 source_port;
	uint16 dest_port;
	uint8 protocol;

	uint32 mes_sequence_number;
	uint32 mes_hop_count;
	std::vector<msg::mnslp_mspec_object *> mspec_objects;
	selection_metering_entities::selection_metering_entities_t sel_met_entities;
	uint32 session_lifetime;

	FastQueue *return_queue;
};

/**
 * An API request to send a Refresh Message.
 *
 * Conditions: 
 */
class api_refresh_event : public api_event {
	
  public:
  
	api_refresh_event(session_id *sid, const hostaddress &source, const hostaddress &dest,
					  uint16 source_port=0, uint16 dest_port=0, uint8 protocol=0,
					  uint32 lifetime=0, uint32 msgseqnbr=0, FastQueue *rq=NULL)
		: api_event(), source_addr(source), dest_addr(dest),
		  source_port(source_port), dest_port(dest_port), 
		  protocol(protocol), session_lifetime(lifetime), 
		  msg_sequence_number(msgseqnbr), return_queue(rq) { }

	virtual ~api_refresh_event() { }

	inline hostaddress get_source_address() const { return source_addr; }
	inline uint16 get_source_port() const { return source_port; }

	inline hostaddress get_destination_address() const { return dest_addr; }
	inline uint16 get_destination_port() const { return dest_port; }

	inline uint8 get_protocol() const { return protocol; }
	
	inline uint32 get_session_lifetime() const { return session_lifetime; }
	
	inline uint32 get_msg_sequence_number() const { return msg_sequence_number; }
	
	inline FastQueue *get_return_queue() const { return return_queue; }

	virtual ostream &print(ostream &out) const {
		return out << "[api_refresh_event]"; }

  private:
  
	hostaddress source_addr;
	hostaddress dest_addr;
	uint16 source_port;
	uint16 dest_port;
	uint8 protocol;
	uint32 session_lifetime;
	uint32 msg_sequence_number;

	FastQueue *return_queue;
};

/**
 * An API request to send a Notify Message.
 *
 * Conditions: 
 */
class api_notify_event : public api_event {
	
  public:
  
	api_notify_event(session_id *sid, const hostaddress &source, const hostaddress &dest,
		uint16 source_port=0, uint16 dest_port=0, uint8 protocol=0,
		uint8 severity=2, uint8 response_code=1, uint16 object_type = 0, 
		FastQueue *rq=NULL) : api_event(), source_addr(source), dest_addr(dest),
		  source_port(source_port), dest_port(dest_port),
		  protocol(protocol), 
		  severity(severity), // success
		  response_code(response_code), // suc_successfully_processed
		  object_type(object_type), // obj_none
		  return_queue(rq) { }

	virtual ~api_notify_event() { }

	inline hostaddress get_source_address() const { return source_addr; }
	inline uint16 get_source_port() const { return source_port; }

	inline hostaddress get_destination_address() const { return dest_addr; }
	inline uint16 get_destination_port() const { return dest_port; }

	inline uint8 get_protocol() const { return protocol; }
	
	inline uint8 get_severity_class() const { return severity; }
	
	inline uint16 get_response_object_type() const { return object_type; }
	
	inline FastQueue *get_return_queue() const { return return_queue; }

	virtual ostream &print(ostream &out) const {
		return out << "[api_notify_event]"; }

  private:
  
	hostaddress source_addr;
	hostaddress dest_addr;
	uint16 source_port;
	uint16 dest_port;
	uint8 protocol;
	uint8 severity;
	uint8 response_code;
	uint16 object_type;

	FastQueue *return_queue;
};


/**
 * An API request to send a Response Message.
 *
 * Conditions: 
 */
class api_response_event : public api_event {
	
  public:
  
	api_response_event(session_id *sid, const hostaddress &source, const hostaddress &dest,
		uint16 source_port=0, uint16 dest_port=0, uint8 protocol=0,
		uint32 lifetime=0, uint32 msgseqnbr=0, uint8 severity=2, 
		uint8 response_code=1, uint16 object_type = 0, FastQueue *rq=NULL): 
		api_event(sid), source_addr(source), dest_addr(dest),
		source_port(source_port), dest_port(dest_port),protocol(protocol), 
		session_lifetime(lifetime), msg_sequence_number(msgseqnbr),
		severity(severity), // success
		response_code(response_code), // suc_successfully_processed
		object_type(object_type), // obj_none
		return_queue(rq) { }

	virtual ~api_response_event() { }

	inline hostaddress get_source_address() const { return source_addr; }
	inline uint16 get_source_port() const { return source_port; }

	inline hostaddress get_destination_address() const { return dest_addr; }
	inline uint16 get_destination_port() const { return dest_port; }

	inline uint8 get_protocol() const { return protocol; }

	inline uint32 get_session_lifetime() const { return session_lifetime; }
	
	inline uint32 get_msg_sequence_number() const { return msg_sequence_number; }	
	
	inline uint8 get_severity_class() const { return severity; }
	
	inline uint16 get_response_object_type() const { return object_type; }
	
	inline FastQueue *get_return_queue() const { return return_queue; }

	virtual ostream &print(ostream &out) const {
		return out << "[api_response_event]"; }

  private:
  
	hostaddress source_addr;
	hostaddress dest_addr;
	uint16 source_port;
	uint16 dest_port;
	uint8 protocol;
	uint32 session_lifetime;
	uint32 msg_sequence_number;	
	uint8 severity;
	uint8 response_code;
	uint16 object_type;

	FastQueue *return_queue;
};

/**
 * An API request to send an options message.
 *
 * Conditions: 
 */
class api_options_event : public api_event {
	
  public:
  
	api_options_event(const hostaddress &source, const hostaddress &dest,
					  uint16 source_port=0, uint16 dest_port=0, uint8 protocol=0,
					  uint32 msgseqnbr=0)
		: api_event(), source_addr(source), dest_addr(dest),
		  source_port(source_port), dest_port(dest_port), protocol(protocol), 
		  msg_sequence_number(msgseqnbr)  { }
		
	virtual ~api_options_event() { }


	virtual ostream &print(ostream &out) const {
		return out << "[api_options_event]"; }

  private:
	hostaddress source_addr;
	hostaddress dest_addr;
	uint16 source_port;
	uint16 dest_port;
	uint8 protocol;
	uint32 msg_sequence_number;	

};


/**
 * An API request to shut down a session.
 *
 * Conditions: tg_CONF-O and 
 */
class api_teardown_event : public api_event {
  public:
	api_teardown_event(session_id *sid )
		: api_event(sid) { }
	virtual ~api_teardown_event() { }

	virtual ostream &print(ostream &out) const {
		return out << "[api_teardown_event]"; }

};


/**
 * Check if the event is a timer event with the given timer ID.
 */
inline bool is_timer(const event *evt, timer t) {
	const timer_event *e = dynamic_cast<const timer_event *>(evt);

	if ( e == NULL )
		return false;
	else
		return e->is_timer(t);
}


inline bool is_timer(const event *evt) {
	return dynamic_cast<const timer_event *>(evt) != NULL;
}

inline bool is_api_configure(const event *evt) {
	return dynamic_cast<const api_configure_event *>(evt) != NULL;
}

inline bool is_api_refresh(const event *evt) {
	return dynamic_cast<const api_refresh_event *>(evt) != NULL;
}

inline bool is_api_notify(const event *evt) {
	return dynamic_cast<const api_notify_event *>(evt) != NULL;
}

inline bool is_api_response(const event *evt) {
	return dynamic_cast<const api_response_event *>(evt) != NULL;
}

inline bool is_api_options(const event *evt) {
	return dynamic_cast<const api_options_event *>(evt) != NULL;
}

inline bool is_api_teardown(const event *evt) {
	return dynamic_cast<const api_teardown_event *>(evt) != NULL;
}

inline bool is_routing_state_check(const event *evt) {
	return dynamic_cast<const routing_state_check_event *>(evt) != NULL;
}

inline bool is_route_changed_bad_event(const event *evt) {
	return dynamic_cast<const route_changed_bad_event *>(evt) != NULL;
}

inline bool is_no_next_node_found_event(const event *evt) {
	return dynamic_cast<const no_next_node_found_event *>(evt) != NULL;
}

inline bool is_mnslp_configure(const event *evt) {
	const msg_event *e = dynamic_cast<const msg_event *>(evt);

	if ( e == NULL )
		return false;
	else
		return e->get_configure() != NULL;
}

inline bool is_mnlsp_response(const event *evt) {
	const msg_event *e = dynamic_cast<const msg_event *>(evt);

	return e != NULL && e->get_response() != NULL;
}

inline bool is_mnslp_response(const event *evt, uint32 msn) {
	const msg_event *e = dynamic_cast<const msg_event *>(evt);

	if ( e == NULL )
		return false;

	mnslp_response *r = e->get_response();
	if ( r == NULL )
		return false;

	std::cout << "Event Message number:" << r->get_msg_sequence_number() << std::endl;

	return r->has_msg_sequence_number()
		&& msn == r->get_msg_sequence_number();
}

inline bool is_mnslp_response(const event *evt, ntlp_msg *msg) {
	if ( msg == NULL )
		return false;

	mnslp_msg *m = msg->get_mnslp_msg();

	if ( m == NULL || ! m->has_msg_sequence_number() )
		return false;

	std::cout << "Message number:" << m->get_msg_sequence_number() << std::endl;
	
	return is_mnslp_response(evt, m->get_msg_sequence_number());
}

inline bool is_mnslp_refresh(const event *evt) {
	const msg_event *e = dynamic_cast<const msg_event *>(evt);

	if ( e == NULL )
		return false;
	else
		return e->get_refresh() != NULL;
}

inline bool is_mnslp_options(const event *evt) {
	const msg_event *e = dynamic_cast<const msg_event *>(evt);

	if ( e == NULL )
		return false;
	else
		return e->get_options() != NULL;
}

inline bool is_mnslp_notify(const event *evt) {
	const msg_event *e = dynamic_cast<const msg_event *>(evt);

	if ( e == NULL )
		return false;
	else
		return e->get_notify() != NULL;
}

inline bool is_invalid_mnslp_msg(const event *evt) {
	const msg_event *e = dynamic_cast<const msg_event *>(evt);

	if ( e == NULL )
		return false;
	else
		return e->get_mnslp_msg() == NULL;
}


/**
 * Wrap a mnslp event in a protlib message.
 *
 * This may be used to handle mnslp::event classes using FastQueues.
 *
 * Note: The event is not deleted by the destructor!
 */
class mnslp_event_msg : public message {
  public:
	mnslp_event_msg(session_id id, event *e, qaddr_t source=qaddr_unknown)
		: message(message::type_transport, source), sid(id), evt(e) { }

	session_id get_session_id() const { return sid; }
	event *get_event() const { return evt; }

  private:
	session_id sid;
	event *evt;
};


} // namespace mnslp


#endif // MNSLP__EVENTS_H
