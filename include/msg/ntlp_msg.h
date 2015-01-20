/*
 * An NTLP Message.
 *
 * $Id: ntlp_msg.h 2438 2014-11-05 14:10:00  $
 * $HeadURL: https://./include/msg/ntlp_msg.h $
 */
#ifndef MNSLP_MSG__NTLP_MSG_H
#define MNSLP_MSG__NTLP_MSG_H

#include "mri.h"	// from NTLP

#include "mnslp_msg.h"
#include "session_id.h"
#include "session.h"
#include "mnslp_response.h"



namespace mnslp {
  namespace msg {
     using namespace protlib;


/**
 * An NTLP Message.
 *
 * This class encapsulates all the data required for sending a message via
 * the NTLP. It consists of data for the NTLP header, such as addresses,
 * ports, and protocol IDs, and of a MNSLP message. The MNSLP message
 * represents the NTLP body.
 *
 * Note: We don't inherit from IE because this message will not be serialized
 * or deserialized (only the MNSLP message will).
 */
class ntlp_msg {

  public:
	ntlp_msg(const session_id &id, mnslp_msg *body,
			ntlp::mri *m, uint32 sii_handle);
	virtual ~ntlp_msg();

	inline session_id get_session_id() const { return sid; }
	inline mnslp_msg *get_mnslp_msg() const { return msg; }
	inline ntlp::mri *get_mri() const { return routing_info; }
	inline uint32 get_sii_handle() const { return sii_handle; }


	ntlp_msg *copy() const;
	ntlp_msg *copy_for_forwarding(ntlp::mri *new_mri=NULL) const;

	ntlp_msg *create_response(uint8 severity, uint8 response_code) const;
	ntlp_msg *create_error_response(const request_error &e) const;
	ntlp_msg *create_success_response(uint32 session_lifetime) const;

	inline mnslp_configure *get_mnslp_configure() const;
	inline mnslp_refresh *get_mnslp_refresh() const;
	inline mnslp_notify *get_mnslp_notify() const;
	inline mnslp_response *get_mnslp_response() const;
	inline mnslp_options *get_mnslp_options() const;

  private:
	ntlp_msg(const mnslp_msg &other);

	session_id sid;

	ntlp::mri *routing_info;
	uint32 sii_handle;

	// This will be deserialized and used as the NTLP body.
	mnslp_msg *msg;
};

std::ostream &operator<<(std::ostream &out, const ntlp_msg &s);

bool is_greater_than(uint32 s1, uint32 s2);


inline mnslp_configure *ntlp_msg::get_mnslp_configure() const {
	return dynamic_cast<mnslp_configure *>(get_mnslp_msg());
}

inline mnslp_notify *ntlp_msg::get_mnslp_notify() const {
	return dynamic_cast<mnslp_notify *>(get_mnslp_msg());
}

inline mnslp_refresh *ntlp_msg::get_mnslp_refresh() const {
	return dynamic_cast<mnslp_refresh *>(get_mnslp_msg());
}


inline mnslp_response *ntlp_msg::get_mnslp_response() const {
	return dynamic_cast<mnslp_response *>(get_mnslp_msg());
}

inline mnslp_options *ntlp_msg::get_mnslp_options() const {
	return dynamic_cast<mnslp_options *>(get_mnslp_msg());
}

  } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__NTLP_MSG_H
