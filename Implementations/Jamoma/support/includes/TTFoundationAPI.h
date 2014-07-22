/*
 * Jamoma Foundation API
 * Copyright © 2008, Timothy Place
 *
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_FOUNDATION_API_H__
#define __TT_FOUNDATION_API_H__

// CORE
#include "TTFoundation.h"
#include "TTDataObjectBase.h"
#include "TTDictionary.h"
#include "TTEnvironment.h"
#include "TTPath.h"
#include "TTHash.h"
#include "TTInterpolate.h"
#include "TTLimits.h"
#include "TTList.h"
#include "TTMatrixBase.h"
#include "TTMatrixArray.h"
#include "TTMutex.h"
#include "TTObjectBase.h"
#include "TTQueue.h"
#include "TTSymbol.h"
#include "TTSymbolTable.h"
#include "TTSymbolCache.h"

#include "TTRegex.h"

#include "TTAddress.h"
#include "TTAddressTable.h"
#include "TTAddressCache.h"
#include "TTAddressItem.h"
#include "TTNode.h"
#include "TTNodeDirectory.h"

#include "TTUnitTest.h"
#include "TTValue.h"
#include "TTObject.h"
#include "TTObjectTemplates.h"
#include "TTMatrix.h"
#include "TTMatrixObject.h"

#ifdef TT_PLATFORM_WIN
#define TT_EXTENSION_EXPORT __declspec(dllexport)
#else
#define TT_EXTENSION_EXPORT __attribute__((visibility("default")))
#endif


/** A macro for setting up the class binding to the library in extension classes.
	@param strname A C-string that names the object as it should be listed in the environment. */
#define TT_CLASS_SETUP(strname, tags, className)\
	\
	extern "C" TT_EXTENSION_EXPORT TTObject* instantiate ## className (TTSymbol, TTValue& arguments); \
	\
	TTObject*  instantiate ## className (TTSymbol, TTValue& arguments) \
	{\
		return new className (arguments);\
	}\
	\
	extern "C" TT_EXTENSION_EXPORT TTErr loadTTExtension(void);\
	TTErr loadTTExtension(void)\
	{\
		TTFoundationInit();\
		TTClassRegister(strname, tags, & instantiate ## className);\
		return kTTErrNone;\
	}

#endif // __TT_FOUNDATION_API_H__
