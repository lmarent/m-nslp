/*
 * A MNSLP CONFIGURE Message.
 *
 * $Id: mnslp_create.h 2014-11-05  $
 * $HeadURL: https://./include/msg/mnslp_configure.h $
 */
#ifndef MNSLP_MSG__MNSLP_CONFIGURE_H
#define MNSLP_MSG__MNSLP_CONFIGURE_H

#include "ie.h"

#include "mnslp_msg.h"
#include "mnslp_ipfix_message.h"


namespace mnslp {
  namespace msg {


/**
 * A MNSLP CREATE Message.
 *
 * This class implements a MNSLP CONFIGURE Message.
 */
class mnslp_configure : public mnslp_msg {

  public:
	static const uint8 MSG_TYPE = 0x1;

	explicit mnslp_configure();
	explicit mnslp_configure(const mnslp_configure &other);
	virtual ~mnslp_configure();

	/*
	 * Inherited methods:
	 */
	mnslp_configure *new_instance() const;
	mnslp_configure *copy() const;
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
	
	void set_selection_metering_entities(uint32 value);

	uint32 get_selection_metering_entities() const;
	
	void set_message_hop_count(uint32 value);
	
	uint32 get_message_hop_count() const;
	
	void set_mspec_object(mnslp_mspec_object *obj);
	
	void get_mspec_objects(std::vector<mnslp_mspec_object *> &list_return);
	
  protected:
	uint32 serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const;


};


  } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__MNSLP_CONFIGURE_H
