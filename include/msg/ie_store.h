/*
 * ie_store.h - Store IEs by ID
 *
 * $Id: ie_store.h 2276 2014-11-05  $
 * $HeadURL: https://./include/msg/ie_store.h $
 */
#ifndef PROTLIB__IE_STORE_H
#define PROTLIB__IE_STORE_H

#include <map>

#include "ie.h"
#include "ie_object_key.h"


namespace protlib 
{


/**
 * Stores (ID, IE) mappings.
 *
 * This is a helper class intented for internal use. It mainly specializes
 * std::map and takes care of memory management issues.
 */
class ie_store 
{
	
  public:
	ie_store();
	ie_store(const ie_store &other);
	~ie_store();

	size_t size() const throw();
	IE *get(ie_object_key id) const throw();
	void set(ie_object_key id, IE *ie) throw();
	IE *remove(ie_object_key id) throw();

	bool operator==(const ie_store &other) const throw();
	uint32 getMaxSequence(uint32 id) const;

	typedef std::map<ie_object_key, IE *>::const_iterator const_iterator;

	const_iterator begin() const throw() { return entries.begin(); }
	const_iterator end() const throw() { return entries.end(); }

  private:
	/**
	 * Maps IDs to IEs.
	 *
	 * Note: Don't use __gnu_cxx::hash_map here. It is vector-based and
	 *       *extremely* expensive to initialize. The constructor eats
	 *       up much more processing time than we can ever gain by the
	 *       cheaper lookup method.
	 */
	std::map<ie_object_key, IE *> entries;

	/**
	 * Shortcut.
	 */
	typedef const_iterator c_iter;
};


} // namespace protlib

#endif // PROTLIB__IE_STORE_H
