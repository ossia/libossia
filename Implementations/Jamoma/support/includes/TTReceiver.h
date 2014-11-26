/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Receiver Object
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_RECEIVER_H__
#define __TT_RECEIVER_H__

#include "TTModularIncludes.h"

/**	TTReceiver ... TODO : an explanation
 
 
 */

class TTMODULAR_EXPORT TTReceiver : public TTObjectBase
{
	TTCLASS_SETUP(TTReceiver)
	
public:	// use public for quick acces during signal processing
	
	TTObject                    mSignal;					///< any data structure to receive complex signal
	
private:
	
	TTAddress                   mAddress;					///< ATTRIBUTE : the address to bind
	TTBoolean					mActive;					///< ATTRIBUTE : if false, received data won't be output without unregister the attribute observer (default true).
	
	TTObject                    mReturnAddressCallback;		///< a way to return received address to the owner of this receiver
	TTObject                    mReturnValueCallback;		///< a way to return received value to the owner of this receiver
	TTObject                    mAddressObserver;			///< an address life cycle observer
	TTObject                    mApplicationObserver;		///< an application life cycle observer
	TTList                      mNodesObserversCache;		///< a list containing <aNode, anAttrObserver>
    // TODO JamomaCore #305 : TTList : allow to register as none pointer members
	TTListPtr                   mObjectCache;				///< ATTRIBUTE : a cache containing all binded objects for quick access
	
	/**	Setter for mAddress attribute. */
	TTErr setAddress(const TTValue& value);
	
	/**	Setter for mActive attribute. */
	TTErr setActive(const TTValue& value);
    
    /**	Getter for mObjectCache attribute. */
	TTErr getObjectCache(TTValue& value);
	
	/** Ask the value and use the callback to return it */
	TTErr Get();
    
    /** Ask the value and use the outputValue to return it.
        This method is not relevant with address containing wildcards */
	TTErr Grab(const TTValue& inputValue, TTValue& outputValue);
	
	/** */
	TTErr bindAddress(TTNodeDirectoryPtr aDirectory);
    void cacheNodeObserver(TTNodePtr aNode, TTAddress& anAddress, TTSymbol& anAttributeName);
	
	/**  */
	TTErr unbindAddress(TTNodeDirectoryPtr aDirectory);
    void uncacheNodeObserver(TTNodePtr aNode, TTObject& oldObserver, TTSymbol& anAttributeName);
	
	/** */
	TTErr bindApplication();
	
	/** */
	TTErr unbindApplication();
	
	friend TTErr TTMODULAR_EXPORT TTReceiverDirectoryCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTReceiverAttributeCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTReceiverApplicationManagerCallback(const TTValue& baton, const TTValue& data);
	
};

typedef TTReceiver* TTReceiverPtr;

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTReceiverDirectoryCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTReceiverAttributeCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTReceiverApplicationManagerCallback(const TTValue& baton, const TTValue& data);

#endif // __TT_RECEIVER_H__
