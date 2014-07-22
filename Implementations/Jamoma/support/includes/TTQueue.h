/*
 * Jamoma Foundation
 * Copyright © 2011, Timothy Place
 *
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_QUEUE_H__
#define __TT_QUEUE_H__

#include "TTBase.h"
#include "TTThread.h"
#include "TTList.h"
#include "TTObjectBase.h"


#if OLD

/*
	The old TTQueue class (circa 2008) was unused, and was designed specificaly to be used only for getting things
	from an other thread onto the main thread.
 
	The new TTQueue class is more general, and is designed as a non-blocking circular buffer for getting
	events from thread A to thread B where the neither of the threads are required to be the main thread.
 
	A specific use of this queue to get things to the main thread can be created and would then use this
	newer version of the class.
*/



/****************************************************************************************************/
// Class Specification

/**
	The TTQueue class implements a message queue for TTBlue objects.
	
	Instead of sending a message directly to an object, it can be sent to the queue.  The queue
	will then send the message to the object from the queue's thread when the queue is serviced.
	
	The implementation uses a set of related lists; one for the object, one for the message, and
	one for the value argument to the message. 
*/
class TTFOUNDATION_EXPORT TTQueue : public TTBase {
private:
	TTThread*	queueThread;
	TTList*		queueEventObjects;
	TTList*		queueEventMethods;
	TTList*		queueEventValues;
	TTUInt16	throttle;			///< Maximum number of events to process in one shot

public:
	TTQueue();
	virtual	~TTQueue();

	/** This is them main loop that runs the queue */
	void* run();

	/** Add a call to the back of the queue.  Will trigger the queue to be serviced if it isn't
		already scheduled. */
	void queueToBack(TTObject& anObject, TTSymbol aMessage, TTValue& aValue);
	//void queueToFront(TTMethod method, TTValue& value);
};

#else 


/****************************************************************************************************/
// Class Specification


// The problem with this is that it won't be fast enough for audio because everything has to go 
// through the message passing interface.
// 
// But...
//
// The reader doesn't have to actually do that.
// It can just pull the value and then decide what to do with it.
// So maybe this is entirely overkill for the TTQueue, and is actually better off in the TTMainQueue
// implementation...
//
//class TTFOUNDATION_EXPORT TTQueueItem : public TTBase {
//	TTObjectPtr	mObject;	// the object to which to send the message
//	TTSymbolRef	mKind;		// 'attribute' or 'message'
//	TTSymbolRef mName;		// the name of the attribute or message
//	TTValue		mValue;		// the arguments to send to the attribute or message
//};


/**
	The TTQueue class implements a non-blocking circular queue for passing events across threads.
	The items in the queue are TTValues.
	What is contained in the TTValue will be specific to whatever is using the queue.
 
	This is based on "Efficient Adaptations of Non-Blocking Buffer for Event Message Communication
	between Real-Time Threads" by Kim, Colmenares, and Rim from the proceedings of the 10th IEEE
	International Symposium on Object and Component-Oriented Real-Time Distributed Computing.
 
	One important difference is that we copy data at both the insert() and the read(), rather than 
	referencing a pointer at insert() and copying only at read().
	The reason for this is that we could easily put something on the queue (i.e. a reference to an object)
	that is temporary (alloc'd on the stack, for example) and then disappears prior to being serviced.
 

	There are other implementations of a lock-free queue/FIFO that should be referenced:
 
 
	1. PortAudio (pa_ringbuffer.h/.c) does things a little bit differently, for example.
	Similar to what we have done, the memory for the items in the buffer is owned by the buffer.
	The counters are not implemented with OS-level atomic operations, but the reads and writes 
	into the buffer are done using OS-level memory barriers.
	Question: Because of our use of the atomic operations on the counters, do we still need to use memory barriers?
	Reference: http://www.rossbencina.com/code/lockfree

 
	2. SuperCollider is more like a hybrid of the PortAudio approach and the NBB implementation 
	on which this class is currently based.  The implementation can be found in common/Headers/server/MsgFifo.h.
	One cool thing is that it uses a template class in C++ so that the queue could hold any type of items, rather than
	restricted to a type or even a container like we do with TTValue.
 
	TODO: we should do this template thing like SC
	
	SC does not have the busy/retry ability that we have, but it also might be quicker because there is only one
	atomic increment at write instead of two.  
	Instead of using an atomic increment, however, it is using OSAtomicCompareAndSwap32Barrier -- apparently for legacy reasons.
 
	Additionally it does in enforce a memory barrier on the Mac by issuing __sync_synchronize() in the 
	equivalents of both insert() and read().
 
	Unlike NBB, based on a first reading of the code, it looks like it uses shared references rather than copying by value.
	It looks like this is true for *both* read (perform) and insert (write), which means that the heap usage is not
	kept isolated as argued to be the most important factor in the NBB paper.
 */
class TTFOUNDATION_EXPORT TTQueue {
private:
	// Counters used to ensure that the producer (calling insert()) and the consumer (calling read())
	// are always accessing different slots in the circular buffer

	// counters must be *aligned* single-word integers
	
	TTAtomicUInt	mUpdateCounter;				// UC -- only modified by the producer
	TTAtomicUInt	mAcknowledgementCounter;	// AC -- only modified by the consumer
	TTVector		mBuffer;					// Using a std::vector of TTValues right now -- not sure this is best choice?
	TTUInt32		mSize;						// count of slots in mBuffer -- should be power of 2, will use as a bitmask
	TTUInt32		mTwiceSize;					// 2 * mBufferSize, cached for performance
	
	enum InsertStatus {
		kBufferInsertSuccessful = 0,
		kBufferFull,
		kBufferFullButCurrentlyReading
	};
	
	enum ReadStatus {
		kBufferReadSuccessful = 0,
		kBufferEmpty,
		kBufferEmptyButCurrentlyWriting
	};
	
	
public:
	TTQueue();
	virtual	~TTQueue();

	/** attribute accessor */
	TTErr resize(TTUInt32 aNewSize);
	TTErr size(TTUInt32& returnedSize);
	
	/**	add an item to the queue.	*/
	InsertStatus insert(const TTValue& anItem);
	
	/**	get the next event and pop it from the queue.	*/
	ReadStatus read(TTValue& returnedItem);
	

};



#endif


#endif // __TT_QUEUE_H__
