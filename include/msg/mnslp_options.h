/*
 * A MNSLP OPTIONS Message.
 *
 * $Id: mnslp_options.h 2014-11-05  $
 * $HeadURL: https://./include/msg/mnslp_options.h $
 */
#ifndef MNSLP_MSG__MNSLP_OPTIONS_H
#define MNSLP_MSG__MNSLP_OPTIONS_H

#include "ie.h"

#include "mnslp_msg.h"


namespace mnslp {
  namespace msg {


/**
 * A MNSLP Options Message.
 *
 * This class implements a MNSLP OPTIONS Message.
 */
class mnslp_options : public mnslp_msg {

  public:
	static const uint16 MSG_TYPE = 0x4;

	explicit mnslp_options();
	explicit mnslp_options(const mnslp_options &other);
	virtual ~mnslp_options();

	/*
	 * Inherited methods:
	 */
	mnslp_options *new_instance() const;
	mnslp_options *copy() const;
	void register_ie(IEManager *iem) const;
	void serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError);	
	virtual bool check() const;	


	/*
	 * New methods:
	 */

	void set_msg_sequence_number(uint32 msn);
	uint32 get_msg_sequence_number() const;

  protected:
	uint32 serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const;
		

};


  } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__MNSLP_OPTIONS_H
