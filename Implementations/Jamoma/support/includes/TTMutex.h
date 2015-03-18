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
#include <mutex>

#if defined(TT_PLATFORM_WIN)
bool TTFOUNDATION_EXPORT TTIsWindows8OrGreater();
#endif

class TTMutex{
// Ugly hack, one should do a make_mutex template that generates the correct type but this 
// involves changing every mutex creation in Jamoma
	std::mutex mutex;
	std::recursive_mutex rmutex;
	const bool recursive;
	
	public:
	TTMutex(bool isRecursive):
		recursive(isRecursive)
		{
		}
		
		void lock()
		{
#if defined(TT_PLATFORM_WIN)
            if(TTIsWindows8OrGreater())
#endif
                recursive ? rmutex.lock() : mutex.lock();
		}
		
		void unlock()
		{
#if defined(TT_PLATFORM_WIN)
            if(TTIsWindows8OrGreater())
#endif
                recursive ? rmutex.unlock() : mutex.unlock();
		}

};

typedef TTMutex* TTMutexPtr;
typedef TTMutex& TTMutexRef;

#endif // __TT_MUTEX_H__
