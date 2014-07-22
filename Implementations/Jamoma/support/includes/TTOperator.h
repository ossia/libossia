/* 
 * Jamoma DSP Operator Object
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_OPERATOR_H__
#define __TT_OPERATOR_H__

#include "TTDSP.h"


/**	TTOperator performs basic mathematical operations on an input signal, or a pair of input signals */
class TTOperator : TTAudioObjectBase {
	TTCLASS_SETUP(TTOperator)

protected:

	TTSymbol		mOperator;				///< The type of calculation to perform
	TTFloat64		mOperand;				///< The right-hand value of the calculation
	TTBoolean		mOperandIsInteger;		///< Set to true if there is no fractional part
	TTBoolean		mOperandIsPowerOfTwo;	///< Set to true is the operand is a power of two to optimize some operations

	
	TTErr calculateAdd(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateSubtract(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateMultiply(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateDivide(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateModulo(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateSqrt(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateFabs(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateEqual(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateNotEqual(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateGreaterEqual(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateGreater(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateSmallerEqual(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	TTErr calculateSmaller(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data);
	
	/**	A standard audio processing method as used by TTBlue objects. */
	TTErr processAdd				(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processAddSignal			(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr processSubtract			(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processSubtractSignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr processMultiply			(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processMultiplySignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr processDivide				(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processDivideSignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr processModulo				(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processModuloSignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr processSqrt				(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	
	TTErr processFabs				(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	
	TTErr processEqual				(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processEqualSignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr processNotEqual			(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processNotEqualSignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr processGreater			(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processGreaterSignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr processGreaterEqual			(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processGreaterEqualSignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr processSmaller			(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processSmallerSignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	
	TTErr processSmallerEqual			(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	TTErr processSmallerEqualSignal		(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	
#define WRAP_C_FUNCTION_AS_TTOp_METHODS(cName, methodName) \
	TTErr calculate ## methodName (const TTFloat64& x, TTFloat64& y, TTPtrSizedInt data)\
	{\
		y = cName (x);\
		return kTTErrNone;\
	}\
	\
	TTErr process ## methodName (TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs)\
	{\
		TT_WRAP_CALCULATE_METHOD(calculateFabs);\
	}\
	
	WRAP_C_FUNCTION_AS_TTOp_METHODS(acos, Acos)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(asin, Asin)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(atan, Atan)
	//WRAP_C_FUNCTION_AS_TTOp_METHODS(atan2, Atan2)	// TODO: this one requires 2 args
	WRAP_C_FUNCTION_AS_TTOp_METHODS(ceil, Ceil)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(cos, Cos)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(cosh, Cosh)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(exp, Exp)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(floor, Floor)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(log, Log)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(log10, Log10)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(sin, Sin)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(sinh, Sinh)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(tan, Tan)
	WRAP_C_FUNCTION_AS_TTOp_METHODS(tanh, Tanh)
	
	
public:
	
	/** setter for the operator attribute. */
	TTErr setOperator(const TTValue& newValue);
	
	/** setter for the operand attribute. */
	TTErr setOperand(const TTValue& newValue);
};


#endif // __TT_OPERATOR_H__
