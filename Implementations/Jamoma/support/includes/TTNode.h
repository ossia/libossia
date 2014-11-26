/** @file
 * @ingroup foundationLibrary
 *
 * @brief References an object using a name, an instance and any contextual environnement information
 *
 * @details some details about the class.
 
 * other details. @n@n
 
 * another details. @n@n
 *
 * @see TTApplicationManager, Protocol, TTMirror
 *
 * @copyright Copyright © 2008, Théo de la Hogue & Tim Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#if !defined( __TT_NODE_H__ ) && !defined ( DISABLE_NODELIB )
#define __TT_NODE_H__

#ifdef WIN_VERSION
 #pragma warning(disable:4083) //warning C4083: expected 'newline'; found identifier 's'
#endif // WIN_VERSION

#include "TTFoundationAPI.h"
#include "TTCallback.h"
#include "TTAddress.h"
#include "TTAddressTable.h"
#include "TTNodeDirectory.h"

class TTNode;
typedef TTNode*	TTNodePtr;

class TTNodeDirectory;
typedef TTNodeDirectory* TTNodeDirectoryPtr;

class TTCallback;
typedef TTCallback* TTCallbackPtr;

/****************************************************************************************************/
// Class Specifications

/**
 We build a directory of TTNodes, and you can request a pointer for any TTNode, or add an observer to any TTNode, etc.
 
 Every time we create a TTNode (or a string passed to something somewhere?) then this gets added to the directory.  
 If a TTNode is specified, such as /degrade~/audio/mute, 
 and we assume degrade~ has been added to the directory but 'audio' hasn't, then 'audio' is added followed by adding 'mute', 
 so that the directory is properly completed.
 
 When a TTNode is requested, the directory is traversed to find the pointer for the TTNode.  
 After the traversal (assuming no wildcards) the pointer is returned and also stored 
 in a hash table so that future requests for that TTNode are faster.
 
 The case of wildcards is handled, because a request is cached (keyed on the request),
 and the value is a linked list of all of the matches.
 */
class TTFOUNDATION_EXPORT TTNode {
	
private:

	TTSymbol			name;					///< the name of the node
	TTSymbol			instance;				///< an instance symbol (default to "")
	
	TTNodePtr			parent;					///< pointer to the parent node in the directory
	TTHashPtr			children;				///< a hashtab of hashtabs:
												///< hashed on children names which containing hashtabs hashed on instances

	TTObject            object;					///< an object referenced by the node
	TTPtr				context;				///< any pointer to precise the object place in its environnement (e.g. The owning t_object* in Max)

	TTNodeDirectoryPtr	directory;				///< pointer to a node directory
	
public:
	
	/** Constructor 
        @param aName                    the name of the node
        @param anInstance               an instance symbol
        @param anObject                 an object referenced by the node
        @param aContext                 any pointer to precise the object place in its environnement
        @param aDirectory               a pointer to a node directory */
	TTNode (TTSymbol& aName, TTSymbol& anInstance, TTObject& anObject, TTPtr aContext, TTNodeDirectoryPtr aDirectory);
	
	/** Destructor */
	virtual ~TTNode ();

	/** Set the name and the instance of the node
		@details It maintains the directory and the directory
		@param	aNameInstance			#TTAddress name.instance to set
		@param	newInstance				the returned #TTSymbol instance if a new have been created
		@param	newInstanceCreated		true if a new instance have been created
		@return	#TTErr 					an error code */
	TTErr			setNameInstance(TTAddress& aNameInstance, TTSymbol& newInstance, TTBoolean *newInstanceCreated);

	/** Set the parent of the node
		@details This method ensure that the path to the node exist
		@param	parentAddress			The address to check
		@param	newParentCreated		This parameter will be set to true upon return if a new TTNode was created, or false if one was not created - 
										e.g. because a TTNode already existed with this address and instance name.			
		@return	#TTErr 					an error code */
	TTErr			setParent(TTAddress parentAddress, TTBoolean *newParentCreated);

	/** Add a node as a child of the node
		@param child                    a TTNodePtr to add as children of the TTNode.
		@return #TTErr                  a kTTErrGeneric if the instance of the given child already exist. */
	TTErr			setChild(TTNodePtr child);
	
	/** Set the object of the node
        @param anObject                 the object to reference
        @return #TTErr                  always returns #kTTErrNone */
	TTErr			setObject(TTObject anObject = TTObject());

	/** Set the context of the node
        @param aContext                 any pointer usefull to precise a context
        @return #TTErr                  always returns #kTTErrNone */
	TTErr			setContext(TTPtr aContext);
	
	/** Get the name of the node
        @return #TTSymbol               the name of the node */
	TTSymbol&		getName();
	
	/** Get the instance of the node
        @return #TTSymbol               the instance of the node */
	TTSymbol&		getInstance();
	
	/** Get a pointer to the parent node of the node
        @return #TTNodePtr              the parent node */
	TTNodePtr		getParent();
	
	/** Get a linklist of children of the node : select them by name and instance (use wilcards to select them all)
        @return #TTErr                  an error code */
	TTErr			getChildren(TTSymbol& name, TTSymbol& instance, TTList& returnedChildren);
	
	/** Get a linklist of children name 
        @return #TTErr                  an error code */
	TTErr			getChildrenName(TTList& returnedChildrenName);
	
	/** Get a linklist of children instance for a given name 
        @return #TTErr                  an error code */
	TTErr			getChildrenInstance(TTSymbol& aName, TTList& returnedChildrenInstance);
	
	/** Get the object binded by this node. It return also NULL when the object is not valid 
        @return #TTObject               the referenced object */
	TTObject&       getObject();
	
	/** Get a pointer to the context of this node 
        @return #TTPtr                  the context pointer */
	TTPtr			getContext();

	/** Get the address of the node
	 @details It is computed dynamicaly by asking to all the ancestor of the node
	 @param	returnedAddress             a #TTAddress with the address is returned in this parameter.
	 @param	from                        an address from where to start the returned address in order to have a relative address
     @return #TTErr                     an error code*/
	TTErr			getAddress(TTAddress& returnedAddress, TTAddress from = kTTAdrsEmpty);

	/** Generate a new instance of a given child
		@param childName                the name of a child.
		@param newInstance              a new instance created (or "" if not)	.
		@return	#TTErr                  a kTTErrGeneric if the child doesn't exist.	*/
	TTErr			generateInstance(TTSymbol& childName, TTSymbol& newInstance);
};

	
#endif // __TT_NODE_H__
