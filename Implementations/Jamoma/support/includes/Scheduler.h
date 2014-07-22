/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Scheduler interface
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2013, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "TTFoundationAPI.h"

#define SCHEDULER_CONSTRUCTOR \
TTObjectBasePtr thisTTClass :: instantiate (TTSymbol name, TTValue arguments) {return new thisTTClass (arguments);} \
\
extern "C" void thisTTClass :: registerClass () {TTClassRegister( TTSymbol(thisTTClassName), thisTTClassTags, thisTTClass :: instantiate );} \
\
thisTTClass :: thisTTClass (const TTValue& arguments) : Scheduler(arguments)

#define SCHEDULER_INITIALIZE \
mName = TTSymbol(thisTTClassName); \
mVersion = TTSymbol(thisSchedulerVersion); \
mAuthor = TTSymbol(thisSchedulerAuthor); \
mStretchable = TTSymbol(thisSchedulerStretchable); \
registerAttribute(TTSymbol("parameterNames"), kTypeLocalValue, NULL, (TTGetterMethod)& thisTTClass::getParameterNames); \
/*addAttributeProperty(ParameterNames, readOnly, YES); \ */

typedef void (*SchedulerPositionCallback)(TTPtr, TTFloat64, TTFloat64);

/****************************************************************************************************/
// Class Specification

/**	Scheduler is the base class for all Scheduler Unit.
 It still has knowledge and support for ...
 */
class Scheduler : public TTObjectBase {

public:
    
	TTSymbol                        mName;					///< ATTRIBUTE : the name of the scheduler							
	TTSymbol                        mVersion;				///< ATTRIBUTE : the version of the scheduler								
	TTSymbol                        mAuthor;				///< ATTRIBUTE : the author of the scheduler								
	TTBoolean                       mStretchable;			///< ATTRIBUTE : is the scheduler provide stretch feature ?

protected:
    
    TTFloat64                       mDuration;              ///< ATTRIBUTE : the time (in ms) the scheduler will run at normal speed factor
    TTFloat64                       mOffset;                ///< ATTRIBUTE : the date (in ms) the sheduler will run from
    TTFloat64                       mSpeed;                 ///< ATTRIBUTE : the speed factor of the scheduler
    TTBoolean                       mExternalTick;          ///< ATTRIBUTE : if true the Tick message comes from an external source
    
    TTBoolean                       mRunning;               ///< ATTRIBUTE : is the scheduler running right now ?
    TTBoolean                       mPaused;                ///< ATTRIBUTE : is the scheduler paused right now ?
    TTFloat64                       mPosition;              ///< ATTRIBUTE : the progression of the scheduler between the beginning and the end [0. :: 1.]
    TTFloat64                       mDate;                  ///< ATTRIBUTE : how many time the scheduler is running (without no speed factor consideration)
    
    SchedulerPositionCallback       mCallback;              ///< the callback to use for each step
    TTPtr                           mBaton;                 ///< the baton to use for each step
    
public:
    
	//** Constructor.	*/
	Scheduler(const TTValue& arguments);
	
	/** Destructor. */
	virtual ~Scheduler();
	
	/** Get specific parameters names needed by this scheduler 
     @return        an error code if the scheduler fails to give his specific parameters */
	virtual TTErr getParameterNames(TTValue& value) = 0;
	
	/** Start the scheduler 
     @return        an error code if the scheduler fails to start */
    virtual TTErr Go() = 0;
    
    /** Halt the sheduler 
     @return        an error code if the scheduler fails to stop */
    virtual TTErr Stop() = 0;
    
    /** Pause the sheduler progression 
     @return        an error code if the scheduler fails to pause */
    virtual TTErr Pause() = 0;
    
    /** Resume the sheduler progression 
     @return        an error code if the scheduler fails to resume */
    virtual TTErr Resume() = 0;
    
    /** Called every time a new step should be processed 
     @return        an error code if the scheduler step fails  */
    virtual TTErr Tick() = 0;
    
    /** set the duration attribute
     @value             new duration
     @return            kTTErrGeneric if the value is not a single TTFloat64 >= 0. */
    TTErr setDuration(const TTValue& value);
    
    /** set the offset attribute
     @value             new offset
     @return            kTTErrGeneric if the value is not a single TTFloat64 */
    TTErr setOffset(const TTValue& value);
    
    /** set the speed factor attribute 
     @value             new speed factor
     @return            kTTErrGeneric if the value is not a single TTFloat64 */
    TTErr setSpeed(const TTValue& value);

};
typedef Scheduler* SchedulerPtr;

#endif	//__SCHEDULER_H__



#ifndef __SCHEDULERLIB_H__
#define __SCHEDULERLIB_H__

class TT_EXTENSION_EXPORT SchedulerLib {
public:

	/**	Return a list of all available Schedulers. */
	static void getSchedulerNames(TTValue& SchedulerNames);
    
    /**	Return if a scheduler name is available */
	static TTErr isSchedulerNameAvailable(TTSymbol aSchedulerName);
};

#endif	//__SCHEDULERLIB_H__
