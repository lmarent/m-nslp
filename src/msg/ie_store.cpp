/// ----------------------------------------*- mode: C++; -*--
/// @file ie_store.cpp
/// Store IEs by ID
/// ----------------------------------------------------------
/// $Id: ie_store.cpp 2558 2014-11-05  $
/// $HeadURL: https://./src/msg/ie_store.cpp $
// ===========================================================
//                      
// Copyright (C) 2012-2014, all rights reserved by
// - System and Computing Engineering, Universidad de los Andes
//
// More information and contact:
// https://www.uniandes.edu.co/
//                      
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ===========================================================
#include "logfile.h"

#include "msg/ie_store.h"


using namespace protlib;
using namespace protlib::log;


/**
 * Standard constructor.
 *
 * Creates an empty ie_store.
 */
ie_store::ie_store() : entries() {
	// nothing to do
}


/**
 * Copy constructor.
 *
 * Makes a deep copy of the object passed as an argument.
 *
 * @param other the object to copy
 */
ie_store::ie_store(const ie_store &other) {

	// copy all entries
	for (c_iter i = other.entries.begin(); i != other.entries.end(); i++) {
		const ie_object_key id = i->first;
		const IE *ie = i->second;

		if ( ie )
			set(id, ie->copy());
		else
			Log(ERROR_LOG, LOG_CRIT, "ie_store",
				"copy constructor: the other IE is NULL");
	}
}


/**
 * Destructor.
 *
 * Deletes all entries this ie_store contains.
 */
ie_store::~ie_store() {
	
	for ( c_iter i = entries.begin(); i != entries.end(); i++ )
		delete i->second;
	
	
}


bool ie_store::operator==(const ie_store &other) const throw() {

	// The number of entries has to be equal, otherwise the check
	// after this one doesn't work.
	
	std::cout << "in == step 1" 
			  << "left size:" << size() 
			  << "Right size" << other.size() << std::endl;
	
	if ( size() != other.size() )
		return false;

	std::cout << "in == step 2" << std::endl;

	// All entries have to be identical.
	for ( c_iter i = entries.begin(); i != entries.end(); i++ ) {
		const ie_object_key id = i->first;
		const IE *obj = i->second;
  
        std::cout << "in == step 3 -id" << id.to_string() << std::endl;

		IE *other_obj = other.get(id);
		
		// Note: other_obj can't be NULL, set() takes care of that.
		if ( other_obj == NULL || *obj != *other_obj ){
			if (other_obj == NULL)
				std::cout << "The other object is null" << std::endl;
			
			if (*obj != *other_obj)
				std::cout << "Diferent objects" << std::endl;
			return false;
		}
	}
	
	std::cout << "in == step 4" << std::endl;
	
	return true;	// no difference found
}


/**
 * Return the number of contained entries.
 *
 * @return the number of entries this ie_store contains
 */
size_t ie_store::size() const throw() {
	return entries.size();
}


/**
 * Returns the entry registered for a given ID.
 *
 * The object is not removed.
 *
 * @return the entry or NULL, if none is registered for that ID
 */
IE *ie_store::get(ie_object_key id) const throw() {

	/*
	 * Note: This is a true "const" implementation. Accessing the map
	 *       via entries[id] would modify it, even if no entry is found!
	 */
	c_iter i = entries.find(id);

	if ( i != entries.end() )
		return i->second;
	else
		return NULL;
}


/**
 * Add an entry.
 *
 * If there is already an entry registered with the same ID, then the old
 * one is deleted. Note that ie_store's destructor will delete all registered
 * entries. Because of this, IE objects may only be added once. Otherwise
 * the destructor would try to delete them twice.
 *
 * Adding a NULL ie is not allowed. In this case, no operation is performed.
 *
 * @param id the ID used as the key
 * @param ie the IE to add (NULL is not allowed)
 */
void ie_store::set(ie_object_key id, IE *ie) throw () {

	if ( ie == NULL )
		return;

	IE *old = entries[id];

	if ( old )
		delete old;

	entries[id] = ie;
}


/**
 * Remove an entry. 
 *
 * Remove the entry with the given ID from this ie_store. If there is no
 * entry with that ID, NULL is returned.
 *
 * The returned IE itself is *not* deleted.
 *
 * @param id an entry's ID
 * @return the entry with that ID or NULL if there is none
 */
IE *ie_store::remove(ie_object_key id) throw () {
	IE *ie = entries[id];

	// Note: entries[id] created an entry, even if none existed before!
	entries.erase(id);

	return ie; // either the IE or NULL
}


/**
 * Get the max sequence number inserted for an object key. 
 *  
 * @param id an entry's ID
 * @return the max sequence number with that object type ID or 0 if there is none
 */
uint32 
ie_store::getMaxSequence(uint32 id) const {

	uint32 result = 0;
	bool ingress = false;

	// All entries have to be of the same type.
	for ( c_iter i = entries.begin(); i != entries.end(); i++ ) {
		const ie_object_key key = i->first;
		if ( key.get_object_type() == id ){
			if (result <= key.get_sequence_number())
				result = key.get_sequence_number();
				ingress = true;
		}
	}
	// If not found return -1 (an invalid index).
	if (ingress == false)
		result = -1;
	
	return result;
}

// EOF
