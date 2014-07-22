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
class TTSCORE_EXPORT TTTimeContainer : public TTTimeProcess {
    
    TTCLASS_SETUP(TTTimeContainer)
    
protected :
    
    TTList                      mTimeProcessList;               ///< all registered time processes and their observers
    TTList                      mTimeEventList;                 ///< all registered time events and their observers
    TTList                      mTimeConditionList;             ///< all registered time conditions and their observers
    
private :
    
    /** Get all time processes objects
     @param value           all time processes objects
     @return                kTTErrGeneric if mTimeProcessList is empty */
    TTErr           getTimeProcesses(TTValue& value);
    
    /** Get all time events objects
     @param value           all time events objects
     @return                kTTErrGeneric if mTimeEventList is empty */
    TTErr           getTimeEvents(TTValue& value);
    
    /** Get all time conditions objects
     @param value           all time conditions objects
     @return                kTTErrGeneric if mTimeConditionList is empty */
    TTErr           getTimeConditions(TTValue& value);
    
    /** Trigger next pending time events
     @param inputvalue      nothing or any event pending passing there position in the list of pending event (ex : 1 3 if there is 3 or more pending events and we want to trigger the first and the third events)
     @param outputvalue     the triggered time events
     @return                an error code if there is no next pending time event */
    TTErr           Next(const TTValue& inputValue, TTValue& outputValue);
    
    /** Create a time event
     @param inputvalue      a date
     @param outputvalue     a new time event
     @return                an error code if the creation fails */
    virtual TTErr   TimeEventCreate(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Release a time event
     @param inputValue      a time event object to release
     @param outputvalue     nothing
     @return                an error code if the destruction fails */
    virtual TTErr   TimeEventRelease(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Move a time event
     @param inputValue      a time event object, new date
     @param outputvalue     nothing
     @return                an error code if the movement fails */
    virtual TTErr   TimeEventMove(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Link a time event to a condition
     @param inputValue      a time event object, a time condition object
     @param outputvalue     nothing
     @return                an error code if the setting fails */
    virtual TTErr   TimeEventCondition(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Trigger a time event to make it happens
     @param inputValue      a time event object
     @param outputvalue     nothing
     @return                an error code if the triggering fails */
    virtual TTErr   TimeEventTrigger(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};

    /** Dispose a time event
     @param inputValue      a time event object
     @param outputvalue     nothing
     @return                an error code if the disposing fails */
    virtual TTErr   TimeEventDispose(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};

    /** Replace a time event by another one (copying date attribute)
     @param inputValue      a former time event object, a new time event object
     @param outputvalue     nothing
     @return                an error code if the replacement fails */
    virtual TTErr   TimeEventReplace(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Look up the time event list to retreive a time event by name
     @param inputValue      the name as symbol
     @param outputValue     the returned time event object
     @return                kTTErrValueNotFound if there is no event with this name */
    TTErr           TimeEventFind(const TTValue& inputValue, TTValue& outputValue);
    
    
    /** Create a time process
     @param inputvalue      a time process type, a start event, a end event
     @param outputvalue     a new time process
     @return                an error code if the creation fails */
    virtual TTErr   TimeProcessCreate(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Release a time process
     @param inputValue      a time process object to release
     @param outputvalue     its start and end event
     @return                an error code if the destruction fails */
    virtual TTErr   TimeProcessRelease(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Move a time process
     @param inputValue      a time process object, new start date, new end date
     @param outputvalue     nothing
     @return                an error code if the movement fails */
    virtual TTErr   TimeProcessMove(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Limit a time process duration
     @param inputValue      a time process object, new duration min, new duration max
     @param outputvalue     nothing
     @return                an error code if the limitation fails */
    virtual TTErr   TimeProcessLimit(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    
    /** Create a time condition
     @param inputvalue      nothing
     @param outputvalue     a new time condition
     @return                an error code if the creation fails */
    virtual TTErr   TimeConditionCreate(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Release a time process
     @param inputValue      a time condition object to release
     @param outputvalue     nothing
     @return                an error code if the destruction fails */
    virtual TTErr   TimeConditionRelease(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** To be notified when athe scheduler speed changed
     @param inputValue      the new speed value
     @param outputValue     nothing
     @return                kTTErrNone */
    TTErr           SchedulerSpeedChanged(const TTValue& inputValue, TTValue& outputValue);
    
protected :
    
    /* a time container can access too the protected members of any time event or time process */
    
    /** Getter on event's name protected member
     @param aTimeEvent      a time event object
     @return                a name symbol */
    TTSymbol&       getTimeEventName(TTObject& aTimeEvent);
    
    /** Getter on date time event protected member
     @param aTimeEvent      a time event object
     @return                a date value */
    TTUInt32        getTimeEventDate(TTObject& aTimeEvent);
    
    /** Getter on event's status protected member
     @param aTimeEvent      a time event object
     @return                a status symbol */
    TTSymbol&       getTimeEventStatus(TTObject& aTimeEvent);
    
    /** Getter on state time event protected member
     @param aTimeEvent      a time event object
     @return                a state object */
    TTObject&       getTimeEventState(TTObject& aTimeEvent);
    
    /** Getter on condition time event protected member
     @param aTimeEvent      a time event object
     @return                a condition object */
    TTObject&       getTimeEventCondition(TTObject& aTimeEvent);
    
    /** Write basic informations of a time event as Xml
     @param aXmlHandler     a xml handler
     @param aTimeProcess    a time event object
     @return                nothing */
    void            writeTimeEventAsXml(TTXmlHandlerPtr aXmlHandler, TTObject& aTimeEvent);
    
    /** Read basic informations of a time event from Xml
     @param aXmlHandler     a xml handler
     @param                 a new time event 
     @return                #TTErr*/
    TTErr           readTimeEventFromXml(TTXmlHandlerPtr aXmlHandler, TTObject& aNewTimeEvent);
    
    /** Getter on process's name protected member
     @param aTimeProcess    a time process object
     @return                a name symbol */
    TTSymbol        getTimeProcessName(TTObject& aTimeProcess);
    
    /** Getter on start event time process protected member
     @param aTimeProcess    a time process object
     @return                a time event instance */
    TTObject&       getTimeProcessStartEvent(TTObject& aTimeProcess);
    
    /** Setter on start event time process protected member
     @param aTimeProcess    a time process object
     @param aTimeEvent      a time event object */
    void            setTimeProcessStartEvent(TTObject& aTimeProcess, TTObject& aTimeEvent);
    
    /** Getter on end event time process protected member
     @param aTimeProcess    a time process object
     @return                a time event object */
    TTObject&       getTimeProcessEndEvent(TTObject& aTimeProcess);
    
    /** Setter on end event time process protected member
     @param aTimeProcess    a time process object
     @param aTimeEvent      a time event object */
    void            setTimeProcessEndEvent(TTObject& aTimeProcess, TTObject& aTimeEvent);
    
    /** Getter on duration min time process protected member
     @param aTimeProcess    a time process object
     @return                a duration value */
    TTUInt32        getTimeProcessDurationMin(TTObject& aTimeProcess);
    
    /** Getter on duration max time process protected member
     @param aTimeProcess    a time process object
     @return                a duration value */
    TTUInt32        getTimeProcessDurationMax(TTObject& aTimeProcess);
    
    /** Getter on running state of time process protected member
     @param aTimeProcess    a time process object
     @return                a boolean value */
    TTBoolean       getTimeProcessRunning(TTObject& aTimeProcess);
    
    /** Write basic informations of a time process as Xml
     @param aXmlHandler     a xml handler
     @param aTimeProcess    a time process object
     @return                nothing */
    void            writeTimeProcessAsXml(TTXmlHandlerPtr aXmlHandler, TTObject& aTimeProcess);
    
    /** Read basic informations of a time process from Xml
     @param aXmlHandler     a xml handler
     @param                 a new time process object 
     @return                #TTErr */
    TTErr           readTimeProcessFromXml(TTXmlHandlerPtr aXmlHandler, TTObject& aNewTimeProcess);
    
    
    /** Write basic informations of a time condition as Xml
     @param aXmlHandler     a xml handler
     @param aTimeProcess    a time condition object
     @return                nothing */
    void            writeTimeConditionAsXml(TTXmlHandlerPtr aXmlHandler, TTObject& aTimeCondition);
    
    /** Read basic informations of a time condition from Xml
     @param aXmlHandler     a xml handler
     @param                 a new time condition
     @return                #TTErr */
    TTErr           readTimeConditionFromXml(TTXmlHandlerPtr aXmlHandler, TTObject& aNewTimeCondition);
    
    
    friend TTErr TTSCORE_EXPORT TTTimeContainerSchedulerSpeedCallback(const TTValue& baton, const TTValue& data);
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
