/* 
 * TTBlue Object Base Class
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_MESSAGE_H__
#define __TT_MESSAGE_H__

#include "TTObjectBase.h"


/**	A convenience macro to be used by subclasses for registering messages.
	@ingroup macros
	@param	name	The name of the message, and also the name of the classes' method to be called.
*/
#define addMessage(name)					registerMessage(#name, (TTMethod)& thisTTClass ::name , kTTMessagePassNone )
#define registerMessageSimple(name)			registerMessage(#name, (TTMethod)& thisTTClass ::name , kTTMessagePassNone )


/**	A convenience macro to be used by subclasses for registering messages.
	@ingroup macros
	@param	name	The name of the message, and also the name of the classes' method to be called.
*/
#define addMessageWithArguments(name)		registerMessage(#name, (TTMethod)& thisTTClass ::name )
#define registerMessageWithArguments(name)	registerMessage(#name, (TTMethod)& thisTTClass ::name )


/** A convenience macro to be used for registering properties of messages.
 This assumes that the property is one that has been explicitly supported by TTMessage through the definition of accessor methods.
 If you are adding a custom property then you must define your own accessor methods and register the property by calling the
 TTObjectBase::registerMessageProperty() method directly.
 @ingroup macros
 */
#define addMessageProperty(messageName, propertyName, initialValue)		registerMessageProperty(#messageName, #propertyName, initialValue, (TTGetterMethod)& TTMessage::get##propertyName , (TTSetterMethod)& TTMessage::set##propertyName )


/**	An 'update' is a message sent to a subclass instance from its parent class.
	For example, to update the sample rate of the subclass when the parent's sample rate attribute has changed.
 @ingroup macros
 */
#define addUpdates(updateName)	addMessageWithArguments(update##updateName); addMessageProperty(update##updateName, hidden, YES);


/****************************************************************************************************/
// Class Specifications

/**
	This class represents a single message, as used by the TTObjectBase class.
	At the moment we define it in the same file because we are sharing the typedef
	for TTMethod.
*/
class TTFOUNDATION_EXPORT TTMessage : public TTObjectBase {
public:
	// Should make this group private, but to get things working initially, we're leaving them public...
	const TTSymbol		name;		///< the name of the message.
	TTMessageFlags		flags;		///< define the behavior of the message.
	TTMethod			method;		///< method associated with this message.
	TTBoolean			hidden;		///< Property: this message is private/hidden from outside usage
	TTSymbol			description;	///< Property: description of this message

	TTMessage(const TTSymbol newName, TTMethod newMethod, TTMessageFlags newFlags);
	virtual ~TTMessage();
	
	TTErr sethidden(const TTValue& newHiddenFlag);
	TTErr gethidden(TTValue& currentHiddenFlag);
	
	TTErr setdescription(const TTValue& newDescription);
	TTErr getdescription(TTValue& returnedDescription);
};


#endif // __TT_MESSAGE_H__

