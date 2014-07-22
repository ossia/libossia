/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A contextual subscriber to register #TTObject as #TTNode in a #TTNodeDirectory
 *
 * @details TODO: Add more info
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_SUBSCRIBER_H__
#define __TT_SUBSCRIBER_H__

#include "TTModularIncludes.h"

/*
 
 The aim of a subscriber is to make a node refering on an object of our environnement and, if needed,
 to create each upper nodes to make a path to this node in the tree structure.
 To do that we need :
	
	- the #TTObject to subscribe.
	- a Context : a pointer to a structural element of our environnement which contains the subscriber (e.g. a Max patcher, a Pd patcher, a html page, ...).
	- a relative address of the subscriber in this Context to get the name and the instance (but this could be ommited and be generated automatically).
	- a way to share the node which refers on that Context with other suscribers inside.
	- or, a list containing all Contexts above since a top level Context (a pointer to a top level element of our environnement) until the subscriber's Context.
 
 Then, if the Context node is shared, we just build the node refering on our object (with all inter level nodes if needed)
 else we insure that all Contexts node above are created before.
 
 */

class TTData;
typedef TTData* TTDataPtr;

class TTMODULAR_EXPORT TTSubscriber : public TTObjectBase
{
	
public:
	
	TTCLASS_SETUP(TTSubscriber)

private:
    
    TTObject                    mObject;					///< the object to subscribe
	
	TTAddress					mRelativeAddress;			///< the address of this subscriber relative to the Context node
	
	TTAddress					mNodeAddress;				///< cache the address of this subscriber in the tree structure
	
	TTAddress					mContextAddress;			///< cache the address of the Context node in the tree structure
	
	TTBoolean					mNewInstanceCreated;		///< a flag to know if a new instance has been automatically generated
    TTBoolean					mNewContextInstanceCreated; ///< a flag to know if a new context instance has been automatically generated
    
    TTBoolean					mSubscribed;                ///< a flag to know if a subcription is made
	
	TTHashPtr					mExposedMessages;
	TTHashPtr					mExposedAttributes;
	
	/** Make all needed registrations to set up a TTNode in the tree strucuture for the given mObject 
		Note : if anObject is NULL the process will only retrieve all context info. This is usefull 
		to know the namespace without subscribing to it	
		Important Note : 
			- the top level context have to be the first element of the list.
			- each element have to be TTValue with < context name, context pointer */
	TTErr Subscribe(const TTValue& inputValue, TTValue& outputValue);
    
    /** */
    TTErr Unsubscribe();
	
	/** Register each given Context of the list as TTNode if they don't exist yet and return the Context node */
	TTNodePtr registerContextList(TTListPtr aContextList);
	
	friend TTErr TTMODULAR_EXPORT TTSubscriberMessageReturnValueCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTSubscriberAttributeReturnValueCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTSubscriberAttributeObserveValueCallback(const TTValue& baton, const TTValue& data);
	
public:
	
	/** Expose a message of any #TTObject as #TTData in the same context than subscribed object */
	TTErr exposeMessage(TTObject anObject, TTSymbol messageName, TTObject& returnedData);
	
	/** Expose an attribute of any #TTObject as #TTData (parameter or return) in the same context than subscribed object */
	TTErr exposeAttribute(TTObject anObject, TTSymbol attributeName, TTSymbol service, TTObject& returnedData);
	
	/** Remove message exposition */
	TTErr unexposeMessage(TTSymbol messageName);
	
	/** Remove attribute exposition */
	TTErr unexposeAttribute(TTSymbol attributeName);
};

typedef TTSubscriber* TTSubscriberPtr;

TTErr TTMODULAR_EXPORT TTSubscriberMessageReturnValueCallback(const TTValue& baton, const TTValue& data);

TTErr TTMODULAR_EXPORT TTSubscriberAttributeReturnValueCallback(const TTValue& baton, const TTValue& data);

TTErr TTMODULAR_EXPORT TTSubscriberAttributeObserveValueCallback(const TTValue& baton, const TTValue& data);

#endif // __TT_SUBSCRIBER_H__
