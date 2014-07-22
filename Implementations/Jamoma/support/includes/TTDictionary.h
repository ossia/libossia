/** @file
	@ingroup foundationLibrary

	@brief Foundation Dictionary Class.

	@details 
	The dictionary is a data structure that combines the fast lookup of a hashtable, but may be sorted like a linked-list.
	Dictionaries are associated with symbols.

	@authors Timothy Place, Trond Lossius

	@copyright Copyright © 2010, Timothy Place @n
	This code is licensed under the terms of the "New BSD License" @n
	http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_DICTIONARY_H__
#define __TT_DICTIONARY_H__

#include "TTDictionaryBase.h"
#include "TTSymbolCache.h"

class TTObject;


/****************************************************************************************************/
// Class Specification

/** Map names to internal dictionary instances */
typedef std::unordered_map<TTPtr, TTDictionaryBase*>	TTDictionaryTable;
//typedef std::unordered_map<TTPtrSizedInt, void*>	TTDictionaryTable;

extern TTFOUNDATION_EXPORT TTDictionaryTable gTTDictionaryTable;

/** A type that represents the key as a C-String and the value as a pointer to the matching TTSymbol object. */
//typedef std::pair<TTString, TTSymbolBase*>				TTSymbolTablePair;


/** An iterator for the STL hash_map used by TTSymbolTable. */
//typedef TTSymbolTableHash::const_iterator			TTSymbolTableIter;


/**
 Create and destroy Jamoma object instances.
 */
class TTFOUNDATION_EXPORT TTDictionary {
protected:
//	friend class TTEnvironment;
	
	TTDictionaryBasePtr		mDictionaryInstance;
	TTSymbol				mName;
	
public:
	
	/** Constructor.
		@param aDictionaryName			The symbolic name associated with this dictionary.
										If you pass an empty symbol then a random name with be generated and returned here.
		@param aDictionaryWasCreated	Upon return this will be true if a new dictionary was created 
										rather than an existing dictionary being referenced.
	 */
	TTDictionary(TTSymbol aDictionaryName, TTBoolean& aDictionaryWasCreated)
	{
		create(aDictionaryName, kTTSym_none, aDictionaryWasCreated);
	}
	
	TTDictionary(TTSymbol aDictionaryName)
	{
		TTBoolean dictionaryCreated;
		create(aDictionaryName, kTTSym_none, dictionaryCreated);
	}
	
	TTDictionary()
	{
		TTBoolean dictionaryCreated;
		create(TTSymbol::random(), kTTSym_none, dictionaryCreated);
	}
	
private:
	void create(TTSymbol aDictionaryName, TTSymbol aSchemaName, TTBoolean& aDictionaryWasCreated)
	{
		if (aDictionaryName == "")
			aDictionaryName = TTSymbol::random();
		
		mDictionaryInstance = gTTDictionaryTable[aDictionaryName.rawpointer()];
		if (!mDictionaryInstance) {
			mDictionaryInstance = new TTDictionaryBase;
			gTTDictionaryTable[aDictionaryName.rawpointer()] = mDictionaryInstance;
			setSchema(aSchemaName);
			aDictionaryWasCreated = true;
		}
		else
			aDictionaryWasCreated = false;
		
		mName = aDictionaryName;
		mDictionaryInstance->mReferenceCount++;
	}
	
public:
	TTDictionary(TTElement* do_not_use_this_unless_your_name_is_ttelement_and_you_are_a_destructor)
	{
		mName = TTSymbol(do_not_use_this_unless_your_name_is_ttelement_and_you_are_a_destructor->mValue.dictionary);
		mDictionaryInstance = gTTDictionaryTable[mName.rawpointer()];
		// DO NOT INCREASE THE REFERENCE COUNT -- THIS IS A SPECIAL CONSTRUCTOR USED FOR TTELEMENT'S DESTRUCTOR
	}
	
	
	/** Copy Constructor */
	TTDictionary(const TTDictionary& aSourceDictionary)
	{
		mName = aSourceDictionary.mName;
		mDictionaryInstance = aSourceDictionary.mDictionaryInstance;
		mDictionaryInstance->mReferenceCount++;
	}

	
	/** Destructor.
	 */
	virtual ~TTDictionary()
	{
		mDictionaryInstance->mReferenceCount--;
		if (mDictionaryInstance->mReferenceCount == 0) {
			gTTDictionaryTable.erase(mName.rawpointer());
			delete mDictionaryInstance;
			mDictionaryInstance = NULL;
		}
	}
	
	
	/** Return the name associated with the dictionary. */
	TTSymbol name() const
	{
		return mName;
	}
	
	
	
	
	/** Register an observer.
	 The observer will be monitoring this object.
	 
	 @param anObservingObject	Reference to the observing object.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr registerObserverForNotifications(const TTObjectBase& anObservingObject);
	TTErr registerObserverForNotifications(const TTObject& anObservingObject);
	
	
	/** Unregister an observer for notifications.
	 The observer wiln no longer be monitoring.
	 
	 @param anObservingObject	Reference to the observing object.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr unregisterObserverForNotifications(const TTObjectBase& anObservingObject);
	TTErr unregisterObserverForNotifications(const TTObject& anObservingObject);
	
	
	// The copy assignment constructor doesn't appear to be involved, at least with resizes, on the Mac...
	// This operator is used when pushing to an append# object, however...
	TTDictionary& operator=(const TTDictionary& aSource)
	{
		(*mDictionaryInstance) = (*aSource.mDictionaryInstance);
		return *this;
	}
	
	
	/** TODO: Add documentation
	 @para schemaName			TODO: Add documentation
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setSchema(const TTSymbol aSchemaName)
	{
		return mDictionaryInstance->setSchema(aSchemaName);
	}
	
	
	/** TODO: Add documentation
	 @return					TODO: Add documentation
	 */
	const TTSymbol getSchema() const
	{
		return mDictionaryInstance->getSchema();
	}
	
	
	/** TODO: Add documentation
	 @param newValue			TODO: Add documentation
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setValue(const TTValue aNewValue) {
		return mDictionaryInstance->setValue(aNewValue);
	}
	
	
	/**	Get the dictionary's primary value
		 @param	name				The name of the attribute to get.
		 @param	value				The returned value of the attribute.
		 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	template <class T>
	TTErr getValue(T& aReturnedValue) const
	{
		return mDictionaryInstance->getValue(aReturnedValue);
	}
	
	
	/** Insert an item into the hash table. 
	 @param value	The value to instert.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr append(const TTSymbol aKey, const TTValue aValue)
	{
		return mDictionaryInstance->append(aKey, aValue);
	}
	
	
	/** Find the value for the given key.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr lookup(const TTSymbol aKey, TTValue& aValue) const
	{
		return mDictionaryInstance->lookup(aKey, aValue);
	}
	
	
	/** Remove an item from the hash table.
	 @param key					The key of the item to remove.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr remove(const TTSymbol aKey)
	{
		return mDictionaryInstance->remove(aKey);
	}
	
	
	/** Remove all items from the hash table.
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr clear()
	{
		return mDictionaryInstance->clear();
	}
	
	
	/** Get an array of all of the keys for the hash table.
	 @param hashKeys			Used to return an array of all of the keys for the hash table
	 @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getKeys(TTValue& aSetOfKeys)
	{
		return mDictionaryInstance->getKeys(aSetOfKeys);
	}
	
	
	/** Return the number of keys in the hash table.
	 @return					The number of keys in the hash table.
	 */
	TTPtrSizedInt size()
	{
		return mDictionaryInstance->getSize();
	}
	
	
	/** Return true if the hash has nothing stored in it. 
	 @return					TRUE if the hash has nothing stored in it.
	 */
	TTBoolean empty()
	{
		return mDictionaryInstance->isEmpty();
	}
	
};

/** Pointer to a #TTDictionary.
 @ingroup typedefs
 */
typedef TTDictionary* TTDictionaryPtr;


#endif // __TT_DICTIONARY_H__
