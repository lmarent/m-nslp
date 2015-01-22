/*
 * Interface for M-NSLP fields.
 *
 * $Id: mnslp_field.h 2896 2015-01-20 10:27:00 amarentes $
 * $HeadURL: /include/msg/mnslp_field.h $
 *
 */
#ifndef MNSLP_MSG__MNSLP_FIELD_H
#define MNSLP_MSG__MNSLP_FIELD_H

#include "ie.h"
#include "mnslp_field_key.h"

namespace mnslp {
  namespace msg {
	using namespace protlib;

/**
 * The representation of a MNSLP field.
 *
 * This class is and abstract class for representing a field which belongs
 * to a mnslp_mpec_object. All concrete parameters have to be subclasses.
 */
class mnslp_field {

  public:
  
  	enum msnlp_field_type_t {
		mnslp_ipfix_type	= 0,
		mnslp_diameter_type	= 1,
		mnslp_netflow_type	= 2
	};
  
	virtual ~mnslp_field();

	uint16 get_field_type();
	virtual mnslp_field_key * get_field_key() const = 0;


  protected:

	mnslp_field();
	mnslp_field(msnlp_field_type_t field_type);

  private:
	uint16 field_type;
};


  } // namespace msg
} // namespace mnslp


/*
 * Include all known fields for convenience.
 */
#include "mnslp_ipfix_field.h"

#endif // MNSLP_MSG__MNSLP_FIELD_H
