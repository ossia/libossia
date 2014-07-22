/*
 * Jamoma file to declare the NodeLib
 * Copyright © 2011, Théo de la Hogue
 *
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_NODELIB_H__
#define __TT_NODELIB_H__

#include "TTHash.h"
#include "TTAddress.h"
#include "TTAddressTable.h"
#include "TTAddressItem.h"
#include "TTAddressCache.h"

void TTFOUNDATION_EXPORT TTNodeLibInit();

/**	Convert a "upper/cased/name" symbol from "UpperCasedName" symbol
 @param	upperCasedName					"UpperCasedName" symbol
 @param	convertedInAddress				"upper/cased/name" address converted */
TTErr TTFOUNDATION_EXPORT	convertUpperCasedNameInAddress(TTSymbol upperCasedName, TTAddress& convertedInAddress);

#endif // __TT_NODELIB_H__

