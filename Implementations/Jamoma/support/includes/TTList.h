/* 
 * TTBlue (Linked) List Class
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_LIST_H__
#define __TT_LIST_H__

#include "TTBase.h"
#include "TTValue.h"
#include "TTMutex.h"
#include <list>

class TTObjectBase;
typedef std::list<TTValue>				TTLinkedList;
typedef std::list<TTValue>::iterator	TTListIter;
typedef TTObjectBase*					TTObjectBasePtr;

/****************************************************************************************************/
// Class Specification

class TTFOUNDATION_EXPORT TTList {
private:
	TTBoolean	mThreadProtection;	///< Use thread safety mechanisms.  Only disable this if you are certain that you will be calling from a single thread.
	TTMutexPtr	mMutex;

	#ifdef TT_PLATFORM_WIN
	#pragma warning(disable:4251)
	#endif
	TTLinkedList	theList;
	TTListIter		theIter;
	
	void lock();
	void unlock();
	
public:
	TTList();
	virtual ~TTList();
	TTList(TTList& that);

	/** Determine the number of values in the list.
		@return	The count of the values in the list.	*/
	TTUInt32 getSize();
	
	/** Determine whether the list has any items.
		@return	True if the list is empty, otherwise false.	*/
	TTBoolean isEmpty();
	
	/** Return the first value in the list. 
		@return	The first value in the list.			*/
	TTValue& getHead();
	
	/** Return the last value in the list.
		@return	The last value in the list.				*/
	TTValue& getTail();

	/** Put the iterator pointing to the beginning of the list. */
	void begin();

	/** Is the iterator points to the end of the list ?				
		@return false if the iterator is at the end of the list	*/
	bool end();

	/** Put the iterator pointing to the next position	*/
	void next();

	/** Put the iterator pointing to the prev position	*/
	void prev();

	/** Returns the current value in the list (where the iterator is).
		@return	The current value.						*/
	TTValue& current();
	
	/**	Return a value by it's location in the list.	*/
	TTErr getIndex(TTUInt32 index, TTValue& returnedValue);
	
	/** Appends a value to the list. 
		@param	newValue	The value to add to the list.  */
	void append(const TTValue& newValue);
	
	
	/**	If we don't define a version of this function that takes a pointer, 
		then when a pointer is provided a new temporary TTValue is created to provide the reference
		and then when the temporary is deleted we end up with a corrupt entry in the linked list.		*/
/*	void append(const TTValuePtr newValue)
	{
		append(*newValue);
	}
*/	
	void appendUnique(const TTValue& newValue);
	
	/** Insert a value into the list at a given location.
		@param	index		The location of the value after insertion.
		@param	newValue	The value to add to the list.
		@return	kTTErrGeneric if the value can't be inserted (bad index) */
	void insert(TTUInt32 index, const TTValue& newValue);
	
	/**	If we don't define a version of this function that takes a pointer, 
	 then when a pointer is provided a new temporary TTValue is created to provide the reference
	 and then when the temporary is deleted we end up with a corrupt entry in the linked list.		*/
/*	void insert(TTUInt32 index, const TTValuePtr newValue)
	{
		insert(index, *newValue);
	}
*/	
	/** Appends a list to the list.  
		@param	newList	The list to add to the list.  */
	void merge(TTList& newList);
	
	/** Sort a list using a comparison function
		@param	comparisonFunction which return true if the first element have to be before the second */
	void sort(TTBoolean(comparisonFunction)(TTValue&, TTValue&) = NULL);
	
	/** Find a value in the list by using a passed-in matching function. */
	TTErr find(TTFunctionMatch aMatchFunction, TTPtr aBaton, TTValue& returnedValue);

	/** Find a value in the list that is equal to a value passed-in. */
	TTErr findEquals(const TTValue& valueToCompareAgainst, TTValue& foundValue);
	
	/** Remove the specified value. 
		This doesn't change the value or free it, it just removed the pointer to it from the list.
		@param	The value to remove.					*/
	void remove(const TTValue& value);
	
	/** Remove all values from the list					*/
	void clear();
	
	/**	Remove all values from the list and free them.	*/
	void free();

	/**	Assign the contents of the list to a value as an array.	*/
	void assignToValue(TTValue& value);
	

	/**	Traverse the entire list, sending each item of the list to a specified function.	*/
	TTErr iterate(const TTObjectBasePtr target, const TTFunctionWithBatonAndValue callback);

	/**	Traverse the entire list, sending each item of the list to a specified object with the specified message.	*/
	TTErr iterate(const TTObjectBasePtr target, const TTSymbol messageName);
	
	/**	Traverse the entire list, and if the item in the list is an object, then send it the specified message.		*/
	TTErr iterateObjectsSendingMessage(const TTSymbol messageName);
	TTErr iterateObjectsSendingMessage(const TTSymbol messageName, TTValue& aValue);
	
	
	void setThreadProtection(TTBoolean threadProtection)
	{	
		mThreadProtection = threadProtection;
	}
};


typedef TTList* TTListPtr;


#endif // __TT_LIST_H__

