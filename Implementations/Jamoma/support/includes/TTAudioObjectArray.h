/** @file
 *
 * @ingroup dspLibrary
 *
 * @brief Wrapper for an array of audio objects
 *
 * @details
 *
 * @see TTAudioObjectBase
 *
 * @authors Tim Place, Nils Peters, Trond Lossius
 *
 * @copyright Copyright © 2011 by Timothy Place and Nils Peters @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_AUDIO_OBJECT_ARRAY_H__
#define __TT_AUDIO_OBJECT_ARRAY_H__

#include "TTDSP.h"
#include "TTAudioObjectBase.h"

typedef std::vector<TTAudioObjectBasePtr>	TTAudioObjectBaseVector;
typedef TTAudioObjectBaseVector::iterator	TTAudioObjectBaseIter;


/**	TTAudioObjectBaseArray instantiates an array of TTAudioObjectBases, manages their life-cycles, and process audio through the array.
 @details This permits the processing of an N-channel audio signal through N instances of a given class while using different attribute values for the 
 One application of such a structure is a graphic equalizer.
 */
class TTAudioObjectBaseArray : public TTAudioObjectBase {
	TTCLASS_SETUP(TTAudioObjectBaseArray)
	
protected:
	
	TTSymbol			mClass;					///< The name of the object we will instantiate in the array
	TTUInt16			mSize;					///< The number of instances in the array
	TTAudioObjectBaseVector	mInstances;			///< The actual vector containing object instance pointers
	TTAudioSignalPtr	mInputChannelSignal;	///< Signal used within the process method for passing to individual instances
	TTAudioSignalPtr	mOutputChannelSignal;	///< Signal used within the process method for passing to individual instances
	
	//// accessors /////
	
	/** Update the maximum number of channels that the array will be able to process.
	 @param oldMaxNumChannels
	 @param
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr updateMaxNumChannels(const TTValue& oldMaxNumChannels, TTValue&);
	
	
	/**
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setSize(const TTValueRef newSize);
	
	
	/**
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setClass(const TTValueRef newClass);

	//// access the objects inside the array ///
	
	/**
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr set(TTValue& arguments, TTValue&);
	
	
	/** Process audio.
	 @param inputs		The incoming audio signals to be processed.
	 @param outputs		The processed audio signals.
	 @return			#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr processAudio(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	
	
	/** Unit test for the TTAudioObjectBaseArray.
	 @param returnedTestInfo		The outcome from the performed unit test.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	virtual TTErr test(TTValue& returnedTestInfo);
};


#endif // __TT_AUDIO_OBJECT_ARRAY_H__

