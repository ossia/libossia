/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief This class is used to create a backward communication channel to notify a client that something changed in the #TTnode.
 *
 * @details
 *
 * @todo Use the TTObject class functionnality because TTNode is also a TTObject
 *
 * @author Théo de la Hogue, Tim Place, Nathan Wolek, Trond Lossius
 *
 * @copyright Copyright © 2009, Théo de la Hogue and Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_CALLBACK_H__
#define __TT_CALLBACK_H__

#include "TTFoundationAPI.h"


/** 
 This class is used to be sensitive to any TTObject notifications and report them using a function with baton and value
 We will subclass TTObject in order to gain some functionality -- like observers and notifications.
 */
class TTFOUNDATION_EXPORT TTCallback : public TTObjectBase
{
	TTCLASS_SETUP(TTCallback)

protected:

	TTFunctionWithBatonAndValue	mFunction;          ///< Function called when the notify message is engaged.
	TTValue						mBaton;             ///< User data to be passed back when the callback is triggered.
    TTSymbol                    mNotification;      ///< the notification the callback is sensitive to

private:
    
    /** Set baton value
     @param	value           any value
     @return                kTTErrNone */
    TTErr setBaton(const TTValue& value);
    
    /** Set to which notification the callback is sensitive to
     @param	value           a symbol
     @return                kTTErrNone */
    TTErr setNotification(const TTValue& value);
	
public:

	/**	Message called because we are registered as an observer to some other object, and then calls our external function.
	 @param anInputValue			TODO: Add info
	 @param anUnusedOutputValue		TODO: Add info
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr notify(const TTValue& anInputValue, TTValue &anUnusedOutputValue);
	
	TTErr deliver(const TTValue anInputValue)
	{
		TTValue unused;
		return notify(anInputValue, unused);
	}
	
	
	/**	Usefull to easily free the baton in case the pointer is a TTValue of something which need to be freed corectly.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTPtr getBaton(){return mBaton;};
};

/** Pointer to a #TTCallback object.
 @ingroup typedefs
 */
typedef TTCallback* TTCallbackPtr;


#endif // __TT_CALLBACK_H__
