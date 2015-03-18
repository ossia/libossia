/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Clock interface
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2013, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_CLOCK_H__
#define __TT_CLOCK_H__

#include "TTFoundationAPI.h"
#include <atomic>

#define TT_CLOCK_CONSTRUCTOR \
TTObjectBasePtr thisTTClass :: instantiate (TTSymbol name, TTValue arguments) {return new thisTTClass (arguments);} \
\
extern "C" void thisTTClass :: registerClass () {TTClassRegister( TTSymbol(thisTTClassName), thisTTClassTags, thisTTClass :: instantiate );} \
\
thisTTClass :: thisTTClass (const TTValue& arguments) : TTClock(arguments)

#define TT_CLOCK_INITIALIZE \
mName = TTSymbol(thisTTClassName); \
mVersion = TTSymbol(thisClockVersion); \
mAuthor = TTSymbol(thisClockAuthor); \
mStretchable = TTSymbol(thisClockStretchable); \
registerAttribute(TTSymbol("parameterNames"), kTypeLocalValue, NULL, (TTGetterMethod)& thisTTClass::getParameterNames); \
/*addAttributeProperty(ParameterNames, readOnly, YES); \ */

typedef void (*TTClockPositionCallback)(TTPtr, TTFloat64, TTFloat64);

/****************************************************************************************************/
// Class Specification

/**	TTClock is the base class for all Clock Unit.
 It still has knowledge and support for ...
 */
class TTClock : public TTObjectBase {

public:
    
	TTSymbol                        mName;					///< ATTRIBUTE : the name of the clock							
	TTSymbol                        mVersion;				///< ATTRIBUTE : the version of the clock								
	TTSymbol                        mAuthor;				///< ATTRIBUTE : the author of the clock								
	TTBoolean                       mStretchable;			///< ATTRIBUTE : is the clock provide stretch feature ?

protected:
    
    TTFloat64                       mDuration;              ///< ATTRIBUTE : the time (in ms) the clock will run at normal speed factor
    TTFloat64                       mOffset;                ///< ATTRIBUTE : the date (in ms) the sheduler will run from
    TTFloat64                       mSpeed;                 ///< ATTRIBUTE : the speed factor of the clock
    TTBoolean                       mExternalTick;          ///< ATTRIBUTE : if true the Tick message comes from an external source
    TTBoolean                       mInfinite;              ///< ATTRIBUTE : if true the clock will run until Stop (this attribute overrides mDuration)
    
    bool                            mRunning;				///< ATTRIBUTE : is the clock running right now ?
    TTBoolean                       mPaused;                ///< ATTRIBUTE : is the clock paused right now ?
    TTFloat64                       mPosition;              ///< ATTRIBUTE : the progression of the clock between the beginning and the end [0. :: 1.]
    TTFloat64                       mDate;                  ///< ATTRIBUTE : how many time the clock is running (without no speed factor consideration)
    
    TTClockPositionCallback         mCallback;              ///< the callback to use for each step
    TTPtr                           mBaton;                 ///< the baton to use for each step
    
public:
    
	//** Constructor.	*/
	TTClock(const TTValue& arguments);
	
	/** Destructor. */
	virtual ~TTClock();
	
	/** Get specific parameters names needed by this clock 
     @return        an error code if the clock fails to give his specific parameters */
	virtual TTErr getParameterNames(TTValue& value) = 0;
	
	/** Start the clock 
     @return        an error code if the clock fails to start */
    virtual TTErr Go() = 0;
    
    /** Halt the sheduler 
     @return        an error code if the clock fails to stop */
    virtual TTErr Stop() = 0;
    
    /** Pause the sheduler progression 
     @return        an error code if the clock fails to pause */
    virtual TTErr Pause() = 0;
    
    /** Resume the sheduler progression 
     @return        an error code if the clock fails to resume */
    virtual TTErr Resume() = 0;
    
    /** Called every time a new step should be processed 
     @return        an error code if the clock step fails  */
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
typedef TTClock* TTClockPtr;

#endif	//__TT_CLOCK_H__



#ifndef __TT_CLOCKLIB_H__
#define __TT_CLOCKLIB_H__

class TT_EXTENSION_EXPORT TTClockLib {
public:

	/**	Return a list of all available Clocks. */
	static void getClockNames(TTValue& ClockNames);
    
    /**	Return if a clock name is available */
	static TTErr isClockNameAvailable(TTSymbol aClockName);
};

#endif	//__TT_CLOCKLIB_H__
