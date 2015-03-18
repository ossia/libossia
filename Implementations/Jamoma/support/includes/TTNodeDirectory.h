/* 
 * TTNodeDirectory
 * Copyright © 2008, Théo de la Hogue & Tim Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#if !defined( __TT_NODE_DIRECTORY_H__ ) && !defined ( DISABLE_NODELIB )
#define __TT_NODE_DIRECTORY_H__

#ifdef WIN_VERSION
 #pragma warning(disable:4083) //warning C4083: expected 'newline'; found identifier 's'
#endif // WIN_VERSION

#include "TTObjectBase.h"
#include "TTSymbol.h"
#include "TTValue.h"
#include "TTHash.h"
#include "TTList.h"
#include "TTMutex.h"
#include "TTNode.h"
#include "TTAddress.h"
#include "TTAddressTable.h"

class TTNode;
typedef TTNode*	TTNodePtr;
class TTNodeDirectory;
typedef TTNodeDirectory* TTNodeDirectoryPtr;
class TTCallback;
typedef TTCallback* TTCallbackPtr;


/****************************************************************************************************/
// Class Specifications

/**
	We build a tree of TTNodes, and you can request a pointer for any TTNode, or add an observer to any TTNode, etc.
	
	Every time we create a TTNode (or a string passed to something somewhere?) then this gets added to the tree.  
	If a TTNode is specified, such as /degrade~/audio/mute, 
	and we assume degrade~ has been added to the tree but 'audio' hasn't, then 'audio' is added followed by adding 'mute', 
	so that the tree is properly completed.
	
	When a TTNode is requested, the tree is traversed to find the pointer for the TTNode.  
	After the traversal (assuming no wildcards) the pointer is returned and also stored 
	in a hash table so that future requests for that TTNode are faster.
	
	The case of wildcards is handled, because a request is cached (keyed on the request),
	and the value is a linked list of all of the matches.
	
*/

class TTFOUNDATION_EXPORT TTNodeDirectory {
	
private:

	TTSymbol		name;					///< the name of the tree
	TTNodePtr		root;					///< the root of the tree
	TTHash          directory;				///< a pointer to a global hashtab which reference all address of the tree
	TTHash          aliases;				///< a pointer to a global hashtab which reference all aliases of the tree
	TTHash          observers;				///< a pointer to a hashtab which register all life cycle observers below that node
											///< (address/relative/to/this/node, TTList of all observers below that address)
											///< e.g. so that you can receive notifications of changes to the tree of TTNodes
	TTMutexPtr		mutex;					///< a Mutex to protect the mObservers hash table.
	
public:
	
	/** Constructor */
	TTNodeDirectory (TTSymbol aName);
	
	/** Destructor */
	virtual ~TTNodeDirectory ();
	
	/** Initialize the TTNodeDirectory */
	TTErr			init();

	/** Set the name of the TTNodeDirectory. 
		@param	newName				The name to set */
	TTErr			setName(TTSymbol aname);

	/** Get the name of the TTNodeDirectory */
	TTSymbol		getName();
	
	/** Get the root of the TTNodeDirectory */
	TTNodePtr		getRoot();
	
	/** Get the directory of the TTNodeDirectory */
	TTHashPtr		getDirectory();

	/**	Given an address, return a pointer to a TTNode.
	 @param	anAddress				A pointer to the address for which to find the TTNode (it is replaced by the real address if it contains an alias).
	 @param	returnedTTNode			The .
	 @return						An error code. */
	TTErr			getTTNode(const char* anAddress, TTNodePtr *returnedTTNode);
	TTErr			getTTNode(TTAddress anAddress, TTNodePtr *returnedTTNode);
	
	/** Given an address, return an alias if exists
	 @param	anAddress				The address for which to find the TTNode.
	 @param	returnedAlias			The .
	 @return						An error code if there is no alias. */
	TTErr			getAlias(TTAddress anAddress, TTAddress& returnedAlias);
	
	/** Given an address with alias, return an address with no alias if exists
	 @param	anAddress				The address with an alias inside to replace.
	 @return						An error code if there is no alias. */
	TTErr			replaceAlias(TTAddress& anAddress);
    
    /** fill an address item using the TTNodeDirectory from an address
     @param anAddressItem           an address item
     @param startAddress            an address where to start from
     @return                        An error code if the address item cannont be filled */
	TTErr           fillAddressItem(TTAddressItemPtr anAddressItem, TTAddress startAddress = kTTAdrsRoot);
	
	/**	Find TTNodes by address
	 @param	anAddress				The address you wish to find, possibly including wildcards and instance names/numbers.
	 @param	returnedTTNodes			If non-null, a pointer will be set to the linked-list of TTNodes at the given address pattern.
	 @param	firstReturnedTTNode		If non-null, the address of the first TTNode object pointer that is found for the given pattern is returned here.  
									The value of the pointer will be set upon return.
	 @return						An error code. */
	TTErr			Lookup(TTAddress anAddress, TTList& returnedTTNodes, TTNodePtr *firstReturnedTTNode);
	
	/**	Find TTNodes by testing each TTNodes below an address 
	 @param	whereToSearch			A TTNode list from where to start the research
	 @param testFunction			the test function have to take a TTNode as first argument, and a pointer to something as second argument (a structure for example) 
									it have to return a boolean (true means that the node is ok).
	 @param argument				argument for the testFunction
	 @param	returnedTTNodes			If non-null, a pointer will be set to the linked-list of TTNodes at the given address pattern.
	 @param	firstReturnedTTNode		If non-null, the address of the first TTNode object pointer that is found for the given pattern is returned here.
									The value of the pointer will be set upon return.
	 @param	depthLimit				Precise the depth limit of the exploration (0 means no limit)
     @param comparisonfunction      To sort the children at each level
	 @return						An error code. */
	TTErr			LookFor(TTListPtr whereToSearch, TTBoolean(*testFunction)(TTNodePtr node, TTPtr args), void *argument, TTList& returnedTTNodes, TTNodePtr *firstReturnedTTNode, TTUInt8 depthLimit=0, TTBoolean(*comparisonFunction)(TTValue& v1, TTValue& v2)=NULL);
	
	/**	Is there is one TTNode or more that respect a test below an address 
	 @param	whereToSearch			A TTNode list from where to start the research
	 @param testFunction			the test function have to take a TTNode as first argument, and a pointer to something as second argument (a structure for example) 
	 it have to return a boolean (true means that the node is ok).
	 @param argument				argument for the testFunction.
	 @param	isThere					a pointer will be set to true if there is one node that respect the testFunction.
	 @param	firstReturnedTTNode		If non-null, the address of the first TTNode object pointer that is found for the given pattern is returned here.
									The value of the pointer will be set upon return.
	 @return						An error code. */
	TTErr			IsThere(TTListPtr whereToSearch, TTBoolean(*testFunction)(TTNodePtr node, void*args), void *argument, bool *isThere, TTNodePtr *firstTTNode);
	
	/**	Create a new TTNode, at the given location in the tree.
	 @param	anAddress				The address for which you wish to create a TTNode.
									The address may (optionally) include an instance name or number in the address of the terminal TTNode.
									If you specify an instance name/number that already exists, then returnedTTNode will be a pointer to
									the already existing TTNode upon return and no new TTNode will be created.
									If you do not specify an instance name/number, then one will be generated for you automatically.
	 @param	newObject				The object, if applicable, that is represented by this TTNode.
	 @param aContext				The context in which the object is.
	 @param	returnedTTNode			A pointer to the TTNode at the given address 
	 @param	nodeCreated				A boolean : true if a TTNode have been created, else false
	 @return						An error code. */
	TTErr			TTNodeCreate(TTAddress& anAddress, TTObject& newObject, void *aContext, TTNodePtr *returnedTTNode, TTBoolean *nodeCreated);
	
	/**	Remove a TTNodefrom the directory.
	 @param	anAddress				The address for which you wish to remove the TTNode.
									The address may (optionally) include an instance name or number in the address of the terminal TTNode.
	 @return						An error code. */
	TTErr			TTNodeRemove(TTAddress& anAddress);
	
	/**	Create an alias address
	 @param	alias					The alias address (absolute)
	 @param	anAddress				The address for which you wish to create an alias.
	 @return							An error code. */
	TTErr			AliasCreate(TTAddress alias, TTAddress anAddress);
	
	/**	Create an alias address
	 @param	alias					The alias address to remove
	 @return						An error code. */
	TTErr			AliasRemove(TTAddress alias);
	
	/** Add a TTCallback as a life cycle observer of all nodes below this one
	 @param anAddress				an address to observe
	 @param observer				a #TTCallback object to add
	 @param maxDepthDifference		precise the max depth difference to filter nodes which are below this limit
	 @return						an error code */
	TTErr			addObserverForNotifications(TTAddress anAddress, TTObject& anObserver, TTInt8 maxDepthDifference=-1);
	
	/** Remove a TTCallback as a life cycle observer of all nodes below this one
	 @param anAddress				an address
	 @param observer				a #TTCallback object to remove
	 @return						a kTTErrGeneric if there isn't observer */
	TTErr			removeObserverForNotifications(TTAddress anAddress, TTObject& anObserver);
	
	/** Notify life cycle observers that something appends below this TTNode
	 @param data					an address where something append
	 @param aNode					a TTNodePtr where something append (NULL in case of destruction)
	 @param flag					a flag to notify about creation or destruction (or anything else)
	 @return						a kTTErrGeneric if there isn't observer	*/
	TTErr			notifyObservers(TTAddress anAddress, TTNodePtr aNode, TTAddressNotificationFlag flag);
};

/**	An test tool : test the object stored inside the node.
 This method could be used as testFunction for the LookFor or IsThere methods.
 @param	node						A node
 @param args						An TTObjectBasePtr argument to retreive an object instance
 @return							true if the object is the same than the one passed in argument */
TTBoolean TTFOUNDATION_EXPORT testNodeObject(TTNodePtr n, TTPtr args);

/**	An test tool : test the type of the object stored inside the node. 
	This method could be used as testFunction for the LookFor or IsThere methods.
 @param	node						A node
 @param args						An TTSymbolRef argument for the type
 @return							true if the object have the correct type */
TTBoolean TTFOUNDATION_EXPORT testNodeObjectType(TTNodePtr n, TTPtr args);

/**	An test tool : test the context of the object stored inside the node. 
	This method could be used as testFunction for the LookFor or IsThere methods.
 @param	node						A node
 @param args						An TTSymbolRef argument for the type
 @return							true if the object have the correct context */
TTBoolean TTFOUNDATION_EXPORT testNodeContext(TTNodePtr n, TTPtr args);

/**	An test tool : test a node using a TTCallback. 
	This method could be used as testFunction for the LookFor or IsThere methods.
 @param	node						A node
 @param args						A TTCallback argument
 @return							true if the TTCallback argument is replaced by kTTVal1 */
TTBoolean TTFOUNDATION_EXPORT testNodeUsingCallback(TTNodePtr n, TTPtr args);

/**	An test tool : test a node using a list of filter from a bank.
	In default exclusion mode, if one field of a filter matches a node, this node is excluded.
	In inclusion mode, if all fields of a filter match a node, this node is included.
	This method could be used as testFunction for the LookFor or IsThere methods.
 @param	node						A node
 @param args						A TTValuePtr containing a TTHashPtr (the bank) and a TTListPtr (the name of the filter)
 @return							true if the node have to be include in the result */
TTBoolean TTFOUNDATION_EXPORT testNodeUsingFilter(TTNodePtr n, TTPtr args);

/**	An comparison tool : compare the priority attribute of to object's node then if equal the name and then the instance to sort them aphabeticaly
 This method could be used as comparison function to sort a node list.
 @param	v1						first node
 @param v2						second node
 @return						true if priority of first node is lower than the second node (but a priority of 0 is always higher than any other value)*/
TTBoolean TTFOUNDATION_EXPORT compareNodePriorityThenNameThenInstance(TTValue& v1, TTValue& v2);

/**	An method used to find an observer in the observers table */
void findObserver(const TTValue& value, TTPtr observerToMatch, TTBoolean& found);


#endif // __TT_NODE_DIRECTORY_H__
