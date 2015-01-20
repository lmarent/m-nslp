/*
 * The Session Lifetime Object.
 *
 * $Id: session_lifetime.h 2895 2014-11-05 00:42:23Z bless $
 * $HeadURL: https://./include/msg/session_lifetime.h $
 */
#ifndef MNSLP_MSG__SESSION_LIFETIME_H
#define MNSLP_MSG__SESSION_LIFETIME_H

#include "mnslp_object.h"


namespace mnslp {
 namespace msg {

    using namespace protlib;


/**
 * \class session_lifetime
 *
 * \brief The Session Lifetime Object.
 * 
 * RFC definition: This object carries the requested lifetime for a M-NSLP session in a
 * 				   CONFIGURE / REFRESH message or the granted session lifetime in a
 * 				   RESPONSE message, in milliseconds. When a M-NSLP session expires,
 *   			   the Metering Manager MUST configure the Monitoring Probe to stop the
 * 				   Metering.
 * 
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2014/12/22 10:31:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class session_lifetime : public mnslp_object {

  public:
	static const uint16 OBJECT_TYPE = 0x00F8;

	explicit session_lifetime();
	
	explicit session_lifetime(uint32 slt, treatment_t t = tr_mandatory, bool _unique = true);

	virtual ~session_lifetime();

	virtual session_lifetime *new_instance() const;
	
	virtual session_lifetime *copy() const;

	virtual size_t get_serialized_size(coding_t coding) const;
	
	virtual bool check_body() const;
	
	virtual bool equals_body(const mnslp_object &other) const;
	
	virtual const char *get_ie_name() const;
	
	virtual ostream &print_attributes(ostream &os) const;

	virtual bool deserialize_body(NetMsg &msg, uint16 body_length,
			IEErrorList &err, bool skip);

	virtual void serialize_body(NetMsg &msg) const;

	/**
	 * Get the session lifetime value
	 */ 
	uint32 get_value() const;

	/**
	 * Set the session lifetime value
	 */ 
	void set_value(uint32 _slt);

	/**
	 * Assignment operator
	 */
	session_lifetime &operator=(const session_lifetime &other);

  private:

	static const char *const ie_name;

	uint32 slt;
};


 } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__SESSION_LIFETIME_H
