/*
 * The Selection Metering Entities Object.
 *
 * $Id: selection_metering_entities.h 2895 2014-11-05 $
 * $HeadURL: https://./include/msg/selection_metering_entities.h $
 */
#ifndef MNSLP_MSG__MSPEC_OBJECT_H
#define MNSLP_MSG__MSPEC_OBJECT_H

#include "mnslp_object.h"


namespace mnslp {
 namespace msg {

    using namespace protlib;



/**
 * \class msnlp_mspec_object
 *
 * \brief The generic metering spec object.
 * 
 * RFC definition: the MSPEC objects describe the actual configuration information.
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
class mnslp_mspec_object : public mnslp_object
{

  public:
	
	/*
	 * Inherited from IE
	 */
	virtual mnslp_mspec_object *new_instance() const =0;
	virtual mnslp_mspec_object *copy() const = 0;

	virtual const char *get_ie_name() const = 0;
	virtual size_t get_serialized_size(coding_t coding) const = 0;
	
	virtual ~mnslp_mspec_object();

	virtual bool isEqual(const mnslp_mspec_object &object) const = 0;
	
	virtual bool notEqual(const mnslp_mspec_object &object) const = 0;


  protected:

	mnslp_mspec_object();
			
	mnslp_mspec_object(uint16 object_type, treatment_t tr, bool _unique);

	virtual bool check_body() const = 0;
	virtual bool equals_body(const mnslp_object &obj) const = 0;

	virtual bool deserialize_body(NetMsg &msg, uint16 body_length,
			IEErrorList &err, bool skip) = 0;

	virtual void serialize_body(NetMsg &msg) const = 0;
	
};


 } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__MSPEC_OBJECT_H
