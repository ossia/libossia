/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A Mapper Object
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_MAPPER_H__
#define __TT_MAPPER_H__

#include "TTModularIncludes.h"

/**	TTMapper ... TODO : an explanation
 
 
 */

class TTReceiver;
typedef TTReceiver* TTReceiverPtr;

class TTSender;
typedef TTSender* TTSenderPtr;

class TTMODULAR_EXPORT TTMapper : public TTObjectBase
{
	TTCLASS_SETUP(TTMapper)
	
private:
	
	TTAddress			mInput;						///< ATTRIBUTE : address of the data for the incoming value
	
	TTFloat64			mInputMin;					///< ATTRIBUTE : 
	TTFloat64			mInputMax;					///< ATTRIBUTE :
    TTUInt32            mInputIndex;                ///< ATTRIBUTE : select an index of the input value to map (0 means to take the whole value)
    TTFloat64			mInputThresholdDown;        ///< ATTRIBUTE :
	TTFloat64			mInputThresholdUp;			///< ATTRIBUTE :
    TTBoolean			mInputGoingDown;            ///< memorize last state to filter repetitions
	TTBoolean			mInputGoingUp;              ///< memorize last state to filter repetitions
    
    TTAddress			mOutput;					///< ATTRIBUTE : address of the data for the value to control
    
	TTFloat64			mOutputMin;					///< ATTRIBUTE :
	TTFloat64			mOutputMax;					///< ATTRIBUTE :
    TTFloat64			mOutputThresholdDown;       ///< ATTRIBUTE :
	TTFloat64			mOutputThresholdUp;			///< ATTRIBUTE :
    TTBoolean			mOutputGoingDown;           ///< memorize last state to filter repetitions
	TTBoolean			mOutputGoingUp;             ///< memorize last state to filter repetitions
	
	TTBoolean			mActive;					///< ATTRIBUTE : does it observe Input value ?
	
	TTBoolean			mInverse;					///< ATTRIBUTE : to make the output equal to mOutputMax - result
#ifndef TT_NO_DSP	
	TTValue				mFunctionLibrary;			///< ATTRIBUTE : names of all available function from FunctionLib
	TTSymbol			mFunction;					///< ATTRIBUTE : name of the map function
	TTValue				mFunctionParameters;		///< ATTRIBUTE : names of parameter's function
	TTValue				mFunctionSamples;			///< ATTRIBUTE : an overview of the mapping (each value between [map(InputMin) :: map(InputMax)])
#endif
    TTUInt32            mRamp;                      ///< ATTRIBUTE : a ramp time to pass to the output data
	
	TTObject            mReceiver;					///< the receiver which binds on In data(s)
	TTObject            mSender;					///< the sender which binds on Out data(s)
	
	TTObject            mInputObserver;				///< the receiver which observe the input address creation/destruction
	TTObject            mOutputObserver;			///< the receiver which observe the output address creation/destruction
	
	TTObject            mInputRangeObserver;		///< the receiver which observe the input rangeBounds attribute
	TTObject            mOutputRangeObserver;		///< the receiver which observe the output rangeBounds attribute
	TTBoolean			mObserveInputRange;			///< do observe Input rangeBounds attribute ?
													///< Set to true if there are a setting Input address
													///< Set to false if there are a setting of Input Min/Max value
	TTBoolean			mObserveOutputRange;		///< do observe Output rangeBounds attribute ?
													///< Set to true if there are a setting Output address
													///< Set to false if there are a setting of Output Min/Max value
	
	TTObject            mReturnValueCallback;		///< a way to return back value to the owner of this mapper
    
    TTObject            mReturnInputGoingDownCallback;///< a way to return back if the input value is lower than or equal to the down threshold
    TTObject            mReturnInputGoingUpCallback;///< a way to return back if the input value is greater than or equal to the up threshold
    
    TTObject            mReturnOutputGoingDownCallback;///< a way to return back if the output value is lower than or equal to the down threshold
    TTObject            mReturnOutputGoingUpCallback;///< a way to return back if the output value is greater than or equal to the up threshold
	
	TTFloat64			mA, mB, mC, mD;				//< Coefficients used for normalizing input(A, B) and output (C, D)
#ifndef TT_NO_DSP
	TTObject            mFunctionUnit;
	TTBoolean			mValid;						//< true if the functionUnit can be used
#endif
	
	/** process mapping */
	TTErr Map(TTValue& inputValue, TTValue& outputValue);
#ifndef TT_NO_DSP	
	/** */
	TTErr getFunctionLibrary(TTValue& value);
	
	/** */
	TTErr getFunctionSamples(TTValue& value);
#endif
	/** set the input address and set InputMin and InputMax using RangeBounds attributes */
	TTErr setInput(const TTValue& value);
	TTErr observeInput();
	TTErr observeInputRange();
	
	/**  set the output address and set OutputMin and OutputMax using RangeBounds attributes */
	TTErr setOutput(const TTValue& value);
	TTErr observeOutput();
	TTErr observeOutputRange();
	
	/** */
	TTErr setInputMin(const TTValue& value);
	
	/** */
	TTErr setInputMax(const TTValue& value);
	
	/** */
	TTErr setOutputMin(const TTValue& value);
	
	/** */
	TTErr setOutputMax(const TTValue& value);
	
	/** */
	TTErr setActive(const TTValue& value);
#ifndef TT_NO_DSP	
	/** set the function unit and set datas name extending attributes of the unit */
	TTErr setFunction(const TTValue& value);
#endif
	/** process mapping */
	TTErr processMapping(const TTValue& inputValue, TTValue& outputValue);
	
	/** */
	TTErr scaleInput();
	
	/** */
	TTErr scaleOutput();
	
	/** */
	TTErr notifyObservers(TTSymbol attrName, const TTValue& value);
	
	friend TTErr TTMODULAR_EXPORT TTMapperInputCreationCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTMapperOutputCreationCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTMapperInputRangeCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTMapperOutputRangeCallback(const TTValue& baton, const TTValue& data);
	friend TTErr TTMODULAR_EXPORT TTMapperReceiveValueCallback(const TTValue& baton, const TTValue& data);
};

typedef TTMapper* TTMapperPtr;

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTMapperInputCreationCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTMapperOutputCreationCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTMapperInputRangeCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTMapperOutputRangeCallback(const TTValue& baton, const TTValue& data);

/**	
 @param	baton						..
 @param	data						..
 @return							an error code */
TTErr TTMODULAR_EXPORT TTMapperReceiveValueCallback(const TTValue& baton, const TTValue& data);

#endif // __TT_MAPPER_H__
