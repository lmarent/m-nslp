/*
 * The Message Sequence Number Object.
 *
 * $Id: msg_sequence_number.h 2895 2014-11-05 00:42:23Z bless $
 * $HeadURL:  $
 */
#ifndef MNSLP_MSG__MSG_SEQUENCE_NUMBER_H
#define MNSLP_MSG__MSG_SEQUENCE_NUMBER_H

#include "mnslp_object.h"


namespace mnslp {
 namespace msg {

    using namespace protlib;


/**
 * \class msg_sequence_number
 *
 * \brief Maintains the sequence number of messages.
 *
 * Every M-NSLP message carries a unique sequence number. This number has
 * to be created randomly on first use and is incremented for every message
 * that is sent. Using the MSN, nodes on the path can discover duplicated or
 * missing messages.
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
class msg_sequence_number : public mnslp_object {

  public:
	static const uint16 OBJECT_TYPE = 0x00F7;

	explicit msg_sequence_number();
	explicit msg_sequence_number(uint32 msn, 
							     treatment_t t = tr_mandatory,
							     bool _unique = true);

	virtual ~msg_sequence_number();

	virtual msg_sequence_number *new_instance() const;
	virtual msg_sequence_number *copy() const;

	virtual size_t get_serialized_size(coding_t coding) const;
	virtual bool check_body() const;
	virtual bool equals_body(const mnslp_object &other) const;
	virtual const char *get_ie_name() const;
	virtual ostream &print_attributes(ostream &os) const;


	virtual bool deserialize_body(NetMsg &msg, uint16 body_length,
			IEErrorList &err, bool skip);

	virtual void serialize_body(NetMsg &msg) const;


	/*
	 * New methods
	 */
	uint32 get_value() const;
	void set_value(uint32 msn);

  private:
	// Disallow assignment for now.
	msg_sequence_number &operator=(const msg_sequence_number &other);

	static const char *const ie_name;

	uint32 value;
};


 } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__MSG_SEQUENCE_NUMBER_H
