/*
 * A MNSLP RESPONSE Message.
 *
 * $Id: mnslp_response.h 2014-11-05  $
 * $HeadURL: https://./include/msg/mnslp_response.h $
 */
#ifndef MNSLP_MSG__MNSLP_RESPONSE_H
#define MNSLP_MSG__MNSLP_RESPONSE_H

#include "ie.h"

#include "mnslp_msg.h"
#include "mnslp_ipfix_message.h"


namespace mnslp {
  namespace msg {


/**
 * \class mnslp_response
 *
 * \brief This class implements a MNSLP RESPONSE Message.
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
class mnslp_response : public mnslp_msg {

  public:
	
	static const uint16 MSG_TYPE = 0x2;			///< Number assigned to the response message

	/**
	 * Constructor for the class
	 */
	explicit mnslp_response();
	
	/**
	 * Constructor copying the definition of other message
	 */
	explicit mnslp_response(const mnslp_response &other);
	
	/**
	 * Class destructor 
	 */
	virtual ~mnslp_response();

	/*
	 * Inherited methods:
	 */
	/**
	 * Return a pointer for a new instance of the class
	 */
	mnslp_response *new_instance() const;
	
	/**
	 * Return a pointer to the copy of a message response.
	 */
	mnslp_response *copy() const;
	
	void register_ie(IEManager *iem) const;
	

	virtual void serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError);
	
	virtual bool check() const;		
	

	/*
	 * New methods:
	 */
	/** 
	 * Set the session life time of the message
	 */
	void set_session_lifetime(uint32 seconds);
	
	/** 
	 * Get the session life time of the message
	 */
	uint32 get_session_lifetime() const;

	/** 
	 * Set the message sequence number for the message
	 */
	void set_msg_sequence_number(uint32 msn);
	
	/** 
	 * Get the message sequence number for the message
	 */
	uint32 get_msg_sequence_number(void) const;

	/** 
	 * Set the message information code for the message
	 */
	void set_information_code(uint8 severity, uint8 response_code,
							  uint16 object_type = 0);
							  
	/** 
	 * Get the severity class of the message
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
	
	/** 
	 * Get the object type of the message
	 */	
	uint16 get_object_type() const;
	

	/** 
	 * Get whether the message was sucessfull or not
	 */	
	bool is_success() const;

	/** 
	 * Determine whether this message is response to the configure message 
	 * given as parameter.
	 */	
	bool is_response_to(const mnslp_configure *msg) const;
	
	/** 
	 * Determine whether this message is response to the refresh message 
	 * given as parameter.
	 */	
	bool is_response_to(const mnslp_refresh *msg) const;

	/** 
	 * Insert a ipfix maesage to the response message
	 */ 
	void set_ipfix_message(mnslp_ipfix_message *message);

  protected:
	uint32 serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const;


};


  } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__MNSLP_RESPONSE_H
