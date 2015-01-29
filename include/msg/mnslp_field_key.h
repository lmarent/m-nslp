/*
 * Interface for M-NSLP fields keys.
 *
 * $Id: mnslp_field_key.h 2896 2015-01-20 10:27:00 amarentes $
 * $HeadURL: /include/msg/mnslp_field_key.h $
 *
 */
#ifndef MNSLP_MSG__MNSLP_FIELD_KEY_H
#define MNSLP_MSG__MNSLP_FIELD_KEY_H

#include "ie.h"


namespace mnslp {
  namespace msg {
	using namespace protlib;

/**
 * The representation of a MNSLP field key.
 *
 * This class is an abstract class for representing a field key used by 
 * field containers as a key. All concrete parameters have to be subclasses.
 */
class mnslp_field_key {

  public:
  
  	enum msnlp_field_key_type_t {
		mnslp_ipfix_key	= 0,
		mnslp_diameter_key	= 1,
		mnslp_netflow_key	= 2
	};
  
	virtual ~mnslp_field_key();

	/**
	 *  Equals to operator. It is equal when it has the same enterprise number an type
	 */
	virtual uint16 get_field_key_type();
	
	virtual mnslp_field_key *copy() const = 0;	
	
	virtual bool operator ==(const mnslp_field_key &rhs) const = 0;
	
	/** 
	 * Less operator.
	*/ 
	virtual bool operator< (const mnslp_field_key& rhs) const = 0; 
	
	/** 
	 * Not equal to operator. 
	*/ 
	virtual bool operator != (const mnslp_field_key &rhs) const = 0;
	
	/**
	 * Print into string the field key 
	 */
	virtual std::string to_string() const = 0;
	
  protected:

	mnslp_field_key();
	mnslp_field_key(msnlp_field_key_type_t field_key_type);

  private:
	uint16 field_key_type;
};


  } // namespace msg
} // namespace mnslp


#endif // MNSLP_MSG__MNSLP_FIELD_KEY_H
