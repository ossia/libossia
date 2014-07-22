/** @file
 *
 * @ingroup dspLibrary
 *
 * @brief #TTAudioObjectBase is the Jamoma DSP Audio Object Base Class
 *
 * @details
 *
 * @authors Tim Place, Nils Peters, Trond Lossius
 *
 * @copyright Copyright © 2008 by Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_AUDIO_OBJECT_BASE_H__
#define __TT_AUDIO_OBJECT_BASE_H__

#include "TTObjectBase.h"
#include "TTSymbol.h"
#include "TTValue.h"
#include "TTAudioSignal.h"
#include "TTAudioSignalArray.h"


// Forward declaration of TTAudioObjectBase for the typedef that follows...
class TTAudioObjectBase;
class TTAudio;


/** A type that can be used to store a pointer to a process method (which calculates a vector of samples).
 @param in			Audio to be processed
 @param out			Processed audio
 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
 @ingroup typedefs
 */
typedef TTErr (TTAudioObjectBase::*TTProcessMethod)(TTAudioSignalArrayPtr in, TTAudioSignalArrayPtr out);


/**	A type that can be used to store a pointer to a calculate method (which calculates a single sample).
 @param x			A single sample to be processed
 @param y			A single processed sample
 @param data		TODO
 @ingroup typedefs
 */
typedef TTErr (TTAudioObjectBase::*TTCalculateMethod)(const TTFloat64& x, TTFloat64& y, TTPtr data);


/**	A convenience macro to be used by subclasses for setting the process method.
 @param	methodName	The name of the method to use for processing audio vectors.
 */
#define setProcessMethod(methodName)				setProcess((TTProcessMethod)& thisTTClass ::methodName )


/**	A convenience macro to be used by subclasses for setting the calculate method.
 @param	methodName	The name of the method to use for processing individual audio samples.
 */
#define setCalculateMethod(methodName)				setCalculate((TTCalculateMethod)& thisTTClass ::methodName )


#define TT_WRAP_CALCULATE_METHOD(methodName) \
TTAudioSignal&	in = inputs->getSignal(0); \
TTAudioSignal&	out = outputs->getSignal(0); \
TTUInt16		vs; \
TTSampleValue*	inSample; \
TTSampleValue*	outSample; \
TTChannelCount	numchannels = TTAudioSignal::getMinChannelCount(in, out); \
TTPtrSizedInt	channel; \
\
for (channel=0; channel<numchannels; channel++) { \
	inSample = in.mSampleVectors[channel]; \
	outSample = out.mSampleVectors[channel]; \
	vs = in.getVectorSizeAsInt(); \
	\
	while (vs--) { \
		methodName (*inSample, *outSample, channel); \
		outSample++; \
		inSample++; \
	} \
}\
return kTTErrNone;



/****************************************************************************************************/
// Class Specification

/**	TTAudioObjectBase is the base class for all audio generating and processing objects in Jamoma DSP.
 *	
 *	@details The theory of operation is that this class handles the public interface to any subclass,
 *	including the main processing method, which calls an appropriate method through a function pointer.
 *	By default, this points to the built-in bypassProcess().  Subclasses then set it to point to their 
 *	own process() method(s) as needed.
 */
class TTDSP_EXPORT TTAudioObjectBase : public TTObjectBase {
protected:
	TTUInt32				sr;							///< Current sample rate being used by this object
	TTFloat64				srInv;						///< 1.0 over the current sample rate (inverse)
	TTFloat64				srMill;						///< 1/1000 of the current sample rate (samples per millisecond)
	TTChannelCount			mMaxNumChannels;			///< This is the maximum number of channels that can be guaranteed to work
	TTBoolean				unused;						///< Old var that is not used anymore, but we want to keep the struct size the same
	TTBoolean				attrBypass;					///< Are we bypassing the processMethod?
	TTBoolean				attrMute;					///< Mute the processMethod.
	TTProcessMethod			processMethod;				///< This function pointer points to the active (non-bypass) processing routine.
	TTProcessMethod			currentProcessMethod;		///< This function pointer always points to the current processing routine.
	TTCalculateMethod		calculateMethod;			///< This function pointer points to the active (non-bypass) calculate routine.
	TTCalculateMethod		currentCalculateMethod;		///< This function pointer always points to the current calculate routine.
	TTObject				inputArray;					///< If the process method is passed a signal, rather than an array of signals, we wrap the signal in this array.
	TTObject				outputArray;				///< If the process method is passed a signal, rather than an array of signals, we wrap the signal in this array.
	TTFloat64				startProcessingTime;		///< The time at which this object's process method was last invoked (for benchmarking)
	TTFloat64				accumulatedProcessingTime;	///< The amount of time spent in this object's process method (for benchmarking)
	TTFloat64				accumulatedProcessingCalls;	///< The number of times the process method has been called (for benchmarking)
	
	
	/** Set the audio processing routine to point to a method that is defined as an arg to this function.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setProcess(TTProcessMethod processMethod);
	
	
	/** Set the sample calculate routine to point to a method that is defined as an arg to this function.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setCalculate(TTCalculateMethod newCalculateMethod);
	
	
	/** Bypass the audio processing routine and copy all input samples to the output unchanged.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setBypass(const TTValue& value);
	
	
	/**	Reset the benchmarking accumulation used to calculate the results of getProcessingBenchmark().
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr resetBenchmarking();
	
	
	/**	Return the average time spent by this object processing audio since the last reset.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getProcessingBenchmark(TTValueConstRef, TTValueRef v);
	
	
public:
	
	/** Mute the audio processing routine and zero all output.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setMute(const TTValue& value);

	
	/**	Setter for the maxNumChannels attribute.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setMaxNumChannels(const TTValue& newValue);

	
	/**	Increase the maxNumChannels attribute, if neccessary.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr adaptMaxNumChannels(const TTChannelCount newMaxNumChannels)
	{
		if (newMaxNumChannels > mMaxNumChannels)
			return setAttributeValue(kTTSym_maxNumChannels, newMaxNumChannels);
		else
			return kTTErrNone;
	}
	
	
	/**	Setter for the sample-rate attribute.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setSr(const TTValue& newValue);
	
	
	/**	Convenience method for updating the sample-rate.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setSampleRate(const TTUInt32& newSampleRate)
	{
		if (newSampleRate && newSampleRate != sr)
			return setAttributeValue(kTTSym_sampleRate, newSampleRate);
		else
			return kTTErrNone;
	}
	
	
protected:
	
	/** Constructor. 
	 @details Requires that the maximum number of channels to be used with this instance is defined.
	 @param arguments	Arguments to the constructor.
	 */
	TTAudioObjectBase(const TTValue& arguments);
	
	
public:
	
	/** Destructor. 
	 */
	virtual ~TTAudioObjectBase();
		
	
	/**	Calculate a single sample of output for a single sample of input. 
	 @param	x			The input to the function.
	 @param	y			The output of the function.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr calculate(const TTFloat64& x, TTFloat64& y);
	TTErr calculate(const TTValue& x, TTValue& y);
	
	
	/**
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr calculateMessage(TTValueConstRef input, TTValueRef output);

	
	/**	A process method that may be used by subclasses to wrap a calculate method in a semi-standard way.
	 @details			Unfortunately, this is slow.  An alternative is to use the #TT_WRAP_CALCULATE_METHOD macro.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr calculateProcess(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	
	/** Process the input signal, resulting in an output signal. This method wraps the actual process method that will be called.
	 @param in			The input signal.
	 @param out			The output signal.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr process(TTAudioSignal& in, TTAudioSignal& out);
	
	// Shortcut for when the caller is using pointers
	TTErr process(TTAudioSignal* in, TTAudioSignal* out)
	{
		return process(*in, *out);
	}
	
	
	/** Process the an output signal only, e.g. for a signal generator. This method wraps the actual process method that will be called.
	 @param in			The input signal.
	 @param out			The output signal.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr process(TTAudioSignal& out);

	// shortcut for when the caller is using pointers
	TTErr process(TTAudioSignal* out)
	{
		return process(*out);
	}
	
	
	/** Process two input signals, resulting in two output signals.
	 @details			This method wraps the actual process method, which may use the extra signal as a sidechain or other input.
	 @param in1			The main input signal.
	 @param in2			A secondary or sidechain input signal.
	 @param in1			The main output signal.
	 @param in2			A secondary or sidechain output signal.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr process(TTAudioSignal& in1, TTAudioSignal& in2, TTAudioSignal& out1, TTAudioSignal& out2);

	// Shortcut for when the caller is using pointers
	TTErr process(TTAudioSignal* in1, TTAudioSignal* in2, TTAudioSignal* out1, TTAudioSignal* out2)
	{
		return process(*in1, *in2, *out1, *out2);
	}
	
	
	/** Process two input signals, resulting in an output signal. 
	 @details			This method wraps the actual process method that will be called.
	 @param in1			The main input signal.
	 @param in2			A secondary or sidechain input signal.
	 @param out			The output signal.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr process(TTAudioSignal& in1, TTAudioSignal& in2, TTAudioSignal& out);

	// shortcut for when the caller is using pointers
	TTErr process(TTAudioSignal* in1, TTAudioSignal* in2, TTAudioSignal* out)
	{
		return process(*in1, *in2, *out);
	}
	
	/**	Process an array of audio signals.
	 @param inputs		The input audio signals to process.
	 @param outputs		The resulting processed audio signals.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr process(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	
	TTErr process(TTAudio& inputs, TTAudio& outputs);
	
	/**	The default audio processing method, which simply copies a signal through with no modifications.
	 @param inputs
	 @param outputs
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr bypassProcess(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	/**	Produces a result for calls to calculate when the unit is bypassed.
	 @param inputs		The audio input signals that will be passed through with no further processing.
	 @param outputs		The resulting unprocessesed (bypassed) audio signals.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr bypassCalculate(const TTFloat64& x, TTFloat64& y, TTPtr data);
	
	/**	A muted audio processing method, which simply copies zeroes to the output.
	 @param inputs		The audio input signals to be muted
	 @param outputs		The processed (muted) audio signals.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr muteProcess(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	
	
	/**	A default calculate method, which simply calls the process method with 1 channel signals that are 1 sample in length.  
	 @details This method is not fast, and if you anticipate the calculate method to be called often, you should write a proper
		calculate method in the subclass.
	 @param x
	 @param y
	 @param data
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr defaultCalculateMethod(const TTFloat64& x, TTFloat64& y, TTPtr data);
	
	
	// UTILITIES
	
	/**	Convert Hertz to radians.
	 @details Be sure to set the sr for this object before calling this function.
	 @param hz			Frequency expressed in Hz.
	 @return			Frequency expressed in radians.
	 */
	TTFloat64 hertzToRadians(const TTFloat64 hz);

	/**	Convert radians into Hertz.
	 @details Be sure to set the sr for this object before calling this function.
	 @param radians		Frequency expressed in radians.
	 @return			Frequency expressed in Hz.
	 */
	TTFloat64 radiansToHertz(const TTFloat64 radians);

	/** Degrees-to-radians conversion. 
	 @param degrees		Frequency expressed in degrees.
	 @return			Frequency expressed in radians.
	 */
	TTFloat64 degreesToRadians(const TTFloat64 degrees);

	/** Radians-to-degrees conversion. 
	 @param radians		Frequency expressed in radians.
	 @return			Frequency expressed in degrees.
	 */
	TTFloat64 radiansToDegrees(const TTFloat64 radians);

	/** Decay Time (seconds) to feedback coefficient conversion.
	 @param decay_time	-60 dB decay time in seconds.
	 @param delay		The delay time of the feedback loop.
	 @return			Calculated feedback coefficient.
	 */
	TTFloat64 decayToFeedback(const TTFloat64 decay_time, TTFloat64 delay);

	/** Return the decay time based on the feedback coefficient.
	 @param feedback	The feedback coefficient.
	 @param delay		The delay time of the feedback loop.
	 @return			Calculated -60 dB decay time.
	 */
	TTFloat64 feedbackToDecay(const TTFloat64 feedback, const TTFloat64 delay);

	
	
	/** Default (empty) template for unit tests.
	 @param returnedTestInfo		Returned information on the outcome of the unit test(s)
	 @return						#kTTErrNone if tests exists and they all pass, else #TTErr error codes depending on the outcome of the test.
	 */
	virtual TTErr test(TTValue& returnedTestInfo)
	{
		logMessage("No Tests have been written for this class -- please supply a test method.\n");
		return kTTErrGeneric;
	}

};


/** Pointer to a #TTAudioObjectBase.
 @ingroup typedefs
 */
typedef TTAudioObjectBase* TTAudioObjectBasePtr;


/** Convert linear amplitude into deciBels.
 @todo This could probably be using DataspaceLib instead?
 @param value		The linear gain value to convert.
 @return			Converted decibel value.
 */
inline TTFloat64 TTLinearGainToDecibels(const TTFloat64 value)
{
	if (value >= 0) 
		return(20.0 * (log10(value)));
	else
	 	return 0;
}


/** Convert decibels into linear ampliude.
 @todo This could probably be using DataspaceLib instead?
 @param value		The decibel value to convert.
 @return			The converted linear gain value.
 */
inline TTFloat64 TTDecibelsToLinearGain(TTFloat64 value)
{
	return(pow(10., (value / 20.)));
}


/** Convert midi into linear amplitude. 
 @todo This could probably be using DataspaceLib instead?
 @param value		The midi gain value to convert.
 @return			The converted linear gain value.
 */
inline TTFloat64 TTMidiToLinearGain(TTFloat64 value)
{
	return pow(value * 0.01, kTTGainMidiPower);
}

	
/** Convert linear amplitude into midi. 
 @todo This could probably be using DataspaceLib instead?
 @param value		A linear amplitude to convert.
 @return			The converted midi gain value.
 */
inline TTFloat64 TTLinearGainToMidi(TTFloat64 value)
{
	return 100.0 * pow(value, kTTGainMidiPowerInv);
}
	

/** Generate the next prime number higher than the value passed in.
 @param value		The number passed in.
 @return			The next prime number higher than the number passed in.
 */
inline TTUInt32 TTPrime(TTUInt32 value)
{
	long	candidate, last, i, isPrime;

   	if (value < 2)
  		candidate = 2;
	else if (value == 2)
		candidate = 3;
	else {
		candidate = value;
		if (candidate % 2 == 0)								// Test only odd numbers
			candidate--;
		do{
			isPrime = true;									// Assume glorious success
			candidate += 2;									// Bump to the next number to test
			last = TTUInt32(sqrt((TTFloat32)candidate));  	// We'll check to see if candidate has any factors, from 2 to last
			for (i=3; (i <= last) && isPrime; i+=2) {		// Loop through odd numbers only
				if ((candidate % i) == 0)
				isPrime = false;
			}
		} 
		while (!isPrime);
	}
	return candidate;
}


/** An idiosyncratic utility for slightly randomizing a number.
 Specifically this is used in applications such as randoming delay times for a reverb.
 @param value		The value to randomise
 @param aSampleRate Current sample rate
 @return			The slightly randomised value
 */
inline TTFloat64 TTDeviate(TTFloat64 value, TTFloat64 aSampleRate = 1.0)
{   //TODO use Mersedian Twister for rand-generator
	value += (2.0 * (TTFloat32(rand()) / TTFloat32(RAND_MAX))) - 1.0;	// randomize input with +1 to -1 ms
	value = value * 0.001 * aSampleRate;								// convert from ms to samples
	value = (TTFloat32)TTPrime(TTUInt32(value));						// find the nearest prime number (in samples)
	value = (value / aSampleRate) * 1000.0;								// convert back to ms
	
	return value;
}


#endif // __TT_AUDIO_OBJECT_BASE_H__
