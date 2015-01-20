/*
 * A MNSLP Message.
 *
 * $Id: mnslp_msg.h 2438 2014-11-05  $
 * $HeadURL: https://./include/msg/mnslp_msg.h $
 *
 */
#ifndef MNSLP_MSG__MNSLP_MSG_H
#define MNSLP_MSG__MNSLP_MSG_H

#include "ie.h"

#include "ie_store.h"
#include "ie_object_key.h"
#include "mnslp_object.h"


namespace mnslp {
  namespace msg {
     using namespace protlib;


/**
 * A MNSLP Message.
 *
 * This class implements a MNSLP Message. It can read and initialize itself
 * from or write itself into a NetMsg object using deserialize()/serialize(),
 * respectively.
 */
class mnslp_msg : public IE {

  public:
	virtual ~mnslp_msg();

	/*
	 * Inherited from IE:
	 */
	virtual mnslp_msg *new_instance() const;
	virtual mnslp_msg *copy() const;

	virtual mnslp_msg *deserialize(NetMsg &msg, coding_t coding,
			IEErrorList &errorlist, uint32 &bytes_read, bool skip);

	virtual void serialize(NetMsg &msg, coding_t coding,
			uint32 &bytes_written) const throw (IEError);

	virtual bool check() const;
	virtual bool supports_coding(coding_t c) const;
	virtual size_t get_serialized_size(coding_t coding) const;

	virtual bool operator==(const IE &ie) const;
	virtual const char *get_ie_name() const;
	virtual ostream &print(ostream &os, uint32 level, const uint32 indent,
			const char *name = NULL) const;

	virtual void register_ie(IEManager *iem) const;


	/*
	 * New methods:
	 */
	virtual uint8 get_msg_type() const;
	virtual bool has_msg_sequence_number() const;
	virtual uint32 get_msg_sequence_number() const;

	static uint8 extract_msg_type(uint32 header_raw) throw ();

  protected:
	static const uint16 HEADER_LENGTH;

	// protected constructors to prevent instantiation
	explicit mnslp_msg();
	explicit mnslp_msg(uint8 msg_type);
	explicit mnslp_msg(const mnslp_msg &other);

	virtual size_t get_num_objects() const;
	virtual mnslp_object *get_object(ie_object_key &object_type) const;
	virtual void set_object(mnslp_object *obj);
	virtual mnslp_object *remove_object(ie_object_key &object_type);

	virtual void set_msg_type(uint8 mt);

	/**
	 * Map MNSLP Object Type to mnslp_object.
	 */
	ie_store objects;
	typedef ie_store::const_iterator obj_iter;


  private:
	static const char *const ie_name;

	/**
	 * MNSLP Message header fields.
	 */
	uint8 msg_type;

};

  } // namespace msg
} // namespace msnlp


/*
 * Include all known messages for convenience.
 */
#include "mnslp_configure.h"
#include "mnslp_refresh.h"
#include "mnslp_options.h"
#include "mnslp_notify.h"

#endif // MNSLP_MSG__MNSLP_MSG_H
