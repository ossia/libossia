/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief TTEnvironment is a global object providing information on the environemt.
 *
 * @details The global TTEnvironent object maintains all global attributes/settings for the Jamoma environment.
 * All attribute members of the environment are made public, since essentially every class is a friend.
 * They should, however, be treated as read-only.
 *
 * @author Timothy Place, Nils Peters, Trond Lossius
 *
 * @copyright Copyright © 2008, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_ENVIRONMENT_H__
#define __TT_ENVIRONMENT_H__

#include "TTObjectBase.h"
#include "TTHash.h"


/**	A function pointer for an instance creation function required to be provided by all classes.
 @ingroup typedefs
 */
typedef TTObjectBase* (*TTObjectBaseInstantiationMethod)(TTSymbol className, const TTValue arguments);

/**	A function pointer for an instance creation function required to be provided by all classes.
 @ingroup typedefs
 */
typedef TTErr (*TTExtensionInitializationMethod)();

class TTClass;

/****************************************************************************************************/
// Class Specifications

/**
	The global object maintains all global attributes/settings for the Jamoma environment.
 
	All attribute members of the environment are made public, since essentially every class is a friend.
	They should, however, be treated as read-only.
*/
class TTFOUNDATION_EXPORT TTEnvironment : public TTObjectBase {
private:
	TTHash*		classes;			///< A hash keyed on classNames, and returning a TTClassPtr.
	TTHash*		tags;				///< A hash keyed on tag names, which map to TTLists of all classes with that tag name.
	
public:
	TTBoolean	mDebugBasic;		///< Attribute: Basic debug functionality is enabled when true.
	TTBoolean	mDebugMessaging;	///< Attribute: Should all message traffic be logged?
	TTUInt32	mSampleRate;		///< Current sample rate as understood by the environment as a whole.
	TTBoolean	mBenchmarking;		///< Attribute: Enable benchmarking in TTAudioObject and TTDataObject ?

	
	/**	Constructor	*/
	TTEnvironment();
	
	
	/**	Destructor */
	virtual ~TTEnvironment();

	
	/**	Retrieve the environment version number. 
	 @param anInputValue				This is ignored.
	 @param anOutputValue				Returns the current version number of the environment.
	 @return							#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getVersion(const TTValue& anInputValue, TTValue &anOutputValue);

	
	/**	Register the unit name, and associate it with the constructor to be called. 
		@param	className				The name of the class to register.
		@param	tags					A comma-delimited list of tags in a string.
		@param	anInstantiationMethod	A pointer to the C-function that is used to create a new instance of the class.
		@return							#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr registerClass(const TTSymbol className, const TTString& tagString, const TTObjectBaseInstantiationMethod anInstantiationMethod);

	// The above creates a class and registers it -- this one just registers a class after it is created.
	TTErr registerClass(TTClass* theClass);
	
	
	/** Remove a class from the environment's registry.	
		If the executable from which it is loaded is no longer referenced by other classes, then also unload the executable.
		@param	className	The name of the class to remove the registry.
		@return				An error code.	
	TTErr unregisterClass(const TTSymbolRef className);
	*/
	
	
	/*
	TTErr refreshClass(const TTSymbolRef className)
	{
		// TODO: cache class Info (like the path of the extension) here
		unregisterClass(className);
		//	registerClass(className);
	
		// TODO: now re-load the DLL.
	}
	*/

	
	/**	unload/reload all external classes.
	NOTE that to do this, we need to keep a piece of information that tags a class as external or internal.

	This probably means that the hash is going to need to map not to a function pointer, 
	but rather to a struct or an object that keeps all of the class's information bundled together.
	TTErr refreshAllClasses();
	*/
	
	
	/**	Retreive the names of all registered #TTObjectBase classes in the environment.
		@param anInputValue			This is not being used.
		@param	anOutputValue		Pass a #TTValue that will be filled with an array of #TTSymbol pointers with the names of the classes.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getAllClassNames(const TTValue& anInputValue, TTValue &anOutputValue);
	
	
	/**	Retreive the names of all registered #TTObjectBase classes in the environment that
		are associated with the given tag(s). 
		@param	classNames	An array of TTSymbols upon return.
		@param	tags		An array of tags by which to search the environment's registry.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getClassNamesWithTags(TTValue& classNames, const TTValue& searchTags);

	/**	A message-bound version of getClassNamesWithTags()	*/
	TTErr getClassNamesForTags(const TTValue& anInputValue, TTValue &anOutputValue);
    
    /**	Check if a class is registered
     @param className			the name of a class
     @return					#TTErr error code if the class is not registered, else #kTTErrNone.
	 */
	TTErr isClassRegistered(const TTSymbol& className);
	
	
	/**	Create a new instance of a registered #TTObjectBase class.
		@param	className	Pass in a #TTSymbol with the name of the unit to load.  
		@param	anObject	Upon successful return, the value will be set to a #TTObjectBase which is the new instance.
							If the pointer is passed in as non-NULL then createUnit() will try to free to the
							existing object to which it points prior to instantiating the new unit.
		@param	anArgument	For most audio processing objects, this should be passed the maximum number of channels.
							For this reason, we overload this method with a TTUint16 argument as a convenience.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr createInstance(const TTSymbol className, TTObjectBasePtr* anObject, const TTValue& anArgument);
	
	
	/**	Create a reference to an object. */
	TTObjectBasePtr referenceInstance(TTObjectBasePtr anObject);

	
	/**	Release an instance of a #TTObjectBase class.
		At the moment this is simply freeing the class, but it may use reference counting in the future 
		(e.g. for TTBuffer references).

		There are a couple of reasons we want to have this wrapper around the delete operator.
		- For instrumenting the code to investigate bugs, performance, etc.
		- So that we can handle any threading, spin-locks, mutexes, or other issues before actually freeing the object.
		- At some point we may do a more release-like-thing where we reference count for pseudo-garbage-collection.

		@param	unit		A pointer to the unit to free.
		@return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr releaseInstance(TTObjectBasePtr* anObject);
};


/** The environment object has one instance, which is global in scope.
 */
extern TTFOUNDATION_EXPORT TTEnvironment* ttEnvironment;


// Public Interface -- REPLACED BY TTOBJECT

/** @brief DEPRECATED
 @deprecated Instead, please use the @n #TTObject constructor as detailed in @ref chapter_usingobjects_lifecycle */
TT_DEPRECATED( TTFOUNDATION_EXPORT TTErr TTObjectBaseInstantiate(const TTSymbol className, TTObjectBasePtr* returnedObjectPtr, const TTValue arguments) );
TT_DEPRECATED( TTFOUNDATION_EXPORT TTErr TTObjectBaseInstantiate(const TTSymbol className, TTObjectBasePtr* returnedObjectPtr, const TTUInt16 arguments) );

/** @brief DEPRECATED
 @deprecated Instead, most uses can be handled via the @n #TTObject and its = operator */
TT_DEPRECATED( TTFOUNDATION_EXPORT TTObjectBasePtr TTObjectBaseReference(TTObjectBasePtr anObject) );

/** @brief DEPRECATED
 @deprecated Instead, please use the @n #TTObject destructor via the delete operator as detailed in @ref chapter_usingobjects_lifecycle */
TT_DEPRECATED( TTFOUNDATION_EXPORT TTErr TTObjectBaseRelease(TTObjectBasePtr* anObject) );

TTFOUNDATION_EXPORT TTErr TTClassRegister(const TTSymbol className, const TTString& tagString, const TTObjectBaseInstantiationMethod anInstantiationMethod);
TTFOUNDATION_EXPORT TTErr TTClassRegister(const TTSymbol className, TTImmutableCString tagString, const TTObjectBaseInstantiationMethod anInstantiationMethod);

/** @brief DEPRECATED
 @deprecated Instead, please use @n TTObject::GetRegisteredClassNames */
TT_DEPRECATED( TTFOUNDATION_EXPORT TTErr TTGetRegisteredClassNames(TTValue& classNames) );

/** @brief DEPRECATED
 @deprecated Instead, please use @n TTObject::GetRegisteredClassNamesForTags */
TT_DEPRECATED( TTFOUNDATION_EXPORT TTErr TTGetRegisteredClassNamesForTags(TTValue& classNames, const TTValue& searchTags) );

#endif // __TT_ENVIRONMENT_H__

