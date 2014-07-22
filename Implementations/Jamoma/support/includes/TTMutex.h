/* 
 * TTBlue Class that maintains a mutual exclusion lock
 * Copyright © 2008, Timothy Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_MUTEX_H__
#define __TT_MUTEX_H__

#include "TTBase.h"
#ifdef TT_PLATFORM_MAC
	#include <pthread.h>
#endif

/****************************************************************************************************/
// Class Specification

/**
	The TTMutex class maintains a mutual exclusion lock.
	See http://en.wikipedia.org/wiki/Mutex for more details.
*/
class TTFOUNDATION_EXPORT TTMutex {
private:
	#ifdef TT_PLATFORM_WIN
	CRITICAL_SECTION		pMutex;
	#else // TT_PLATFORM_MAC or TTPLATFORM_LINUX
	pthread_mutex_t			pMutex;
	#endif

public:
	TTMutex(bool isRecursive);
	virtual	~TTMutex();

	void lock();
	void unlock();
};

typedef TTMutex* TTMutexPtr;
typedef TTMutex& TTMutexRef;


/**	TTLock provides an exception-safe, scoped, mutex locking that cannot be left accidentally locked.
	It uses the #TTMutex class internally.
	
	The idea for this comes from Effective STL #12, and uses the 'Resource Allocation is Initialization'
	pattern popularized by Stroustrup.  http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization
*/
class TTFOUNDATION_EXPORT TTLock {
private:
	TTMutexPtr		mMutex;
	
public:
	TTLock(bool isRecursive)
	{
		mMutex = new TTMutex(isRecursive);
		mMutex->lock();
	}
	
	TTLock(TTMutexRef aMutex)
	{
		mMutex = &aMutex;
		// TODO: can we increment a reference count or something on the mutex?
		mMutex->lock();
	}
	
	virtual ~TTLock()
	{
		mMutex->unlock();
		delete mMutex;
	}
};


#endif // __TT_MUTEX_H__
