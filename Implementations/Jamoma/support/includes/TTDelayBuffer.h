/** @file
 *
 * @ingroup dspLibrary
 *
 * @brief Audio delay buffer object.
 * 
 * @authors Timothy Place
 *
 * @copyright Copyright © 2010, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_DELAYBUFFER_H__
#define __TT_DELAYBUFFER_H__

#include "TTDSP.h"


/**	TTDelayBuffer is a container object that holds some audio in a chunk of memory,
	with accessors for using it as a circular delay line.	
*/


// class TTDSP_EXPORT TTDelayBuffer {
// This class is designed to be inlined, so there is nothing to export
class TTDelayBuffer {

public: // NW: Should these be protected?

	TTSampleVector		mBuffer;
	TTSampleValuePtr	mWritePointer;		///< "record" pointer for buffer
	TTSampleValuePtr	mReadPointer;		///< "playback" pointer
	TTSampleValuePtr	mEndPointer;		///< points to last sample in buffer (for speed)	
	
public:
	
	TTDelayBuffer() : 
	mWritePointer(NULL),
	mReadPointer(NULL),
	mEndPointer(NULL)
	{
		;
	}
	
	TTUInt32 size()
	{
		return mBuffer.size();
	}
	
	void resize(TTUInt32 newSize)
	{
		mBuffer.resize(newSize);
		mReadPointer = mWritePointer = &mBuffer[0];
		mEndPointer = (&mBuffer[0]) + size();
	}
	
	void clear()
	{
		mBuffer.assign(size(), 0.0);
	}
	
	void setDelay(TTUInt32 delayInSamples)
	{
		mReadPointer = mWritePointer - delayInSamples;
		if (mReadPointer < (&mBuffer[0]))
			mReadPointer = mEndPointer + (mReadPointer - (&mBuffer[0])) + 1;
		else if (mReadPointer > mEndPointer)
			mReadPointer = (&mBuffer[0]) + (mReadPointer - mEndPointer);
	}
	
	TTSampleValuePtr head()
	{
		return &mBuffer[0];
	}
	
	TTSampleValuePtr tail()
	{
		return mEndPointer;
	}
	
	// check pointers (both directions) to ensure they are in-bounds
	TTSampleValuePtr wrapPointer(TTSampleValuePtr ptr)
	{
		if (ptr > tail())
			ptr = head() + (ptr - tail()) - 1;
		else if (ptr < head())
			ptr = tail() + (ptr - head()) + 1;
		return ptr;
	}
	
	// check pointers (only at the end) to ensure they are in-bounds
	TTSampleValuePtr wrapPointerForward(TTSampleValuePtr ptr)
	{
		if (ptr > tail())
			ptr = head() + (ptr - tail()) - 1;
		return ptr;
	}
	
	void wrap()
	{
		mReadPointer = wrapPointer(mReadPointer);
		mWritePointer = wrapPointer(mWritePointer);
	}
	
	void wrapForward()
	{
		mReadPointer = wrapPointerForward(mReadPointer);
		mWritePointer = wrapPointerForward(mWritePointer);
	}
	
};
typedef TTDelayBuffer*					TTDelayBufferPtr;
typedef std::vector<TTDelayBuffer>		TTDelayBufferVector;
typedef TTDelayBufferVector::iterator	TTDelayBufferIter;


#endif // __TT_DELAYBUFFER_H__
