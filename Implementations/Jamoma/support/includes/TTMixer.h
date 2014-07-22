/** @file
 *
 * @ingroup dspMathLib
 *
 * @brief #TTMixer is a Jamoma DSP Audio Mixer Object
 *
 * @details
 *
 * @authors Tim Place, Nathan Wolek, Trond Lossius
 *
 * @copyright Copyright © 2010, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_MIXER_H__
#define __TT_MIXER_H__

#include "TTDSP.h"


/**	TTMixer mixes N input channels of a signal to M output channels of a signal. */
class TTMixer : TTAudioObjectBase {
	TTCLASS_SETUP(TTMixer)

protected:

	TTMatrix		mGainMatrix;					///< A NxM matrix of mixing coefficients.
	TTBoolean		mInterpolated;					///< A flag determine if we will be interpolating towards new matrix values whenever a gain value is being updated.
	TTMatrix        oldGainMatrix;                  ///< Required for the process of resizing when changing the number of inputs or outputs.
    TTMatrix        tempGainMatrix;                  ///< Required for the process of resizing when changing the number of inputs or outputs.
	TTChannelCount	mNumInputs;						///< The number of audio inputs.
	TTChannelCount	mNumOutputs;					///< The number of audio outputs.
	
	///// Attributes ///
	
	/** Set the number of audio input channels to the mixer.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setNumInputs(const TTValue& newValue);
	
	
	/** Set the number of audio output channels from the mixer.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setNumOutputs(const TTValue& newValue);
	
	
	/** Check what the current mixer size is.
	 @param x						The current number of audio input signals
	 @param y						The current number of audio output signals
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr checkMatrixSize(const TTChannelCount x, const TTChannelCount y);
	
	//TTErr setInterpolated(const TTValue& newValue);
	
	
	///// Messages /////
	
	/** Clear the matrix by setting all gain coefficients to 0.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr clear();
	
	
	/** Restore matrix coefficients. 
	 @details This method is used when the matrix is being resized.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr restoreMatrix();
	
	
	/** Set the gain as desibel for one inlet to one outlet.
	 @param newValue				An array of three values is expected:
	 -# The audio inlet.
	 -# The audio outlet.
	 -# New gain value in desibel for this inlet-outlet pair
	 @param
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setGain(const TTValue& newValue, TTValue&);
	
	
	/** Set the gain as linear amplitude for one inlet to one outlet.
	 @param newValue				An array of three values is expected:
	 -# The audio inlet.
	 -# The audio outlet.
	 -# New gain value as linear amplitude for this inlet-outlet pair
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setLinearGain(const TTValue& newValue, TTValue&);
	
	
	/** Set the gain as midi value for one inlet to one outlet.
	 @param newValue				An array of three values is expected:
	 -# The audio inlet.
	 -# The audio outlet.
	 -# New gain value as MIDI value for this inlet-outlet pair
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr setMidiGain(const TTValue& newValue, TTValue&);	
	
	///// Audio /////
	
	/** Process audio.
	 @details This version of the processing method is the simpler and more efficient. It will be used
	 whenever no gain values have been recently changes. If the #mInterpolated flag is NOT set,
	 it will also be used when gain coefficients change. This saves CPU as compared to the more expensive
	 #processAudioInterpolated method, but at the risk of audible clicks and discontinuities.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr processAudio(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
	
	
	/** Process audio with interpolation.
	 @details If a gain coefficient has been updated, and the #mInterpolated flag is set, 
	 this method will be used for the next vector to be processed, and will ensure that matrix coefficients
	 ramp (interpolate) to their new values over the duration of the next vector.
	 Upon completion, this unit will be set to use the simpler and more efficient #processAudio method again,
	 until next time a matrix gain coefficient is changed.
	 @return						#TTErr error code if the method fails to execute, else #kTTErrNone.
	 */
	TTErr processAudioInterpolated(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);
};


#endif // __TT_MIXER_H__
