/** @file
 *
 * @ingroup scoreLibrary
 *
 * @brief a class to define a condition and a set of different cases
 *
 * @details The TTTimeCondition class allows to ... @n@n
 *
 * @see TTTimeEvent
 *
 * @authors Théo de la Hogue & Clément Bossut
 *
 * @copyright Copyright © 2013, Théo de la Hogue & Clément Bossut @n
 * This code is licensed under the terms of the "CeCILL-C" @n
 * http://www.cecill.info
 */

#ifndef __TT_TIME_CONDITION_H__
#define __TT_TIME_CONDITION_H__

#include "TTScoreIncludes.h"
#include "Expression.h"

/** Define a struct containing an expression and a boolean, as the expression to trigger and the default comportment */
struct Comportment {
    
    Comportment() : trigger(), dflt(true) {}

    Expression trigger;
    TTBoolean dflt;
};

/** Define an unordered map to store and retreive a comportment relative to a TTTimeEventPtr */
#ifdef TT_PLATFORM_WIN
    #include <hash_map>
    using namespace stdext;	// Visual Studio 2008 puts the hash_map in this namespace
    typedef hash_map<TTObjectBasePtr,Comportment>    TTCaseMap;
#else
//	#ifdef TT_PLATFORM_LINUX
//  at least for GCC 4.6 on the BeagleBoard, the unordered map is standard
    #include <unordered_map>
//	#else
//		#include "boost/unordered_map.hpp"
//		using namespace boost;
//	#endif
    typedef std::unordered_map<TTObjectBasePtr,Comportment>	TTCaseMap;
#endif

typedef	TTCaseMap*                  TTCaseMapPtr;
typedef TTCaseMap::const_iterator   TTCaseMapIterator;


/**	a class to define a condition and a set of different cases
 
 The TTTimeCondition class allows to ...
 
 @see TTTimeEvent
 */
class TTSCORE_EXPORT TTTimeCondition : public TTObjectBase {
    
    TTCLASS_SETUP(TTTimeCondition)
    
    friend class TTTimeEvent;

    TTObject                        mContainer;                     ///< the container which handles the condition
    
protected :
    
    TTSymbol                        mName;                          ///< the name of the condition
    
    TTBoolean                       mActive;                        ///< enable/disable the condition (if it is ready only)
    
    TTBoolean                       mReady;                         ///< is the condition ready to be activated ?
    
    TTHash                          mReceivers;                     ///< a table of receivers stored by address
    TTCaseMap                       mCases;                         ///< a map linking an event to its comportment

    Expression                      mDispose;                       ///< the expression to dispose the condition

    TTUInt8                         mPendingCounter;                ///< counting the number of unready events

private :
    
    /** get all expressions symbol associated to each event
     @param	value           a value containing one expression per event
     @return                kTTErrNone */
    TTErr           getExpressions(TTValue& value);

    /** get all the events associated to the condition
     @param value           the time events objects
     @return                kTTErrNone */
    TTErr           getEvents(TTValue& value);
    
    /** Set the active state
     @param	value           a new active state
     @return                kTTErrNone */
    TTErr           setActive(const TTValue& value);
    
    /**  Get the dispose expression
     @param value           an expression symbol
     @return                kTTErrNone */
    TTErr           getDisposeExpression(TTValue& value);
    
    /** Set the dispose expression
     @param	value           the expression
     @return                kTTErrNone */
    TTErr           setDisposeExpression(const TTValue& value);
    
    /**  Add an event to the condition
     @param	inputValue      an event and optionnally the comportment associated
     @param	outputValue     nothing
     @return                an error code if the operation fails */
    TTErr           EventAdd(const TTValue& inputValue, TTValue& outputValue);
    
    /**  Remove an event from the condition
     @param	inputValue      an event
     @param	outputValue     nothing
     @return                an error code if the operation fails */
    TTErr           EventRemove(const TTValue& inputValue, TTValue& outputValue);
    
    /**  Edit the expression associated to an event
     @param	inputValue      an event and the expression
     @param	outputValue     nothing
     @return                an error code if the operation fails */
    TTErr           EventExpression(const TTValue& inputValue, TTValue& outputValue);

    /**  Edit the default comportment associated to an event
     @param	inputValue      an event and a boolean
     @param	outputValue     nothing
     @return                an error code if the operation fails */
    TTErr           EventDefault(const TTValue& inputValue, TTValue& outputValue);

    /**  Find the expression associated to an event
     @param	inputValue      an event
     @param	outputValue     an expression symbol
     @return                an error code if the operation fails */
    TTErr           ExpressionFind(const TTValue& inputValue, TTValue& outputValue);

    /**  Find the default comportment associated to an event
     @param	inputValue      an event
     @param	outputValue     an expression symbol
     @return                an error code if the operation fails */
    TTErr           DefaultFind(const TTValue& inputValue, TTValue& outputValue);

    /** Test an expression
     @param inputvalue      an expression value or symbol
     @param outputvalue     the result as a boolean
     @return                an error code if the operation fails */
    TTErr           ExpressionTest(const TTValue& inputValue, TTValue& outputValue);
    
    /**  Tell the events to trigger. All the other events will be disposed
     @param	inputValue      events to trigger (if no events are passed all the events will be triggered)
     @param	outputValue     nothing
     @return                an error code if the operation fails */
    TTErr           Trigger(const TTValue& inputValue, TTValue& outputValue);
    
    /**  Tell the events to dispose. All the other events will be triggered
     @param	inputValue      events to dispose (if no events are passed all the events will be disposed)
     @param	outputValue     nothing
     @return                an error code if the operation fails */
    TTErr           Dispose(const TTValue& inputValue, TTValue& outputValue);
    
    /**  needed to be handled by a TTXmlHandler
     @param	inputValue      ..
     @param	outputValue     ..
     @return                .. */
	TTErr           WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
	TTErr           ReadFromXml(const TTValue& inputValue, TTValue& outputValue);
    
    /** To be notified when an event date changed
     @param inputValue      the event which have changed his date
     @param outputValue     nothing
     @return                kTTErrNone */
    TTErr           EventDateChanged(const TTValue& inputValue, TTValue& outputValue);
    
    /** To be notified when an event status changed
     @param inputValue      the event which have changed his status
     @param outputValue     nothing
     @return                kTTErrNone */
    TTErr           EventStatusChanged(const TTValue& inputValue, TTValue& outputValue);
    
    /** Helper function to set the ready attribute and notify
     @param	newReady        a boolean
     @return                kTTErrNone */
    TTErr           setReady(TTBoolean newReady);
    
    /** Helper function to manage receivers : add a receiver for to the address if no receiver already exists
     @param	anAddress      an address to observe */
    void            addReceiver(TTAddress anAddress);

    /** Helper function to apply the default comportment of each event */
    void            applyDefaults();

    friend TTErr TTSCORE_EXPORT TTTimeConditionReceiverReturnValueCallback(const TTValue& baton, const TTValue& data);
    
};

typedef TTTimeCondition* TTTimeConditionPtr;

/** The case receiver callback return back the value of observed address
 @param	baton               a time condition instance, an address
 @param	data                a value to test
 @return					an error code */
TTErr TTSCORE_EXPORT TTTimeConditionReceiverReturnValueCallback(const TTValue& baton, const TTValue& data);

#endif // __TT_TIME_CONDITION_H__
