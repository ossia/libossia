/** @file
 *
 * @ingroup dspLibrary
 *
 * @brief Container for an array of #TTAudioSignal pointers.
 *
 * @details TODO: put more info here
 *
 * @see TTAudioSignal
 *
 * @authors Tim Place
 *
 * @copyright Copyright © 2008 by Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_AUDIO_SIGNAL_ARRAY_H__
#define __TT_AUDIO_SIGNAL_ARRAY_H__

#include "TTAudioSignal.h"


/****************************************************************************************************/
// Class Specification

/**	A simple container for an array of TTAudioSignal pointers.
	This class does not automatically manage the signals themselves, instantiate, or free them.	*/
class TTDSP_EXPORT TTAudioSignalArray : public TTDataObjectBase {
	TTCLASS_SETUP(TTAudioSignalArray)

protected:

	TTAudioSignalPtr*	mAudioSignals;			///< The actual array of audio signal pointers.
	TTChannelCount		mAudioSignalMaxCount;	///< The maximum number of audio signals that can be passed in this array.

public:

	TTChannelCount		numAudioSignals;		///< The number of audio signal pointers which are actually valid.
	
	void chuck();
	void init();
	void releaseAll();

	void clearAll()
	{
		for (TTChannelCount i=0; i<mAudioSignalMaxCount; i++)
			mAudioSignals[i]->clear();
	}
	
	void		allocAllWithVectorSize(TTUInt16 vs);
	TTUInt16	getVectorSize();
	void		setAllMaxNumChannels(TTChannelCount newMaxNumChannels);
	void		setAllNumChannels(TTChannelCount newNumChannels);
	
	void setAllSampleRates(TTUInt32 newSampleRate)
	{
		for (TTChannelCount i=0; i<numAudioSignals; i++)
			mAudioSignals[i]->setSampleRate(newSampleRate);
	}
	
	/**	Note: calling this function will invalidate all audioSignal pointers contained within the array. */
	void setMaxNumAudioSignals(TTChannelCount newMaxNumAudioSignals)
	{
		if (newMaxNumAudioSignals != mAudioSignalMaxCount) {
			chuck();
			mAudioSignalMaxCount = newMaxNumAudioSignals;
			init();
		}
	}
	
	TTChannelCount getMaxNumAudioSignals()
	{
		return mAudioSignalMaxCount;
	}

	
	TTAudioSignal& getSignal(TTChannelCount index)
	{
		return *mAudioSignals[index];
	}
	

	TTErr setSignal(TTChannelCount index, const TTAudioSignalPtr aSignal);
	
	
	void matchNumChannels(TTAudioSignalArray* anotherArray)
	{
		matchNumChannels(*anotherArray);
	}

	void matchNumChannels(TTAudioSignalArray& anotherArray)
	{
		TTChannelCount	audioSignalCount =  TTClip<TTChannelCount>(mAudioSignalMaxCount, 0, anotherArray.mAudioSignalMaxCount);
		TTValue		v;
		
		for (int i=0; i<audioSignalCount; i++) {
			TTChannelCount numChannels = anotherArray.mAudioSignals[i]->getNumChannelsAsInt();

			v = numChannels;
			// TODO: for efficiency, we should only set the maxNumChannels if it is larger than the current so we aren't allocing memory on the heap!
			mAudioSignals[i]->setMaxNumChannels(v);
			mAudioSignals[i]->setNumChannels(v);
		}
		
		// TODO, for all channels that are not in this array, but are in another array, we should zero the numChannels
	}
	
	TTChannelCount getMaxNumChannels()
	{
		TTChannelCount maxNumChannels = 0;
		
		for (TTChannelCount i=0; i<mAudioSignalMaxCount; i++) {
			if (mAudioSignals[i]->getMaxNumChannelsAsInt() > maxNumChannels)
				maxNumChannels = mAudioSignals[i]->getMaxNumChannelsAsInt();
		}
		return maxNumChannels;
	}
	
};


typedef TTAudioSignalArray* TTAudioSignalArrayPtr;

#endif // __TT_AUDIO_SIGNAL_ARRAY_H__

