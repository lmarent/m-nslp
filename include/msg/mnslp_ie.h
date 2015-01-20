/*
 * The MNSLP IE Manager.
 *
 * $Id: mnslp_ie.h 1718 2014-11-05  $
 * $HeadURL: https://include/msg/mnslp_ie.h $
 */
#ifndef MNSLP_MSG__IE_H
#define MNSLP_MSG__IE_H

#include "ie.h"

namespace mnslp {
 namespace msg {
     using namespace protlib;


/**
 * Categories for MNSLP classes.
 */
enum category_t {
	cat_mnslp_msg			= 0,
	cat_mnslp_object		= 1,

	cat_default_mnslp_msg		= 2,
	cat_default_mnslp_object	= 3
};


/**
 * An Interface for reading/writing MNSLP Messages.
 *
 * The MNSLP_IEManager is a Singleton which provides methods to read/write
 * MNSLP Messages from/to NetMsg objects. Those methods are called
 * deserialize() and serialize(), respectively.
 *
 * To deserialize() a MNSLP Message, each IE to be used during the process
 * has to be registered with MNSLP_IEManager using register_ie(). Registered
 * IEs will be freed automatically as soon as either clear() is called or
 * MNSLP_IEManager itself is destroyed.
 *
 * The only way to get an MNSLP_IEManager object is through the static
 * instance() method.
 */
class MNSLP_IEManager : public IEManager {

  public:
	static MNSLP_IEManager *instance();
	static void clear();

	static void register_known_ies();

	virtual IE *deserialize(NetMsg &msg, uint16 category,
			IE::coding_t coding, IEErrorList &errorlist,
			uint32 &bytes_read, bool skip);

  protected:
	// protected constructor to prevent instantiation
	MNSLP_IEManager();
	
	virtual IE *lookup_ie(uint16 category, uint16 type, uint16 subtype);

  private:
	static MNSLP_IEManager *mnslp_inst;

	IE *deserialize_msg(NetMsg &msg,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip);

	IE *deserialize_object(NetMsg &msg,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip);
};


 } // namespace msg
} // namespace mnslp

#endif // MNSLP_MSG__IE_H
