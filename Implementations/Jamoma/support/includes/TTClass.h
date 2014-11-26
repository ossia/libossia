/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief The TTClass object represents a class in the Jamoma environment.
 *
 * @details This is to say that it represents everything about a class except for the class itself,
 * which is a subclass of TTObjectBase.
 *
 * @todo: We could override the () operator to make TTClass a functor for creating instances.
 *
 * @author Timothy Place, Trond Lossius
 *
 * @copyright Copyright © 2008, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_CLASS_H__
#define __TT_CLASS_H__

#include "TTObject.h"
#include "TTHash.h"
#include "TTEnvironment.h"


/** The TTClass object represents a class in the Jamoma environment.
 * This is to say that it represents everything about a class except for the class itself,
 * which is a subclass of TTObjectBase.
 */
class TTFOUNDATION_EXPORT TTClass {
private:
	friend class TTEnvironment;
	friend class TTObjectBase;
	
	TTSymbol						name;					///< The name of the class.
	TTValue							tags;					///< An array of TTSymbols containing tags associated with this class.
	TTObjectBaseInstantiationMethod		instantiationMethod;	///< Pointer to the function that will create a new instance.
	TTString						path;					///< The path to executable from which this class loaded.
	TTBoolean						external;				///< Is this class external to the main library?
	TTUInt16						count;					///< Reference count for how many instantiateds of this object exist.
		
public:
	
	/**	Constructor method.	
	 @param className				The name of the class.
	 @param tags					The tags that this class is associated with.
	 @param anInstantiationMethod	The instantiation method for this class.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTClass(const TTSymbol className, const TTValue& tags, const TTObjectBaseInstantiationMethod anInstantiationMethod);
		
	
	/**	Destructor. */
	virtual ~TTClass();
	
	
	/**	Create a new instance of a registered #TTObjectBase class.
		@param	anObject	Upon successful return, the value will be set to a #TTObjectBase which is the new instance.
							If the pointer is passed in as non-NULL then createUnit() will try to free to the
							existing object to which it points prior to instantiating the new unit.
		@param	anArgument	For most audio processing objects, this should be passed the maximum number of channels.
							For this reason, we overload this method with a TTUint16 argument as a convenience.
		@return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr createInstance(TTObjectBase** anObject, const TTValue& anArgument);
	
	
	/**	Free an instance of a #TTObjectBase class.
		There are a couple of reasons we want to have this wrapper around the delete operator.
		- For instrumenting the code to investigate bugs, performance, etc.@n
		- So that we can handle any threading, spin-locks, mutexes, or other issues before actually freeing the object.

		@param	unit		A pointer to the unit to free.
		@return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr releaseInstance(TTObjectBase* anObject);
	
	
};

/** Pointer to a #TTClass.
 @ingroup typedefs
 */
typedef TTClass* TTClassPtr;


#endif // __TT_CLASS_H__

