/*
 * TTFoundation First-in/First-out buffer
 * Copyright 2011 by Timothy Place
 *
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_FIFO_H__
#define __TT_FIFO_H__

#include "TTBase.h"
#include "TTLimits.h"


/** First-in/First-out buffer that is non-blocking and thread-safe for writing from a single thread 
	and reading from another (possibly different) single thread.
 
	@seealso	TTQueue
 */

template<class T>
class TTFifo {

protected:	
	
	typedef	T							TTFifoItem;
	typedef std::vector<TTFifoItem>		TTFifoItemVector;
	//typedef TTFifoItemVector::iterator	TTFifoItemIter;
	
	TTAtomicUInt		mUpdateCounter;				///< UC -- only modified by the producer
	TTAtomicUInt		mAcknowledgementCounter;	///< AC -- only modified by the consumer
	TTUInt32			mSlotCount;					///< count of slots in mBuffer -- should be power of 2, will use as a bitmask
	TTUInt32			mDoubleSlotCount;			///< 2 * mBufferSize, cached for performance
	TTFifoItemVector	mSlots;
	
public:
	
	TTFifo(int initialSlotCount) 
	{
		resize(initialSlotCount);
	}
	
	
	virtual ~TTFifo() 
	{
		
	}
	
	
	/** Change the number of slots in the circular queue.	 
		This method should be used with extreme caution as it is not thread-safe
		with regards to the push() and pop() methods.
	 
		@param	newSlotCount	Must be a power of two.  
								If the argument provided is not a power-of-two, 
								it will be increased to the next-higher power of two upon return.
	 */
	void resize(TTUInt32& newSlotCount)
	{
		TTLimitPowerOfTwo(newSlotCount);
		
		mSlotCount = newSlotCount;
		mDoubleSlotCount = newSlotCount*2;
		mSlots.resize(mDoubleSlotCount);
	}
	
	
	int size()
	{
		return mSlotCount;
	}
	
	
	enum TTFifoWriteStatus {
		kBufferWriteSuccessful = 0,
		kBufferFull,
		kBufferFullButCurrentlyReading
	};

	
	/**	Copy an item into the queue.	*/	
	TTFifoWriteStatus push(const T& item) 
	{
		TTUInt32	lastUpdateCounter = mUpdateCounter;
		TTUInt32	lastAcknowledgementCounter = mAcknowledgementCounter;	// this read should be atomic
		TTInt32		counterDifference = lastUpdateCounter - lastAcknowledgementCounter;
		TTInt32		index;
		TTInt32		twicesize = mDoubleSlotCount;
		TTInt32		size = mSlotCount;
	
		if (counterDifference == twicesize)
			return kBufferFull;
		if (counterDifference == twicesize-1)
			return kBufferFullButCurrentlyReading;
	
		TTAtomicIncrementWithBarrier(mUpdateCounter);	// now odd, indicating that we are in the middle of an insertion
	
		index = (lastUpdateCounter/2) & (size-1);		// fast modulo for power of 2
		mSlots[index] = item;							// copy
	
		TTAtomicIncrementWithBarrier(mUpdateCounter);	// now even, indicating that the insertion has completed
		return kBufferWriteSuccessful;
	}
	
	
	enum TTFifoReadStatus {
		kBufferReadSuccessful = 0,
		kBufferEmpty,
		kBufferEmptyButCurrentlyWriting
	};

	
	/**	Copy the next item and pop it from the queue. */
	TTFifoReadStatus pop(T& item)
	{
		TTUInt32	lastUpdateCounter = mUpdateCounter;						// this read should be atomic
		TTUInt32	lastAcknowledgementCounter = mAcknowledgementCounter;
		TTInt32		index;
		TTInt32		size = mSlotCount;
	
		if (lastUpdateCounter == lastAcknowledgementCounter)
			return kBufferEmpty;
		if (lastUpdateCounter - lastAcknowledgementCounter == 1)
			return kBufferEmptyButCurrentlyWriting;
	
		TTAtomicIncrementWithBarrier(mAcknowledgementCounter);		// now odd, indicating that we are in the middle of a read
	
		index = (lastAcknowledgementCounter/2) & (size-1);			// fast modulo for power of 2
		item = mSlots[index];										// copy
		
		TTAtomicIncrementWithBarrier(mAcknowledgementCounter);		// now even, indicating that the read has completed	
		return kBufferReadSuccessful;
	}
	
};


#endif // __TT_FIFO_H__

