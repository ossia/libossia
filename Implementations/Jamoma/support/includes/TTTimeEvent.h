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
class TTSCORE_EXPORT TTTimeEvent : public TTObjectBase
{    
    TTCLASS_SETUP(TTTimeEvent)
    
    friend class TTTimeContainer;
    friend class TTTimeProcess;
    
    TTObject                        mContainer;                     ///< the container which handles the event
    
protected :
    
    TTSymbol                        mName;                          ///< the name of the event

    TTUInt32                        mDate;                          ///< the date of the event
    
    TTSymbol                        mStatus;                        ///< the status of the event (kTTSym_eventWaiting, kTTSym_eventPending, kTTSym_eventHappened, kTTSym_eventDisposed)
    
    TTBoolean                       mMute;                          ///< to not push the state
    
    TTObject                        mState;                         ///< a state handled by the event
    
    TTObject                        mCondition;                     ///< a pointer to an optional condition object to make the event interactive
    
private :
    
    TTValue                         mAttachedProcesses;             ///< all the processes the event observes
    TTUInt32                        mMinReachedProcessesCounter;    ///< how many processes have reached their minimun duration bound ?
    TTUInt32                        mEndedProcessesCounter;         ///< how many processes have ended ?
    TTUInt32                        mDisposedProcessesCounter;      ///< how many processes have been disposed ?
    
    TTBoolean                       mRequestWait;                   ///< an internal flag to know if there is a request to make the event to wait
    TTBoolean                       mRequestHappen;                 ///< an internal flag to know if there is a request to make the event happen
    TTBoolean                       mRequestDispose;                ///< an internal flag to know if there is a request to dispose the event
    
    TTBoolean                       mPushing;                       ///< an internal flag to know if the event is pushing its state
    
    /** Set the date of the event
     @param	value           a date
     @return                #kTTErrGeneric if the date is wrong */
    TTErr           setDate(const TTValue& value);
    
    /** Link the event to a condition
     @param	value           a condition object
     @return                kTTErrNone */
    TTErr           setCondition(const TTValue& value);
    
    /** Set status directly when the container is not running
     @details this is usefull to reset event at precise status but it couldn't not be used when container is running
     @param	value           a #TTSymbol (kTTSym_eventWaiting, kTTSym_eventPending, kTTSym_eventHappened, kTTSym_eventDisposed)
     @return                #kTTErrGeneric if the container is running */
    TTErr           setStatus(const TTValue& value);
    
    /** Set the state relative to the event
     @param	value           a state
     @return                #kTTErrNone*/
    TTErr           setState(const TTValue& value);
    
    /** Get the state relative to the event
     @param	value           a state
     @return                #kTTErrNone*/
    TTErr           getState(TTValue& value);
    
    /** Request to make the event to wait
     @details the request will be apply on next StatusUpdate call
     @return                #kTTErrGeneric if the request cannot be handled */
    TTErr           Wait();
    
    /** Request to make the event happen
     @details the request will be apply on next StatusUpdate call
     @return                #kTTErrGeneric if the request cannot be handled */
    TTErr           Happen();
    
    /** Request to make the event not happen
     @details the request will be apply on next StatusUpdate call
     @return                #kTTErrGeneric if the request cannot be handled */
    TTErr           Dispose();
    
    /** Apply request or update event status depending on attached processes statement
     @details this methods should only be called by our container or the event itself
     @return                #kTTErrGeneric if nothing change for the event */
    TTErr           StatusUpdate();
    
    /** Internal method to apply new status and notify observers
     @param	value           a #TTSymbol (kTTSym_eventWaiting, kTTSym_eventPending, kTTSym_eventHappened, kTTSym_eventDisposed)
     @return                #kTTErrGeneric if repetitions are detected */
    TTErr           applyStatus(const TTValue& value);
    
    /**  needed to be handled by a TTXmlHandler
     @param	inputValue      ..
     @param	outputValue     ..
     @return                .. */
	TTErr           WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
	TTErr           ReadFromXml(const TTValue& inputValue, TTValue& outputValue);
    
    
    /** Push the state content
     @details this method eases the call of state run method
     @return                #kTTErrNone */
    TTErr           StatePush();
    
    /** Clear the state content
     @details this method eases the call of state clear method
     @return                #kTTErrNone */
    TTErr           StateClear();
    
    /** Get a line value of the state for an address
     @details this method eases the access of one state value
     @param	inputValue      #TTAddress
     @param	outputValue     #TTValue of the state for an address
     @return                kTTErrNone */
    TTErr           StateAddressGetValue(const TTValue& inputValue, TTValue& outputValue);
    
    /** Set a line value of the state for an address
     @details this method eases the access of one state value
     @param	inputValue      #TTAddress, #TTValuePtr
     @param	outputValue     the value of the state for an address
     @return                kTTErrNone */
    TTErr           StateAddressSetValue(const TTValue& inputValue, TTValue& outputValue);
    
    /** Remove a line to the state for an address
     @details this method eases the access of one state value
     @param	inputValue      #TTAddress
     @param	outputValue     nothing
     @return                #kTTErrNone */
    TTErr           StateAddressClear(const TTValue& inputValue, TTValue& outputValue);
    
    /** Get all addresses of the state
     @details this method eases the access of all addresses of the state
     @param	inputValue      nothing
     @param	outputValue     all #TTAddress
     @return                #kTTErrNone */
    TTErr           StateAddresses(const TTValue& inputValue, TTValue& outputValue);
    
    /** Attach a process to enable notification observation
     @param inputValue      the process to observe
     @param outputValue     nothing
     @return                #kTTErrNone */
    TTErr           ProcessAttach(const TTValue& inputValue, TTValue& outputValue);
    
    /** Detach a process to disable notification observation
     @param inputValue      the process to observe
     @param outputValue     nothing
     @return                #kTTErrNone */
    TTErr           ProcessDetach(const TTValue& inputValue, TTValue& outputValue);
    
    /** To be notified when a previous process reaches its minimal duration
     @param inputValue      the process which is started
     @param outputValue     nothing
     @return                #kTTErrNone */
    TTErr           ProcessDurationMinReached(const TTValue& inputValue, TTValue& outputValue);
    
    /** To be notified when a previous ended process
     @param inputValue      the process which is ended
     @param outputValue     nothing
     @return                #kTTErrNone */
    TTErr           ProcessEnded(const TTValue& inputValue, TTValue& outputValue);
    
    /** To be notified when a previous disposed process
     @param inputValue      the process which have been disposed
     @param outputValue     nothing
     @return                #kTTErrNone */
    TTErr           ProcessDisposed(const TTValue& inputValue, TTValue& outputValue);
    
    friend void TTSCORE_EXPORT TTTimeContainerFindTimeEventWithName(const TTValue& aValue, TTPtr timeEventNamePtrToMatch, TTBoolean& found);
    friend TTBoolean TTSCORE_EXPORT TTTimeEventCompareDate(TTValue& v1, TTValue& v2);
};

typedef TTTimeEvent* TTTimeEventPtr;

/** Comparison function
 @param	v1			a first time event
 @param	v2			a second time event
 @return			return true if first time event is before second time event */
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
