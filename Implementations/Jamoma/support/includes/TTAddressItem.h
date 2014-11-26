/* 
 * TTAddressItem
 * Copyright © 2012, Théo de la Hogue
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#if !defined( __TT_NODE_ADDRESS_ITEM_H__ ) && !defined ( DISABLE_NODELIB )
#define __TT_NODE_ADDRESS_ITEM_H__

#ifdef WIN_VERSION
 #pragma warning(disable:4083) //warning C4083: expected 'newline'; found identifier 's'
#endif // WIN_VERSION

#include "TTList.h"
#include "TTAddress.h"

class TTAddressItem;
typedef TTAddressItem*	TTAddressItemPtr;

/****************************************************************************************************/
// Class Specifications

/**
 
 */

class TTFOUNDATION_EXPORT TTAddressItem : public TTList
{
	
private:
	
	TTSymbol				symbol;					// any symbol
	TTAddressItemPtr		parent;					// the parent item
    TTListPtr               options;				// any optionnal symbols (like attribute, message)
	TTBoolean				selection;				// selection state of the item
	TTList					handlers;				// list of all TTObjectBase handling the item
	
public:
	
	/** Constructor */
	TTAddressItem (TTSymbol aSymbol=kTTSymEmpty, TTAddressItemPtr aParent=NULL, TTBoolean aSelection=NO);
	
	/** Destructor */
	virtual ~TTAddressItem ();
	
	/** Set the parent of the item
	 @param	newParent					a namespace item pointer
	 @return							a error code	*/
	void						setParent(const TTAddressItemPtr newParent);

	/** Set the selection state of the item
		@param	newSelectionState		YES to select the item
		@return							a error code	*/
	void						setSelection(const TTBoolean newSelectionState, TTBoolean recursively=NO);

	/** Get the symbol of the item */
	TTSymbol&					getSymbol();
	
	/** Get the parent of the item */
	TTAddressItemPtr			getParent();
    
    /** Get the options of the item */
	TTListPtr                   getOptions();
	
	/** Get the selection state of the item */
	TTBoolean					getSelection();
	
	/** Get an Item below this item */
	TTAddressItemPtr			getItem(TTSymbol aSymbol);
	
	/** Overwrite the TTList::clear() method to delete all items below */
	void						clear();
	
	/** Overwrite the TTList::append() method to append a node address */
	TTErr						append(TTAddress addressToAppend, TTAddressItemPtr *returnedItem);
	
	/** Overwrite the TTList::remove() method to remove a node address */
	TTErr						remove(TTAddress addressToRemove);
	
	/** Overwrite the TTList::current() method to access directly to the current item */
	TTAddressItemPtr			current();
	
	/** Overwrite the TTList::find() method to find a node address
		Note : if it fails, it returns the TTAddressItem where the find fails */
	TTErr						find(TTAddress addressToFind, TTAddressItemPtr *returnedItem);
	
	/** Merge an item */
	TTErr						merge(const TTAddressItemPtr anItemToAppend);
	
	/** Destroy an item and his parent if he's empty */
	TTErr						destroy(const TTAddressItemPtr anItemToRemove);
	
	/** Check if a similar item exist and returns the last tested item */
	TTBoolean					exist(TTAddressItemPtr anItemToCheck, TTAddressItemPtr *returnedItem);

	/** Copy the item into a given item */
	TTErr						copy(TTAddressItemPtr *anItemCopy);
	
	/** Register a TTObjectBase handler */
	void						registerHandler(TTObject& anObject);
	
	/** Unregister a TTObjectBase handler */
	void						unregisterHandler(TTObject& anObject);
	
	/** Send a message to all handlers */
	void						iterateHandlersSendingMessage(TTSymbol& messageName);
	void						iterateHandlersSendingMessage(TTSymbol& messageName, TTValue& aValue);
	
	friend void TTFOUNDATION_EXPORT TTAddressItemFind(const TTValue& itemValue, TTPtr aSymbolBaseToMatch, TTBoolean& found);
};

/* a TTFunctionMatch to find an item */
void TTFOUNDATION_EXPORT TTAddressItemFind(const TTValue& itemValue, TTPtr aSymbolBaseToMatch, TTBoolean& found);

	
#endif // __TT_NODE_ADDRESS_ITEM_H__
