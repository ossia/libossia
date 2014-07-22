/** @file
 *
 * @ingroup dspLibrary
 *
 * @brief The #TTAudioEngine class is the Audio Engine of Jamoma DSP
 *
 * @details #TTAudioEngine is a class that is used to drive realtime audio and scheduling operations in the Jamoma DSP environment.
 * It is currently implemented as a wrapper around PortAudio.
 *
 * QUESTIONS
 *
 * - Should this be a singleton, like the environment object?
 * - How do we properly clean-up the environment from something like Max?  I guess we need a quittask?
 *
 * THOUGHTS
 *
 * - A #TTAudioOutput class will work by writing to the #TTAudioEngine's output buffer.
 * - Likewise a #TTAudioInput class will work by retrieving from the #TTAudioEngine's input buffer.
 * - The scheduler, and others like the Jamoma AudioGraph output class, will subscribe to this class for notifications on each call from PortAudio.
 *
 * @authors Tim Place, Nathan Wolek, Trond Lossius
 *
 * @copyright Copyright © 2008 by Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_AUDIOENGINE_H__
#define __TT_AUDIOENGINE_H__

#include "TTDSP.h"
#include "TTAudioSignal.h"

#ifndef	TT_PLATFORM_IOS
#include "portaudio.h"
#else
#error foo
#endif

/**	
 The #TTAudioEngine class is the Audio Engine of Jamoma DSP.
 */
class TTDSP_EXPORT TTAudioEngine : public TTObjectBase {
	TTCLASS_SETUP(TTAudioEngine)
	
protected:

	TTUInt16			mNumInputChannels;		///< The number of input channels
	TTUInt16			mNumOutputChannels;		///< The number of output channels
	TTUInt16			mVectorSize;			///< Vector size (frames per buffer)
	TTUInt32			mSampleRate;			///< Sample rate
    PaStream*			mStream;
	TTListPtr			mCallbackObservers;
	TTSymbol			mInputDevice;			///< The audio device used for audio input
	TTSymbol			mOutputDevice;			///< The audio device used for audio output
	const PaDeviceInfo*	mInputDeviceInfo;		///< A structure providing information and capabilities of PortAudio devices, including information on latency and max number of channels. Refer to the documentation of PortAudio for further references.
	const PaDeviceInfo*	mOutputDeviceInfo;		///< A structure providing information and capabilities of PortAudio devices, including information on latency and max number of channels. Refer to the documentation of PortAudio for further references.
	TTInt16				mInputDeviceIndex;		///<
	TTInt16				mOutputDeviceIndex;		///<
	TTBoolean			mIsRunning;				///< A flag indicating if audio is currently being processed

public:

	TTAudioSignalPtr	mInputBuffer;
	TTAudioSignalPtr	mOutputBuffer;

public:

	// we are a singleton, so this is how we work with the lifecycle...
	
	/**
	 */
	static TTObjectBasePtr	sSingletonInstance;
	
	
	/**
	 */
	static TTObjectBasePtr create();
	
	
	/**
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	static TTErr destroy();	
	
	
	/**
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr initStream();
	
	
	/** Start audio processing.
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr start();
	
	
	/** Stop audio processing.
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr stop();
	
	
	/** Monitor how taxing current audio processing is on the CPU.
	 @param unusedInput		Not used
	 @param returnedValue	Used to return current CPU load.
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getCpuLoad(const TTValue& unusedInput, TTValue& returnedValue);
	
	
	/** Get the names of all available input devices.
	 @param unusedInput		Not used
	 @param returnedDeviceNames	Used to return the names of all available input devices.
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getAvailableInputDeviceNames(const TTValue& unusedInput, TTValue& returnedDeviceNames);
	
	
	/** Get the names of all available output devices.
	 @param unusedInput		Not used
	 @param returnedDeviceNames	Used to return the names of all available output devices.
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr getAvailableOutputDeviceNames(const TTValue& unusedInput, TTValue& returnedDeviceNames);
	
	
	/** Get the reference pointer for the input signal.
	 @return				The reference pointer for the input signal.
	 */
	TTAudioSignalPtr TTAudioEngineGetInputSignalReference();
	
	
	/** Get the reference pointer for the output signal.
	 @return				The reference pointer for the output signal.
	 */
	TTAudioSignalPtr TTAudioEngineGetOutputSignalReference();

	///// Attribute Accessors /////
	
	/** Set what audio input device to use.
	 @param newDeviceName	The new audio input device to use.
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setInputDevice(TTValue& newDeviceName);
	
	
	/** Set what audio output device to use.
	 @param newDeviceName	The new audio output device to use.
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setOutputDevice(TTValue& newDeviceName);
	
	
	/** Set vector size.
	 @param newVectorSize	The new vector size to use.
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setVectorSize(TTValue& newVectorSize);
	
	
	/** Set the sample rate.
	 @param newSampleRate	The new sample rate to use.
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setSampleRate(TTValue& newSampleRate);
	
	
	/**
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr addCallbackObserver(const TTValue& objectToReceiveNotifications, TTValue& unusedOutput);
	
	
	/**
	 @return				#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr removeCallbackObserver(const TTValue& objectCurrentlyReceivingNotifications, TTValue& unusedOutput);
	
	/** This is called repeatedly by PortAudio every time a new vector of audio is needed.
	 @param input
	 @param output
	 @param frameCount
	 @param timeInfo
	 @param statusFlags
	 @return
	 */
	TTInt32 callback(const TTFloat32*					input, 
					 TTFloat32*							output, 
					 TTUInt32							frameCount, 
					 const PaStreamCallbackTimeInfo*	timeInfo, 
					 PaStreamCallbackFlags				statusFlags);	
};


/** A pointer to a #TTAudioEngine.
 @ingroup typedefs
 */
typedef TTAudioEngine* TTAudioEnginePtr;




/** A C-function used for the callback from PortAudio.  
 @details This simply passes the call to the callback method in the TTAudioEngine object.
 */
int TTAudioEngineStreamCallback(const void*						input,
								void*							output,
								unsigned long					frameCount,
								const PaStreamCallbackTimeInfo*	timeInfo,
								PaStreamCallbackFlags			statusFlags,
								void*							userData ) ;



#endif // __TT_AUDIOENGINE_H__

