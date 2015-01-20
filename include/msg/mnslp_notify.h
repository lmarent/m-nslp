/*
 * A MNSLP NOTIFY Message.
 *
 * $Id: mnslp_notify.h 2895 2014-11-05  $
 * $HeadURL: https://./include/msg/mnslp_notify.h $
 */
#ifndef MNSLP_MSG__MNSLP_NOTIFY_H
#define MNSLP_MSG__MNSLP_NOTIFY_H

#include "ie.h"

#include "mnslp_msg.h"


namespace mnslp {
  namespace msg {


/**
 * A MNSLP NOTIFY Message.
 *
 * This class implements the MNSLP NOTIFY Message, which may be sent
 * asynchronously to report errors.
 */
class mnslp_notify : public mnslp_msg {

  public:
	static const uint8 MSG_TYPE = 0x3;

	explicit mnslp_notify();
	explicit mnslp_notify(const mnslp_notify &other);
	virtual ~mnslp_notify();

	/*
	 * Inherited methods:
	 */
	mnslp_notify *new_instance() const;
	mnslp_notify *copy() const;
	void register_ie(IEManager *iem) const;
	virtual void serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError);
	virtual bool check() const;		
	

	/*
	 * New methods:
	 */
	void set_information_code(uint8 severity, uint8 response_code,
							  uint16 object_type = 0);
	
	/**
	 * Get the severity class type of information code
	 */
	uint8 get_severity_class() const;
	
	/**
	 * Get the response code
	 */
	 uint8 get_response_code() const;
	
	/**
	 * Get the response object type
	 */
	uint16 get_response_object_type() const;
	
	uint16 get_object_type() const;

  protected:
	uint32 serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const;
	
};


  } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__MNSLP_NOTIFY_H
