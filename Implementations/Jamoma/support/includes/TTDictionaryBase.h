/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief Foundation Dictionary Class.
 *
 * @details The dictionary is a data structure that combines the fast lookup of a hashtable,
 * but may be sorted like a linked-list.
 * The linked list contains the key-value pairs of the hash a linked-list of TTKeyValues.
 *
 * @author Timothy Place, Trond Lossius
 *
 * @copyright Copyright © 2010, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_DICTIONARY_BASEH__
#define __TT_DICTIONARY_BASEH__

#include "TTValue.h"
#include "TTList.h"


/****************************************************************************************************/
// Class Specification

typedef std::pair<TTPtrSizedInt,TTValue>			TTDictionaryBasePair;
#if defined( __ICC )
#include "boost/unordered_map.hpp"
typedef boost::unordered_map<TTPtrSizedInt, TTValue>	TTDictionaryBaseMap;
#else
#include <unordered_map>
typedef std::unordered_map<TTPtrSizedInt, TTValue>	TTDictionaryBaseMap;
#endif
typedef TTDictionaryBaseMap::iterator					TTDictionaryBaseMapIter;
typedef TTDictionaryBaseMap::const_iterator				TTDictionaryBaseMapIterK;


/** A type that contains a key and a value. */
//typedef pair<TTPtrSizedInt,TTValue>	TTKeyVal;
//typedef	TTKeyVal*			TTKeyValPtr;
//typedef void (*TTHashIteratorType)(TTPtr, const TTKeyVal&);



/**
	The dictionary is a data structure that combines the fast lookup of a hashtable, but may be sorted like a linked-list.
	This accomplished internally by maintaining the two data structures in parallel.

	Note that generally speaking you should not directly create a TTDictionaryBase!
	Instead, TTDictionaryBase objects should only by created by the instantiating a #TTDictionary.
*/
class TTFOUNDATION_EXPORT TTDictionaryBase {
	friend class TTDictionary;
	
private:
	
//	TTHashPtr	mHashTable;
//	TTListPtr	mList;
	TTDictionaryBaseMap	mMap;
	TTPtrSizedInt		mReferenceCount;
	TTList				mObservers;			///< List of all objects watching this object for life-cycle and other changes.
	
public:
	TTDictionaryBase();
	virtual ~TTDictionaryBase();
	
	// The copy assignment constructor doesn't appear to be involved, at least with resizes, on the Mac...
	// This operator is used when pushing to an append# object, however...
	TTDictionaryBase& operator=(const TTDictionaryBase& source)
	{
//		if (mHashTable)
//			delete mHashTable;
//		mHashTable = new TTHash(*source.mHashTable);
		
		mMap = source.mMap;
		
//		if (mList)
//			delete mList;
//		mList = new TTList(*source.mList);
		return *this;
	}
	
	
	/** TODO: Add documentation
	 @para schemaName			TODO: Add documentation
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setSchema(const TTSymbol schemaName);
	
	
	/** TODO: Add documentation
	 @return					TODO: Add documentation
	 */
	const TTSymbol getSchema() const;
	
	
	/** TODO: Add documentation
	 @param newValue			TODO: Add documentation
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setValue(const TTValue& newValue);
	
	
	/** TODO: Add documentation
	 @param returnedValue		TODO: Add documentation
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getValue(TTValue& returnedValue) const;
	
	/**	Get a dictionary's primary value
		@param	value		The returned value of the dictionary.
		@return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	template <class T>
	TTErr getValue(T& aReturnedValue) const
	{
		TTValue	v;
		TTErr error = getValue(v);
		aReturnedValue = v;
		return error;
	}
	
	
	/** Insert an item into the hash table. 
	 @param value	The value to instert.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr append(const TTSymbol key, const TTValue& value);
	
	
	/** Find the value for the given key.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr lookup(const TTSymbol key, TTValue& value) const;
	
	
	/** Remove an item from the hash table.
	 @param key					The key of the item to remove.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr remove(const TTSymbol key);
	
	
	/** Remove all items from the hash table.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr clear();
	
	
	/** Get an array of all of the keys for the hash table.
	 @param hashKeys			Used to return an array of all of the keys for the hash table
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getKeys(TTValue& hashKeys);
	
	
	/** Return the number of keys in the hash table.
	 @return					The number of keys in the hash table.
	 */
	TTUInt32 getSize();
	
	
	/** Return true if the hash has nothing stored in it. 
	 @return					TRUE if the hash has nothing stored in it.
	 */
	TTBoolean isEmpty();
	
	
	
	
	TTErr registerObserverForNotifications(const TTObjectBase& observingObject);
	TTErr unregisterObserverForNotifications(const TTObjectBase& observingObject);
	TTErr sendNotification(const TTSymbol name, const TTValue& arguments);

};

/** Pointer to a #TTDictionary.
 @ingroup typedefs
 */
typedef TTDictionaryBase* TTDictionaryBasePtr;


#endif // __TT_DICTIONARY_BASEH__
