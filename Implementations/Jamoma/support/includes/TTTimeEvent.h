/** @file
 *
 * @ingroup scoreLibrary
 *
 * @brief a class to define an event
 *
 * @details The TTTimeEvent class allows to ... @n@n
 *
 * @see TTTimeProcess
 *
 * @authors Théo de la Hogue & Clément Bossut
 *
 * @copyright Copyright © 2013, Théo de la Hogue & Clément Bossut @n
 * This code is licensed under the terms of the "CeCILL-C" @n
 * http://www.cecill.info
 */

#ifndef __TT_TIME_EVENT_H__
#define __TT_TIME_EVENT_H__

#include "TTScoreIncludes.h"

/**	a class to define an event
 
 The TTTimeEvent class allows to ...
 
 @see TTTimeProcess
 */
class TTSCORE_EXPORT TTTimeEvent : public TTObjectBase {
    
    TTCLASS_SETUP(TTTimeEvent)
    
    friend class TTTimeContainer;
    friend class TTTimeProcess;
    
    TTObject                        mContainer;                     ///< the container which handles the event
    
protected :
    
    TTSymbol                        mName;                          ///< the name of the event

    TTUInt32                        mDate;                          ///< the date of the event
    
    TTSymbol                        mStatus;                        ///< the status of the event : kTTSym_eventWaiting, kTTSym_eventPending, kTTSym_eventHappened, kTTSym_eventdisposed.
    
    TTBoolean                       mMute;                          ///< to not push the state
    
    TTObject                        mState;                         ///< a state handled by the event
    
    TTObject                        mCondition;                     ///< a pointer to an optional condition object to make the event interactive
 
private :
    
    /** Set the date of the event
     @param	value           a date
     @return                an error code if the date is wrong */
    TTErr           setDate(const TTValue& value);
    
    /** Link the event to a condition
     @param	value           a condition object
     @return                kTTErrNone */
    TTErr           setCondition(const TTValue& value);
    
    /** Set the event status
     @param	value           a TTTimeEventStatusFlag
     @return                kTTErrNone */
    TTErr           setStatus(const TTValue& value);
    
    /** Try to make the event happen (possibility to use the scenario to check event validity)
     @return                an error code returned by the trigger method */
    TTErr           Trigger();
    
    /** Make the event happen
     @return                an error code returned by the happen method */
    TTErr           Happen();
    
    /** Make the event not happen
     @return                an error code returned by the dispose method */
    TTErr           Dispose();
    
    /**  needed to be handled by a TTXmlHandler
     @param	inputValue      ..
     @param	outputValue     ..
     @return                .. */
	TTErr           WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
	TTErr           ReadFromXml(const TTValue& inputValue, TTValue& outputValue);

    /** Get a line value of the state for an address
     @details this method eases the access of one state value
     @param	inputValue      an address
     @param	outputValue     the value of the state for an address
     @return                kTTErrNone */
    TTErr           StateAddressGetValue(const TTValue& inputValue, TTValue& outputValue);
    
    /** Set a line value of the state for an address
     @details this method eases the access of one state value
     @param	inputValue      an address, value pointer
     @param	outputValue     the value of the state for an address
     @return                kTTErrNone */
    TTErr           StateAddressSetValue(const TTValue& inputValue, TTValue& outputValue);
    
    /** Remove a line to the state for an address
     @details this method eases the access of one state value
     @param	inputValue      an address
     @param	outputValue     nothing
     @return                kTTErrNone */
    TTErr           StateAddressClear(const TTValue& inputValue, TTValue& outputValue);
    
    friend void TTSCORE_EXPORT TTTimeContainerFindTimeEventWithName(const TTValue& aValue, TTPtr timeEventNamePtrToMatch, TTBoolean& found);
    friend TTBoolean TTSCORE_EXPORT TTTimeEventCompareDate(TTValue& v1, TTValue& v2);
};

typedef TTTimeEvent* TTTimeEventPtr;

TTBoolean TTSCORE_EXPORT TTTimeEventCompareDate(TTValue& v1, TTValue& v2);

/** Define an unordered map to store and retreive a value relative to a TTTimeEventPtr */
#ifdef TT_PLATFORM_WIN
    #include <hash_map>
    using namespace stdext;	// Visual Studio 2008 puts the hash_map in this namespace
    typedef hash_map<TTTimeEventPtr,TTValuePtr>    TTTimeEventMap;
#else
//	#ifdef TT_PLATFORM_LINUX
//  at least for GCC 4.6 on the BeagleBoard, the unordered map is standard
    #include <unordered_map>
//	#else
//		#include "boost/unordered_map.hpp"
//		using namespace boost;
//	#endif
    typedef std::unordered_map<TTTimeEventPtr,TTValuePtr>	TTTimeEventMap;
#endif

typedef	TTTimeEventMap*                   TTTimeEventMapPtr;
typedef TTTimeEventMap::const_iterator	TTTimeEventMapIterator;

#endif // __TIME_EVENT_H__
