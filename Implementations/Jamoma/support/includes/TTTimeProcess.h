/** @file
 *
 * @ingroup scoreLibrary
 *
 * @brief a class to define a process
 *
 * @details The TTTimeProcess class allows to ... @n@n
 *
 * @see TTTimeEvent
 *
 * @authors Théo de la Hogue & Clément Bossut
 *
 * @copyright Copyright © 2013, Théo de la Hogue & Clément Bossut @n
 * This code is licensed under the terms of the "CeCILL-C" @n
 * http://www.cecill.info
 */

#ifndef __TT_TIME_PROCESS_H__
#define __TT_TIME_PROCESS_H__

#include "TTScoreIncludes.h"
#include "TTTimeEvent.h"

/**	a class to define a process
 
 The TTTimeProcess class allows to ...
 
 @see TTTimeEvent
 */
class TTSCORE_EXPORT TTTimeProcess : public TTObjectBase {
    
    TTCLASS_SETUP(TTTimeProcess)
    
    friend class TTTimeContainer;

    TTObject                        mContainer;                     ///< the container which handles the time process
    
protected :
    
    TTSymbol                        mName;                          ///< the name of the time process
    
    TTUInt32                        mDurationMin;                   ///< the minimal duration of the time process
    TTUInt32                        mDurationMax;                   ///< the maximal duration of the time process
    
    TTBoolean                       mMute;                          ///< to not process anything
    
    TTValue                         mColor;                         ///< the color of the process (useful for gui)
    TTUInt32                        mVerticalPosition;              ///< the Y axe position of the process (useful for gui)
    TTUInt32                        mVerticalSize;                  ///< the Y axe size of the process (useful for gui)
    
    TTObject                        mScheduler;                     ///< the scheduler object which handles the time process execution
    
    TTBoolean                       mRunning;                       ///< a boolean to get the running state of the process
                                                                    ///< it is related to the running state of the scheduler
                                                                    ///< but it also allows to avoid last scheduler tick to call the process method (it could happen one tick after the stop)
    
    TTBoolean                       mCompiled;                      ///< a boolean flag to know if the compile method needs to be called or not (uselly after an event date change)
    
    TTBoolean                       mExternalTick;                  ///< a boolean flag to ease the access to the scheduler externalTick attribute
    
private :
    
    TTObject                        mStartEvent;                    ///< the event object which handles the time process execution start
    
    TTList                          mIntermediateEvents;            ///< the list of all intermediate events
    
    TTObject                        mEndEvent;                      ///< the event object which handles the time process execution stop
    
    /** Specific compilation method used to pre-processed data in order to accelarate Process method.
     @details the compiled attribute allows to know if the process needs to be compiled or not.
     @return                an error code returned by the compile method */
    virtual TTErr   Compile() {return kTTErrGeneric;};
    
    /** Specific process method on start
     @return                an error code returned by the process start method */
    virtual TTErr   ProcessStart() {return kTTErrGeneric;};
    
    /** Specific process method on end
     @return                an error code returned by the process end method */
    virtual TTErr   ProcessEnd() {return kTTErrGeneric;};
    
    /** Specific process method
     @param	inputValue      position and date of the scheduler
     @param	outputValue     return an error of the processing
     @return                an error code returned by the process method */
    virtual TTErr   Process(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Specific process method for pause/resume
     @param	inputValue      boolean paused state of the scheduler
     @param	outputValue     return an error of the processing
     @return                an error code returned by the process paused method */
    virtual TTErr   ProcessPaused(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** Specific go to method to set the process at a date
     @param	inputValue      a date where to go relative to the duration of the time process, an optional boolean to temporary mute the process 
     @param	outputValue     nothing
     @return                an error code if the operation fails */
    virtual TTErr   Goto(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /**  needed to be handled by a TTXmlHandler
     @param	inputValue      ..
     @param	outputValue     ..
     @return                .. */
	virtual TTErr	WriteAsXml(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
	virtual TTErr	ReadFromXml(const TTValue& inputValue, TTValue& outputValue) {outputValue = inputValue; return kTTErrGeneric;};
    
    /** get the time process rigidity
     @param	value           rigidity state
     @return                kTTErrNone */
    TTErr           getRigid(TTValue& value);
    
    /** set the time process rigidity
     @param	value           a new rigidity state
     @return                kTTErrNone */
    TTErr           setRigid(const TTValue& value);
    
    /** set the time process minimal duration
     @param	value           a new minimal duration
     @return                kTTErrNone */
    TTErr           setDurationMin(const TTValue& value);
    
    /** set the time process maximal duration
     @param	value           a new maximal duration
     @return                kTTErrNone */
    TTErr           setDurationMax(const TTValue& value);
    
    /** get the time process start date
     @details this method eases the getting of the start event date
     @param	value           the returned start date
     @return                kTTErrNone */
    TTErr           getStartDate(TTValue& value);
    
    /** set the time process start date
     @details this method eases the setting of the start event date
     @param	value           a new start date
     @return                kTTErrNone */
    TTErr           setStartDate(const TTValue& value);
    
    /** get start event condition object
     @details this method eases the getting of the start event condition object
     @param	value           the returned condition object
     @return                an error code if the event doesn't exist */
    TTErr           getStartCondition(TTValue& value);
    
    /** set start event condition object
     @details this method eases the setting of the start event condition object
     @param	value           a condition object
     @return                an error code if the event doesn't exist */
    TTErr           setStartCondition(const TTValue& value);
    
    /** get the time process end date
     @details this method eases the getting of the end event date
     @param	value           the returned end date
     @return                kTTErrNone */
    TTErr           getEndDate(TTValue& value);
    
    /** set the time process end date
     @details this method eases the setting of the end event date
     @param	value           a new end date
     @return                kTTErrNone */
    TTErr           setEndDate(const TTValue& value);
    
    /** get end event condition object
     @details this method eases the getting of the end event condition object
     @param	value           the returned condition object
     @return                an error code if the event doesn't exist */
    TTErr           getEndCondition(TTValue& value);
    
    /** set end event condition object
     @details this method eases the setting of the end event condition object
     @param	value           a condition object
     @return                an error code if the event doesn't exist */
    TTErr           setEndCondition(const TTValue& value);
    
    /** get the time process duration
     @param	value           the returned duration
     @return                kTTErrNone */
    TTErr           getDuration(TTValue& value);
    
    /** Set the time process color
     @param	value           8 bit color format <red green blue>
     @return                kTTErrNone */
    TTErr           setColor(const TTValue& value);
    
    /** get the speed of the time process
     @details this method eases the getting of speed of the scheduler object
     @param	value           the speed as #TTFloat64 value
     @return                an error code if the speed cannot be get */
    TTErr           getSpeed(TTValue& value);
    
    /** get the speed of the time process
     @details this method eases the setting of speed of the scheduler object
     @param	value           the speed as #TTFloat64 value
     @return                an error code if the speed cannot be set */
    TTErr           setSpeed(const TTValue& value);
    
    /** get the position of the time process
     @details this method eases the getting of position of the scheduler object
     @param	value           the position as #TTFloat64 value
     @return                an error code if the position cannot be get */
    TTErr           getPosition(TTValue& value);
    
    /** get the date of the time process
     @details this method eases the getting of date of the scheduler object
     @param	value           the date as #TTFloat64 value
     @return                an error code if the date cannot be get */
    TTErr           getDate(TTValue& value);
    
    /** Get intermediate events of the time process
     @param	value           returned events
     @return                kTTErrNone */
    TTErr           getIntermediateEvents(TTValue& value);
    
    /** Move the time process
     this method eases the setting of the start and end event dates
     @param	inputValue      new start date, new end date
     @param	outputValue     nothing
     @return                an error code if the movement fails */
    TTErr           Move(const TTValue& inputValue, TTValue& outputValue);
    
    /** Limit the time process duration
        this method eases the setting of the minimal and maximal durations
     @param	inputValue      duration min, duration max
     @param	outputValue     nothing
     @return                an error code if the limitation fails */
    TTErr           Limit(const TTValue& inputValue, TTValue& outputValue);
    
    /** Start the time process
     this method eases the access of the start event trigger message
     @return                an error code if the play fails */
    TTErr           Start();
    
    /** End the time process
     this method eases the access of the end event trigger message
     @return                an error code if the stop fails */
    TTErr           End();
    
    /** Play the time process from a time offset
        this method eases the managment of the scheduler object
     @return                an error code if the play fails */
    TTErr           Play();
    
    /** Stop the time process
     this method eases the managment of the scheduler object
     @return                an error code if the stop fails */
    TTErr           Stop();
    
    /** Pause the time process
        this method eases the managment of the scheduler object
     @return                an error code if the pause fails */
    TTErr           Pause();
    
    /** Resume the time process
        this method eases the managment of the scheduler object
     @return                an error code if the resume fails */
    TTErr           Resume();
    
    /** Drive the time process progression
     this method eases the managment of the scheduler object
     @return                an error code if the tick fails */
    TTErr           Tick();
    
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
    
    /** To be notified when the scheduler running status change
     @param inputValue      the new running status
     @param outputValue     nothing
     @return                kTTErrNone */
    TTErr           SchedulerRunningChanged(const TTValue& inputValue, TTValue& outputValue);
    
protected :
    
    /** get the start event
     @return                a time event object */
    TTObject&       getStartEvent();
    
    /** set the end event
     @return                a time event object */
    TTObject&       getEndEvent();
    
    /** set the start event
     @param aTimeEvent      a time event object
     @return                an error code if it fails */
    TTErr           setStartEvent(TTObject& aTimeEvent);
    
    /** set the end event
     @param aTimeEvent      a time event object
     @return                an error code if it fails */
    TTErr           setEndEvent(TTObject& aTimeProcess);
    
    friend void TTSCORE_EXPORT TTTimeProcessSchedulerCallback(TTPtr object, TTFloat64 position, TTFloat64 date);
    
    friend void TTSCORE_EXPORT TTTimeContainerFindTimeProcessWithTimeEvent(const TTValue& aValue, TTPtr timeEventPtrToMatch, TTBoolean& found);
};

typedef TTTimeProcess* TTTimeProcessPtr;

/** The scheduler time position callback
 @param	object				a time process instance
 @param	position			the time position
 @return					an error code */
void TTSCORE_EXPORT TTTimeProcessSchedulerCallback(TTPtr object, TTFloat64 position, TTFloat64 date);

/** Define some macros to ease the access of events attributes */
#define mStartDate TTTimeEventPtr(mStartEvent.instance())->mDate
#define mStartCondition TTTimeEventPtr(mStartEvent.instance())->mCondition

#define mEndDate TTTimeEventPtr(mEndEvent.instance())->mDate
#define mEndCondition TTTimeEventPtr(mEndEvent.instance())->mCondition

#define mDuration mEndDate - mStartDate

/** Define callback function to get position back from the scheduler */
typedef void (*TTTimeProcessPositionCallback)(TTPtr, TTFloat64, TTFloat64);

/** Define an unordered map to store and retreive a value relative to a TTTimeProcessPtr */
#ifdef TT_PLATFORM_WIN
    #include <hash_map>
    using namespace stdext;	// Visual Studio 2008 puts the hash_map in this namespace
    typedef hash_map<TTTimeProcessPtr,TTValuePtr>    TTTimeProcessMap;
#else
    //	#ifdef TT_PLATFORM_LINUX
    // at least for GCC 4.6 on the BeagleBoard, the unordered map is standard
    #include <unordered_map>
    //	#else
    //		#include "boost/unordered_map.hpp"
    //		using namespace boost;
    //	#endif
    typedef std::unordered_map<TTTimeProcessPtr,TTValuePtr>	TTTimeProcessMap;
#endif

typedef	TTTimeProcessMap*                   TTTimeProcessMapPtr;
typedef TTTimeProcessMap::const_iterator	TTTimeProcessMapIterator;


#endif // __TT_TIME_PROCESS_H__