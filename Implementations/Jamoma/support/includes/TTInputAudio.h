/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief An audio input component for Jamoma models.
 *
 * @details
 *
 * @authors Timothy Place
 *
 * @copyright © 2013, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_INPUT_AUDIO_H__
#define __TT_INPUT_AUDIO_H__

#include "TTInput.h"

/** An audio input component for Jamoma models.
 */
class TTMODULAR_EXPORT TTInputAudio : public TTInput
{
	TTCLASS_SETUP(TTInputAudio)

public:
	/**	Called by the perform method in j.in~ to pass the samples in from the outside world
		Will also be called by the Audiog Graph with j.in=
	 
		@see allpass1~.model
	 */
	void process(TTSampleValue* anInputSampleVector, TTSampleValue* anOutputSampleVector, TTUInt16 aVectorSize);

	
	/**	Used in e.g. allpass1~.model
		Is it used by j.receive~ ???
	 */
	TTSampleValue* getVector()
	{
		return TTAudioSignalPtr(mSignalOut.instance())->mSampleVectors[0];
	}
	
	/**	Used e.g. by the dsp method in j.in~
	 */
	void setupAudioSignals(TTUInt16 aVectorSize)
	{
		mSignalIn.set(kTTSym_numChannels, 1);
		mSignalOut.set(kTTSym_numChannels, 1);
        mSignalZero.set(kTTSym_numChannels, 1);
        
		mSignalIn.set(kTTSym_vectorSize, aVectorSize);
		mSignalOut.set(kTTSym_vectorSize, aVectorSize);
        mSignalZero.set(kTTSym_vectorSize, aVectorSize);
		
		mSignalOut.send(kTTSym_alloc);
        mSignalZero.send(kTTSym_alloc);
        mSignalZero.send(kTTSym_clear);
	}
};

typedef TTInputAudio* TTInputAudioPtr;


#endif // __TT_INPUT_AUDIO_H__
