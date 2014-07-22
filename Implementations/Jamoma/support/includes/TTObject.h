/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief Create Jamoma object instances.
 *
 * @authors Timothy Place
 *
 * @copyright Copyright © 2012, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_OBJECT_H__
#define __TT_OBJECT_H__

#include "TTSymbol.h"

class TTObjectBase;
class TTValue;

/****************************************************************************************************/
// Class Specifications

/**
	Create and use Jamoma object instances.
	@see #TTObjectTest
*/
class TTFOUNDATION_EXPORT TTObject {
protected:
	friend class TTEnvironment;

	TTObjectBase*		mObjectInstance;

public:
	
	/** Constructor.
		 @param aClassName		The symbolic name of the class to create/wrap.
		 @param arguments		Arguments to the constructor.
	 */
	// NOTE: arguments *must* be copied -- otherwise a reference to kTTValNONE may overwrite its value and corrupt memory
	TTObject(const TTSymbol aClassName, const TTValue arguments);

	
	/** Constructor to create an empyt container which will be assigned/copied-to at a later point */
	TTObject(const TTSymbol aClassName);

	
	/** Constructor to create an empyt container which will be assigned/copied-to at a later point */
	TTObject();
	
	
	/** Special constructor to create new object that wraps an existing TTObjectBase pointer. 
		Use of the constructor is generally discouraged as is using TTObjectBase pointers in general.
	 */
	TTObject(TTObjectBase* anObjectBase);
	
	
	/** Copy constructor */
	TTObject(const TTObject& anObjectToCopy);
	
	
	/** Destructor.
	 */
	virtual ~TTObject();
	
		
	/** Query #TTEnvironment for names of all registered #TTObjectBase classes
     
     @param[out]	classNames      #TTValue whose content will be set to an array of #TTSymbol names for available classes
     @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	static TTErr GetRegisteredClassNames(TTValue& classNames);
    
    /** Query #TTEnvironment for names of all registered #TTObjectBase classes that share specific tags
     
     @param[out]	classNames      #TTValue whose content will be set to an array of #TTSymbol names for available classes     
     @param[in]     searchTags      #TTValue array containing the tags used to limit search
     @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	static TTErr GetRegisteredClassNamesForTags(TTValue& classNames, const TTValue& searchTags);
	
    /** Query #TTEnvironment for all registered tags used by #TTObjectBase classes
     
     @param[out]     tags           #TTValue whose content will be set to an array of #TTSymbol names for available tags
     @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
    static TTErr GetRegisteredTags(TTValue& tags);

	
	/**	Assign a TTObject instance to another TTObject
     */
    TTObject& operator = (TTObject object);
    
	
	/** Return a direct pointer to the internal instance.
		Not recommended in most cases. */
	TTObjectBase* instance() const;
	
	
	/**	Set an attribute value for an object
		@param	name			The name of the attribute to set.
		@param	value			The value to use for setting the attribute.
								This value can be changed(!), for example if the value is out of range for the attribute.
								Hence, it is not declared const.
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	template <class T>
	TTErr set(const TTSymbol aName, T aValue);
	
	
	/**	Get an attribute value for an object
		@param	name				The name of the attribute to get.
		@param	value				The returned value of the attribute.
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.	 
	 */
	template <class T>
	TTErr get(const TTSymbol aName, T& aReturnedValue) const;
	
	
	/** Return a list of names of the available attributes.
		@param attributeNameList		Pointer to a list of all attributes registered with this TTObjectBase.
	 */
	void attributes(TTValue& returnedAttributeNames) const;
    
	
    /** Return the type of an attribute as a symbol.
     @param aName                   The name of the attribute we want the type.
     @return #TTSymbol : kTTSym__none, kTTSym_uint8, kTTSym_int8, kTTSym_uint16, kTTSym_int16, kTTSym_uint32, kTTSym_int32, kTTSym_uint64, kTTSym_int64, kTTSym_float32, kTTSym_float64, kTTSym__boolean, kTTSym_symbol, kTTSym_string, kTTSym_pointer, kTTSym_object, kTTSym_value. Returns kTTSymEmpty if the attribute doesn't exist.
	 */
	TTSymbol attributeType(const TTSymbol aName);
	
	
	/** Return a list of names of the available messages.
		@param messageNameList		Pointer to a list of all messages registered with this TTObjectBase.
	 */
	void messages(TTValue& returnedMessageNames) const;
	
	
	/** Return the name of this class.
		@return					The name of this object.
	 */
	TTSymbol name() const;
	
	
	/** Send a message to this object with no arguments.
		@param aName	The name of the message to send.
		@return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr send(const TTSymbol aName);

	
	/** Send a message to this object with arguments.
		All arguments for input must be packed into a #TTValue container.
		Any return values from the message will be packed into a second #TTValue container, which you must provide.
		@param	aName			The name of the message to send.
		@param	anInputValue	Any additional arguments to the message.
		@param	anOutputValue	Will be filled-in with data upon return if the message returns data.
		@return					#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr send(const TTSymbol aName, const TTValue& anInputValue, TTValue& anOutputValue);
	
	
	/** Register an observer.
		The observer will be monitoring this object.

		@param anObservingObject	Reference to the observing object.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr registerObserverForNotifications(const TTObject& anObservingObject);
		
	
	/** Unregister an observer for notifications.
		The observer wiln no longer be monitoring.

		@param anObservingObject	Reference to the observing object.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr unregisterObserverForNotifications(const TTObject& anObservingObject);
		
	
	/**	Determine if the object contained by this TTObject is truly ready for use.
		@return If ready returns #YES otherwise #NO.
	 */
	TTBoolean valid() const;
    
    /**	Enable/Disable reference count tracking of the instance
     @param newTrackingValue        #YES to enable the tracking, #NO to disable it
	 */
	void track(TTBoolean newTrackingValue);
};


/** Compare two objects for equality. */
bool TTFOUNDATION_EXPORT operator == (const TTObject& anObject, const TTObject& anotherObject);

/** Compare two objects for inequality. */
bool TTFOUNDATION_EXPORT operator != (const TTObject& anObject, const TTObject& anotherObject);


/** Macro to access the actual C++ class that is contained inside of the #TTObject as a pointer.
	@details In general we want to avoid using casting to access C++ class member directly rather than through their dynamic interface, but there might be some exceptions. E.g., this macro is used in the DSP #TTFilterLib where we have a number of complex filters that are constructed of smaller building-block filters (examples are #TTHalfband3, #TTHalfband5, #TTHalfband9, #TTHilbert9, #TTHilbert33, #TTMirror5 and #TTMirrorBandpass10). The smaller building block filters are full citizen objects, but to call them through the usual calculate methods for each and every sample is incredibly inefficient when compared to using C++ calls that the compiler can inline.
	@ingroup macros
	@param instance_	The #TTObject to peek inside.
	@param class_		The C++ class name represented by the #TTObject.
	@return				Pointer to the C++ class contained inside of the #TTObject.
 */
#define TTBASE( instance_ , class_ )  ((class_*)instance_.instance())


#endif // __TT_OBJECT_H__
