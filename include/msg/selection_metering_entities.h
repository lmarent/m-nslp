/*
 * The Selection Metering Entities Object.
 *
 * $Id: selection_metering_entities.h 2895 2014-11-05 $
 * $HeadURL: https://./include/msg/selection_metering_entities.h $
 */
#ifndef MNSLP_MSG__SELECTION_METERING_ENTITIES_H
#define MNSLP_MSG__SELECTION_METERING_ENTITIES_H

#include "mnslp_object.h"


namespace mnslp {
 namespace msg {

    using namespace protlib;



/**
 * \class msg_sequence_number
 *
 * \brief The Selection Metering Entities Object.
 * 
 * RFC definition: This object is required to determine which MNEs will actually take
 *                 part in the metering.
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
class selection_metering_entities : public mnslp_object {

  public:
	static const uint16 OBJECT_TYPE = 0x00F5;
	
	enum selection_metering_entities_t {
		sme_all							= 1,
		sme_any							= 2,
		sme_first						= 3,  
		sme_last						= 4,  
		sme_first_last					= 5,
		sme_enterprise_specific			= 1024
	};

	explicit selection_metering_entities();
	explicit selection_metering_entities(uint32 _sme, 
										 treatment_t t = tr_mandatory,
										 bool _unique = true);

	virtual ~selection_metering_entities();

	virtual selection_metering_entities *new_instance() const;
	virtual selection_metering_entities *copy() const;

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
	/**
	 * Get the value specified for the selection metering entities
	 */ 
	uint32 get_value() const;
	
	/**
	 * Set the value specified for the selection metering entities
	 */ 
	void set_value(uint32 _sme);

	/**
	 * Assigment operator
	 */
	selection_metering_entities &operator=(const selection_metering_entities &other);

  private:

	static const char *const ie_name;

	uint32 sme;   			///< 4 bytes used.
};


 } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__SELECTION_METERING_ENTITIES_H
