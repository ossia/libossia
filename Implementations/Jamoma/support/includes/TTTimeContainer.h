/** @file
 *
 * @ingroup scoreLibrary
 *
 * @brief a container class to manage time events and time processes instances in the time
 *
 * @details The TTTimeContainer class allows to ... @n@n
 *
 * @see TTTimeEvent, TTTimeProcess
 *
 * @authors Théo de la Hogue & Clément Bossut
 *
 * @copyright Copyright © 2013, Théo de la Hogue & Clément Bossut @n
 * This code is licensed under the terms of the "CeCILL-C" @n
 * http://www.cecill.info
 */

#ifndef __TT_TIME_CONTAINER_H__
#define __TT_TIME_CONTAINER_H__

#include "TTScoreIncludes.h"
#include "TTTimeProcess.h"
#include "TTTimeCondition.h"

/**	The TTTimeContainer class allows to ...
 
 @see TTTimeProcess, TTTimeEvent
 */
class TTSCORE_EXPORT TTTimeContainer : public TTTimeProcess
{    
    TTCLASS_SETUP(TTTimeContainer)
      
private :
    
    /** To be notified when the clock speed changed
     @param inputValue      the new speed value
     @param outputValue     nothing
     @return                kTTErrNone */
    TTErr                   ClockSpeedChanged(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
protected :
    
    /* a time container can access quickly too some protected members of any time event or time process that are usefull at execution */
    
    /** Getter on date time event protected member
     @param aTimeEvent      a time event object
     @return                a date value */
    TTUInt32                getTimeEventDate(TTObject& aTimeEvent);
    
    /** Getter on event's status protected member
     @param aTimeEvent      a time event object
     @return                a status symbol */
    TTSymbol&               getTimeEventStatus(TTObject& aTimeEvent);
    
    /** Getter on state time event protected member
     @param aTimeEvent      a time event object
     @return                a state object */
    TTObject&               getTimeEventState(TTObject& aTimeEvent);
    
    /** Getter on condition time event protected member
     @param aTimeEvent      a time event object
     @return                a condition object */
    TTObject&               getTimeEventCondition(TTObject& aTimeEvent);
    
    /** Getter on start event time process protected member
     @param aTimeProcess    a time process object
     @return                a time event instance */
    TTObject&               getTimeProcessStartEvent(TTObject& aTimeProcess);
    
    /** Setter on start event time process protected member
     @param aTimeProcess    a time process object
     @param aTimeEvent      a time event object */
    void                    setTimeProcessStartEvent(TTObject& aTimeProcess, TTObject& aTimeEvent);
    
    /** Getter on end event time process protected member
     @param aTimeProcess    a time process object
     @return                a time event object */
    TTObject&               getTimeProcessEndEvent(TTObject& aTimeProcess);
    
    /** Setter on end event time process protected member
     @param aTimeProcess    a time process object
     @param aTimeEvent      a time event object */
    void                    setTimeProcessEndEvent(TTObject& aTimeProcess, TTObject& aTimeEvent);
    
    /** Getter on duration min time process protected member
     @param aTimeProcess    a time process object
     @return                a duration value */
    TTUInt32                getTimeProcessDurationMin(TTObject& aTimeProcess);
    
    /** Getter on duration max time process protected member
     @param aTimeProcess    a time process object
     @return                a duration value */
    TTUInt32                getTimeProcessDurationMax(TTObject& aTimeProcess);
    
    /** Getter on running state of time process protected member
     @param aTimeProcess    a time process object
     @return                a boolean value */
    TTBoolean               getTimeProcessRunning(TTObject& aTimeProcess);
    
    friend TTErr TTSCORE_EXPORT TTTimeContainerClockSpeedCallback(const TTValue& baton, const TTValue& data);
};

typedef TTTimeContainer* TTTimeContainerPtr;

void TTSCORE_EXPORT TTTimeContainerFindTimeProcess(const TTValue& aValue, TTPtr timeProcessPtrToMatch, TTBoolean& found);

void TTSCORE_EXPORT TTTimeContainerFindTimeEvent(const TTValue& aValue, TTPtr timeEventPtrToMatch, TTBoolean& found);

void TTSCORE_EXPORT TTTimeContainerFindTimeEventWithName(const TTValue& aValue, TTPtr timeEventNamePtrToMatch, TTBoolean& found);

void TTSCORE_EXPORT TTTimeContainerFindTimeProcessWithTimeEvent(const TTValue& aValue, TTPtr timeEventPtrToMatch, TTBoolean& found);

void TTSCORE_EXPORT TTTimeContainerFindTimeProcessWithName(const TTValue& aValue, TTPtr timeProcessNamePtrToMatch, TTBoolean& found);

void TTSCORE_EXPORT TTTimeContainerFindTimeCondition(const TTValue& aValue, TTPtr timeConditionPtrToMatch, TTBoolean& found);

void TTSCORE_EXPORT TTTimeContainerFindTimeConditionWithName(const TTValue& aValue, TTPtr timeConditionNamePtrToMatch, TTBoolean& found);

#endif // __TT_TIME_CONTAINER_H__
