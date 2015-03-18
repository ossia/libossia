/** @file
	@ingroup foundationLibrary

	@brief The Jamoma Object Base Class.

	@details Base class for all first-class Jamoma objects.
	Internal objects may inherit directly from #TTObjectBase,
	but most objects will inherit from #TTDataObjectBase or #TTAudioObjectBase.

	@author Timothy Place, Theo de la Hogue, Trond Lossius

	@copyright Copyright © 2008, Timothy Place @n
	This code is licensed under the terms of the "New BSD License" @n
	http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_OBJECTBASE_H__
#define __TT_OBJECTBASE_H__

#include "TTBase.h"
#include "TTList.h"
#include "TTHash.h"
#include "TTSymbol.h"
#include "TTSymbolTable.h"
#include "TTSymbolCache.h"
#include "TTValue.h"
#include "TTSymbolCache.h"

// forward declarations needed by the compiler
class TTAttribute;
class TTMessage;
class TTObjectBase;
class TTClass;

typedef TTAttribute*	TTAttributePtr;
typedef TTMessage*		TTMessagePtr;
typedef TTObjectBase*	TTObjectBasePtr;
typedef TTObjectBase**	TTObjectBaseHandle;
typedef TTObjectBase&	TTObjectBaseRef;
typedef TTClass*		TTClassPtr;


/** A type that can be used to store a pointer to a message for an object.
	@ingroup typedefs
 */
typedef TTErr (TTObjectBase::*TTMethod)(const TTSymbol methodName, const TTValue& anInputValue, TTValue& anOutputValue);


/** A type that can be used to call a message for an object that does not declare the name argument. 
	@ingroup typedefs
 */
typedef TTErr (TTObjectBase::*TTMethodValue)(const TTValue& anInputValue, TTValue& anOutputValue);

/** A type that can be used to call a message for an object that does not declare the name argument. 
	@ingroup typedefs
 */
typedef TTErr (TTObjectBase::*TTMethodInputValue)(const TTValue& anInputValue);

/** A type that can be used to call a message for an object that does not declare the name argument. 
	@ingroup typedefs
 */
typedef TTErr (TTObjectBase::*TTMethodOutputValue)(TTValue& anOutputValue);

/** A type that can be used to call a message for an object that does not declare any arguments. 
	@ingroup typedefs
 */
typedef TTErr (TTObjectBase::*TTMethodNone)();


/** A type that can be used to store a pointer to a message for an object 
	@ingroup typedefs
 */
typedef TTErr (TTObjectBase::*TTGetterMethod)(const TTAttribute& attribute, TTValue& value);

/** A type that can be used to store a pointer to a message for an object 
	@ingroup typedefs
 */
typedef TTErr (TTObjectBase::*TTSetterMethod)(const TTAttribute& attribute, const TTValue& value);


/** Flags that determine the behavior of messages.
	@ingroup enums
 */
enum TTMessageFlags {
	kTTMessageDefaultFlags = 0,		///< The default set of flags will be used if this is specified.  At this time the default is #kTTMethodPassValue.
	kTTMessagePassNone = 1,			///< Set this flag if the method you are binding to this message is prototyped to accept no arguments.
	kTTMessagePassValue = 2,		///< Set this flag if the method you are binding to this message is prototyped with a single #TTValue& argument.
	kTTMessagePassNameAndValue = 4	///< Set this flag if the method you are binding to this message is prototyped with two arguments: a const #TTSymbol and a #TTValue&.
};
	
/** Flags that determine the behavior of messages. 
	@ingroup enums
 */
enum TTAttributeFlags {
	kTTAttrDefaultFlags = 0,		///< The default set of flags will be used if this is specified. At this time the default is #kTTAttrPassValueOnly.
	kTTAttrPassValueOnly = 1,		///< Attribute accessors will only be passed a reference to the attribute's value.
	kTTAttrPassObject = 2			///< Attribute accessors will first be passed a reference to the #TTAttribute object, then it will be passed  a reference to the attribute's value.
};


/****************************************************************************************************/
// Class Specifications

/**
	Base class for all first-class Jamoma objects.
	Internal objects may inherit directly from #TTObjectBase,
	but most objects will inherit from #TTDataObjectBase or #TTAudioObjectBase.
*/
class TTFOUNDATION_EXPORT TTObjectBase {
private:
	friend class TTEnvironment;

	TTClassPtr			classPtr;			///< The class definition for this object
	TTHash*				messages;			///< The collection of all messages for this object, keyed on the message name.
	TTHash*				attributes;			///< The collection of all attributes for this object, keyed on the attribute name.
protected:
	TTList				observers;			///< List of all objects watching this object for life-cycle and other changes.
private:
	TTList*				messageObservers;	///< List of all objects watching this object for messages sent to it.
	TTList*				attributeObservers;	///< List of all objects watching this object for changes to attribute values.
	TTAtomicInt			mLocked;			///< E.g. is this object busy doing something that would prevent us from being able to free it?
	TTUInt16			referenceCount;		///< Reference count for this instance.
public:
	TTBoolean			valid;				///< If the object isn't completely built, or is in the process of freeing, this will be false.
    TTBoolean			track;				///< enable the print of a log message to ease the track of reference counting mechanism on an instance
                                            ///< @see 
private:
	TTPtrSizedInt		reserved1;			///< Reserved -- May be used for something in the future without changing the size of the struct.
	//TTPtrSizedInt		reserved2;			///< Reserved -- May be used for something in the future without changing the size of the struct.

protected:
	/** Constructor.
		@param arguments					Arguments to the constructor.
	 */
	TTObjectBase(const TTValue arguments);
public:
	/** Destructor.
	 */
	virtual ~TTObjectBase();
	
	/**	Query an object to get its current reference count.	
		@return								Reference count.
	 */
	TTUInt16 getReferenceCount() {return referenceCount;}
    
    /**	Query an object to get the number of observers.
     @return								Observers list size.
	 */
	TTUInt32 getObserverCount() const {return observers.getSize();}
	
	/** @brief Register an attribute
		@details The theory on attributes is that the subclass calls registerAttribute()
				and the base class manages a list of all registered attributes.
				The the end-user calls setAttribute() on the object (which is defined in
				the base class only) and it dispatches the message as appropriate.
	 
		@param name				The name of the attribute
		@param type				The data type of the attribute
		@param address			Pointer to the address of the attribute
		@param getter			An optional getter method
		@param setter			An optional setter method
		@param newGetterObject	TODO: Document this
		@param newSetterObject	TODO: Document this
	 
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	*/
	TTErr registerAttribute(const TTSymbol name, const TTDataType type, void* address);
	TTErr registerAttribute(const TTSymbol name, const TTDataType type, void* address, TTGetterMethod getter);
	TTErr registerAttribute(const TTSymbol name, const TTDataType type, void* address, TTSetterMethod setter);
	TTErr registerAttribute(const TTSymbol name, const TTDataType type, void* address, TTGetterMethod getter, TTSetterMethod setter);
	TTErr registerAttribute(const TTSymbol name, const TTObjectBasePtr newGetterObject, const TTObjectBasePtr newSetterObject);
	
	
	/** Extend the attribute of an existing TTObjectBase to this TTObjectBase (using another attribute name)
		@param	name				The name of the attribute as you wish for it to be in your object (e.g. myFrequency).
		@param	extendedObject		A pointer to the object to which the attribute will be bound.
		@param	extendedName		The name of the attribute as defined by the object that you are extending (e.g. frequency).
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr extendAttribute(const TTSymbol name, const TTObjectBasePtr extendedObject, const TTSymbol extendedName);

	
	/** Remove an attribute.
		@param name				The name of the attribute to remove.
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr removeAttribute(const TTSymbol name);
	
	
	/** Find an attribute.
		@param name				The name of the attribute to find.
		@param attr				Return pointer to the attribute associated with the name.
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr findAttribute(const TTSymbol name, TTAttribute** attr);

	
	/**	Set an attribute value for an object
		@param	name			The name of the attribute to set.
		@param	value			The value to use for setting the attribute.
								This value can be changed(!), for example if the value is out of range for the attribute.
								Hence, it is not declared const.
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setAttributeValue(const TTSymbol name, TTValue& value);
	
	// special case for int literals
	TTErr setAttributeValue(const TTSymbol name, int value)
	{
		TTValue v(value);
		return setAttributeValue(name, v);
	}
	
	// special case for float literals
	TTErr setAttributeValue(const TTSymbol name, double value)
	{
		TTValue v(value);
		return setAttributeValue(name, v);
	}
	
	// special case for TTPtr
	TTErr setAttributeValue(const TTSymbol name, TTPtr value)
	{
		TTValue v(value);
		return setAttributeValue(name, v);
	}
	
	template <class T>
	TTErr setAttributeValue(const TTSymbol name, T& value)
	{
		TTValue	v(value);
		return setAttributeValue(name, v);
	}
	
	TTErr setAttributeValue(const TTSymbol name, const char* value)
	{
		TTSymbol s(value);
		TTValue v(s);
		return setAttributeValue(name, v);
	}
	
	/**	Set an attribute value for an object
		This is the same as calling setAttributeValue().
		@param	name			The name of the attribute to set.
		@param	value			The value to use for setting the attribute.
		This value can be changed(!), for example if the value is out of range for the attribute.
		Hence, it is not declared const.
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	template <class T>
	TTErr set(const TTSymbol aName, T aValue)
	{
		return setAttributeValue(aName, aValue);
	}

	
	/**	Get an attribute value for an object
		@param	name				The name of the attribute to get.
		@param	value				The returned value of the attribute.
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.	 
	 */
	TTErr getAttributeValue(const TTSymbol name, TTValue& value);
	
	template <class T>
	TTErr getAttributeValue(const TTSymbol name, T& value)
	{
		TTValue	v;
		TTErr error = getAttributeValue(name, v);
		value = v;
		return error;
	}

	/**	Get an attribute value for an object
		This is the same as calling getAttributeValue().
		@param	name				The name of the attribute to get.
		@param	value				The returned value of the attribute.
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	template <class T>
	TTErr get(const TTSymbol aName, T& aReturnedValue)
	{
		return getAttributeValue(aName, aReturnedValue);
	}
    
    /** Return the type of an attribute as a symbol.
     @param name                   The name of the attribute we want the type.
     @return #TTSymbol : kTTSym__none, kTTSym_uint8, kTTSym_int8, kTTSym_uint16, kTTSym_int16, kTTSym_uint32, kTTSym_int32, kTTSym_uint64, kTTSym_int64, kTTSym_float32, kTTSym_float64, kTTSym__boolean, kTTSym_symbol, kTTSym_string, kTTSym_pointer, kTTSym_object, kTTSym_value. Returns kTTSymEmpty if the attribute doesn't exist.
	 */
    TTSymbol getAttributeType(const TTSymbol name);
	
	/** Get the getterFlags of an attribute
		@param name					The name of the attribute that we are querying properties of.
		@param value					Pointer to attribute flags. Used for returning the result of the query.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getAttributeGetterFlags(const TTSymbol name, TTAttributeFlags& value);
	
	
	/** Set the getterFlags of an attribute
		@param name					The name of the attribute that we want to address.
		@param value					New values for the getterFlags property.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setAttributeGetterFlags(const TTSymbol name, TTAttributeFlags& value);
	
	
	/** Get the setterFlags of an attribute
		@param name					The name of the attribute that we are querying properies of.
		@param value					Pointer to attribute flags. Used for returning the result of the query.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getAttributeSetterFlags(const TTSymbol name, TTAttributeFlags& value);
	
	
	/** Set the setterFlags of an attribute
		@param name					The name of the attribute that we want to address.
		@param value					New values for the getterFlags property.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setAttributeSetterFlags(const TTSymbol name, TTAttributeFlags& value);

	
	/** Register an attribute property.
		@param attributeName			Tyhe name of the attribute for which we want to register a new property.
		@param propertyName			The name of the property.
		@param initialValue			The initial value of the property.
		@param getter					The getter method associated with the property.
		@param setter					The setter method associated with the property.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr registerAttributeProperty(const TTSymbol attributeName, const TTSymbol propertyName, const TTValue& initialValue, TTGetterMethod getter, TTSetterMethod setter);
	
	
	/** Register an message property.
		@param attributeName			Tyhe name of the message for which we want to register a new property.
		@param propertyName			The name of the property.
		@param initialValue			The initial value of the property.
		@param getter					The getter method associated with the property.
		@param setter					The setter method associated with the property.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr registerMessageProperty(const TTSymbol messageName, const TTSymbol propertyName, const TTValue& initialValue, TTGetterMethod getter, TTSetterMethod setter);
	
	
	/** Search for and locate an attribute.
		@param name					The name of the attribute we want to find.
		@param attributeObject			Pointer to the attribute object.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getAttribute(const TTSymbol name, TTAttributePtr* attributeObject)
	{
		return findAttribute(name, attributeObject);
	}
	
	/** Search for and locate a message.
		@param name					The name of the message we want to find.
		@param messageObject			Pointer to the message object.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getMessage(const TTSymbol name, TTMessagePtr* messageObject)
	{
		return findMessage(name, messageObject);
	}
	
	
	/** Return a list of names of the available attributes.
		@param attributeNameList		Pointer to a list of all attributes registered with this TTObjectBase.
	 */
	void getAttributeNames(TTValue& attributeNameList);
	
	
	/** Return a list of names of the available messages.
		@param messageNameList		Pointer to a list of all messages registered with this TTObjectBase.
	 */
	void getMessageNames(TTValue& messageNameList);
	
	
	/** Return the name of this class.
		@return					The name of this object.
	 */
	TTSymbol getName() const;

	
	/** Register a message with this object.
		@param name					The name of the message.
		@flags							Flags associated with the message.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr registerMessage(const TTSymbol name, TTMethod method);
	TTErr registerMessage(const TTSymbol name, TTMethod method, TTMessageFlags flags);
	
	
	/** Find a message registered with this object.
		@param name					The name of the message that we want to find.
		@param message					Pointer to the resulting message.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr findMessage(const TTSymbol name, TTMessage** message);
    
    /** Remove a message.
     @param name				The name of the message to remove.
     @return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
    TTErr removeMessage(const TTSymbol name);
	
	
	/** TODO: Document this function
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr sendMessage(const TTSymbol name);
#ifdef TT_SUPPORT_SINGLE_ARG_MESSAGE_CALLS
	TTErr sendMessage(const TTSymbol name, TTValue& anOutputValue);
	TTErr sendMessage(const TTSymbol name, const TTValue& anInputValue);
#endif
	TTErr sendMessage(const TTSymbol name, const TTValue& anInputValue, TTValue& anOutputValue);

    TTErr sendMessage(const TTSymbol name, const TTValue& anInputValue, TTNoValue /*ignore*/)
	{
		TTValue unusedReturnValue;
		return sendMessage(name, anInputValue, unusedReturnValue);
	}
	
// TODO: implement
//	TTErr registerMessageProperty(const TTSymbolRef messageName, const TTSymbolRef propertyName, const TTValue& initialValue);
	
	
	/** Register an observer for a message.
		The observer will be monitoring if this message is sent to the object.
		@param observingObject			Pointer to the observing object.
		@param messageName				The name of the message to monitor.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr registerObserverForMessage(const TTObject& observingObject, const TTSymbol messageName);
	
	
	/** Register an observer for an attribute.
		The observer will be monitoring if this attribute is changes.
		@param observingObject			Pointer to the observing object.
		@param attributeName			The name of the attribute to monitor.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr registerObserverForAttribute(const TTObject& observingObject, const TTSymbol attributeName);
	
	
	/** Register an observer.
		The observer will be monitoring this object.
		TODO: Exactly what do this imply? What is being observed?
		@param observingObject			Pointer to the observing object.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr registerObserverForNotifications(const TTObject& observingObject);
	
	
	/** Unregister an observer for a message.
		The observer will stop monitoring if this message is sent to the object.
		@param observingObject			Pointer to the observing object.
		@param messageName				The name of the message that no longer will be monitored.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr unregisterObserverForMessage(const TTObject& observingObject, const TTSymbol messageName);
	
	
	/** Unregister an observer for an attribute.
		The observer will stop monitoring changes to this attribute.
		@param observingObject			Pointer to the observing object.
		@param attributeName			The name of the attribute that no longer will be monitored.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr unregisterObserverForAttribute(const TTObject& observingObject, const TTSymbol attributeName);
	
	
	/** Unregister an observer for notifications.
		The observer wiln no longer be monitoring.
		TODO: Exactly what do this imply?
		@param observingObject			Pointer to the observing object.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr unregisterObserverForNotifications(const TTObject& observingObject);
	
	
	/** Send a notification.
		TODO: What do this imply?
		@param name					TODO: Document this
		@param arguments				TODO: Document this
	 */
	TTErr sendNotification(const TTSymbol name, const TTValue& arguments);
	
	
	/**	Log messages scoped to this object instance.
		@param fmtstring				The message to log
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr logMessage(TTImmutableCString fmtstring, ...);
	
	
	/**	Log warnings scoped to this object instance.
		@param fmtstring				The warning to log
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr logWarning(TTImmutableCString fmtstring, ...);
	
	
	/**	Log errors scoped to this object instance.
		@param fmtstring				The error to log
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr logError(TTImmutableCString fmtstring, ...);
	

	/**	Log messages (scoped to this object instance) to output only if the basic debugging flag is enabled in the environment.
		@param fmtstring				The message to log
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr logDebug(TTImmutableCString fmtstring, ...);
	
	
	/** Lock the object in order to ensure thread-safe processing.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	inline TTErr lock()
	{
		TTAtomicIncrement(mLocked);
		return kTTErrNone;
	}
	
	
	/** Unlock the object when thread-safe processing is no longer required.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	inline TTErr unlock()
	{
		TTAtomicDecrement(mLocked);
		return kTTErrNone;
	}
	
	
	/** Query if the object currently is  locked for thread-safe processing.
		@return						TRUE if the object is currently locked, else FALSE.
	 */
	inline TTBoolean isLocked()
	{
		return mLocked > 0;
	}
	
	
	/** If the object is locked (e.g. in the middle of processing a vector in another thread)
	 then we spin until the lock is released
	 TODO: We should also be able to time-out in the event that we have a dead lock.
	*/
	inline TTBoolean waitForLock()
	{
		while (isLocked())
			;
		return true; // TODO: might return false if the operation timed-out.
	}
	
};


#include "TTAttribute.h"
#include "TTMessage.h"


#define TT_OBJECT_CONSTRUCTOR_EXPORT \
	\
	extern "C" TT_EXTENSION_EXPORT TTErr loadTTExtension(void);\
	TTErr loadTTExtension(void)\
	{\
		TTFoundationInit();\
		thisTTClass :: registerClass(); \
		return kTTErrNone;\
	}\
	\
	TT_OBJECT_CONSTRUCTOR


#endif // __TT_OBJECTBASE_H__

