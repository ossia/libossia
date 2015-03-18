/* 
 * TTBlue Class for working with threads
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_THREAD_H__
#define __TT_THREAD_H__

#include "TTBase.h"
#include <thread>

/****************************************************************************************************/
// Class Specification
// Note, a good resource is here: https://computing.llnl.gov/tutorials/pthreads/#CreatingThreads

typedef void*(*TTThreadCallbackType)(void* arg);

// C++11-compliant TTThread
class TTThread
{
	std::thread thread;
	TTThreadCallbackType	callback;	///< method called in the new thread when it starts
	void*					argument;	///< argument passed to the callback

public:
	TTThread(TTThreadCallbackType aCallback, void* anArgument)
	{
		if(aCallback != nullptr)
			thread = std::thread(aCallback, anArgument);
	}

	/** Tells the calling thread to go dormant for an amount of time specified in milliseconds. */
	static void sleep(TTUInt32 millisecondsToSleep)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsToSleep));
	}
	
	/** Tells the calling thread to wait for this thread to terminate before moving on.*/
	void wait()
	{
		if(thread.joinable())
			thread.join();
	}
	
	/** This method is called in the new thread when the thread is created. */
	//void* doCallbackMethod();
	
	// make sure this is a friend so that it can access the private members of the other atom
	friend bool operator == (const TTThread& thread1, const TTThread& thread2)
	{
		return thread1.thread.get_id() == thread2.thread.get_id();
	}
};

typedef TTThread* TTThreadPtr;


#endif // __TT_THREAD_H__
