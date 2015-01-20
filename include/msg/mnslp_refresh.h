/*
 * A MNSLP REFRESH Message.
 *
 * $Id: mnslp_refresh.h 2014-11-05  $
 * $HeadURL: https://./include/msg/mnslp_refresh.h $
 */
#ifndef MNSLP_MSG__MNSLP_REFRESH_H
#define MNSLP_MSG__MNSLP_REFRESH_H

#include "ie.h"

#include "mnslp_msg.h"


namespace mnslp {
  namespace msg {


/**
 * A MNSLP Refresh Message.
 *
 * This class implements a MNSLP REFRESH Message.
 */
class mnslp_refresh : public mnslp_msg {

  public:
	static const uint16 MSG_TYPE = 0x5;

	explicit mnslp_refresh();
	explicit mnslp_refresh(const mnslp_refresh &other);
	virtual ~mnslp_refresh();

	/*
	 * Inherited methods:
	 */
	mnslp_refresh *new_instance() const;
	mnslp_refresh *copy() const;
	void register_ie(IEManager *iem) const;
	virtual void serialize(NetMsg &msg, coding_t coding, uint32 &bytes_written) const throw (IEError);
	virtual bool check() const;

	/*
	 * New methods:
	 */
	void set_session_lifetime(uint32 seconds);
	uint32 get_session_lifetime() const;


	void set_msg_sequence_number(uint32 msn);
	uint32 get_msg_sequence_number() const;

  protected:
	uint32 serialize_object(ie_object_key &key, 
						    NetMsg &msg, 
						    coding_t coding) const;

};


  } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__MNSLP_REFRESH_H
