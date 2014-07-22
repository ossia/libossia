/** @file
 *
 * @ingroup dspLibrary
 *
 * @brief Basic audio delay unit with 4 interpolation options.
 * 
 * @authors Timothy Place & Nathan Wolek
 *
 * @copyright Copyright © 2003-2012, Timothy Place & Nathan Wolek @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_DELAY_H__
#define __TT_DELAY_H__

#include "TTDSP.h"
#include "TTDelayBuffer.h"


/**	Delay a signal. */
class TTDSP_EXPORT TTDelay : public TTAudioObjectBase {
	TTCLASS_SETUP(TTDelay)

protected:

	TTFloat64			mDelay;
	TTFloat64			mDelayInSamples;	///< mIntegralDelay + mFractionalDelay
	TTFloat64			mDelayMax;
	TTUInt64			mDelayMaxInSamples;
	TTSymbol			mInterpolation;
	TTFloat64			mFractionalDelay;	///< mDelayInSamples - mFractionalDelay. Used in interpolated dsp loops, to approximate the distance between two samples. If zero then the delay increment is precisely on a sample boundary.
	TTUInt64			mIntegralDelay;		///< mDelayInSamples - mFractionalDelay. Used to set the TTDelayBuffer->mReadPointer.

	TTDelayBufferVector	mBuffers;

	
	/**	This method gets called when the inherited maxNumChannels attribute is changed. */
	TTErr updateMaxNumChannels(const TTValue& oldMaxNumChannels, TTValue&);

	/** Receives notifications when there are changes to the inherited sr attribute. */
	TTErr updateSampleRate(const TTValue& oldSampleRate, TTValue&);
	
	// internal - set up the buffer memory
	TTErr init(TTUInt64 newDelayMaxInSamples);

	// internal - position the buffer pointers
	void reset();

	/** Zero out the delay's buffer. */
	TTErr clear();
	
	/** Attribute Accessor */
	TTErr setDelay(const TTValue& newValue);
	
	/** Attribute Accessor */
	TTErr setDelayInSamples(const TTValue& newValue);
	
	/** Attribute Accessor */
	TTErr setDelayMax(const TTValue& newValue);
	
	/** Attribute Accessor */
	TTErr setDelayMaxInSamples(const TTValue& newValue);
	
	/** Attribute Accessor */
	TTErr setInterpolation(const TTValue& newValue);

	// Process with a constant delay time
	TTErr calculateNoInterpolation(const TTFloat64& x, TTFloat64& y, TTDelayBufferPtr data);
	TTErr processAudioNoInterpolation(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr calculateLinearInterpolation(const TTFloat64& x, TTFloat64& y, TTDelayBufferPtr data);
	TTErr processAudioLinearInterpolation(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	
	TTErr calculateCosineInterpolation(const TTFloat64& x, TTFloat64& y, TTDelayBufferPtr data);
	TTErr processAudioCosineInterpolation(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	TTErr calculateCubicInterpolation(const TTFloat64& x, TTFloat64& y, TTDelayBufferPtr data);
	TTErr processAudioCubicInterpolation(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	/**	Unit Tests	*/
	virtual TTErr test(TTValue& returnedTestInfo);	
	
	// Process with a delay time set by a signal
//	TTErr processAudioNoInterpolationWithDelaySignal(TTAudioSignal& in, TTAudioSignal& delayIn, TTAudioSignal& out, TTAudioSignal&);
//	TTErr processAudioLinearInterpolationWithDelaySignal(TTAudioSignal& in, TTAudioSignal& delayIn, TTAudioSignal& out, TTAudioSignal&);
//	TTErr processAudioCubicInterpolationWithDelaySignal(TTAudioSignal& in, TTAudioSignal& delayIn, TTAudioSignal& out, TTAudioSignal&);
	
public:

	TTErr calculateNoInterpolation(const TTFloat64& x,		TTFloat64& y,	TTPtrSizedInt channel);
	TTErr calculateLinearInterpolation(const TTFloat64& x,	TTFloat64& y,	TTPtrSizedInt channel);
	TTErr calculateCosineInterpolation(const TTFloat64& x,	TTFloat64& y,	TTPtrSizedInt channel);
	TTErr calculateCubicInterpolation(const TTFloat64& x,	TTFloat64& y,	TTPtrSizedInt channel);	
	
};


#endif // __TT_DELAY_H__
