/* 
 * TTBlue Class for caching common address for speed
 * Copyright © 2011, Théo de la Hogue
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#if !defined( __TT_NODE_ADDRESS_CACHE_H__ ) && !defined ( DISABLE_NODELIB )
#define __TT_NODE_ADDRESS_CACHE_H__

#include "TTAddress.h"
#include "TTAddressTable.h"

extern TTFOUNDATION_EXPORT TTAddressTable	gTTAddressTable;

extern TTFOUNDATION_EXPORT TTAddress		kTTAdrsEmpty;
extern TTFOUNDATION_EXPORT TTAddress		kTTAdrsRoot;
extern TTFOUNDATION_EXPORT TTAddress		kTTAdrsZero;

//void TTAddressCacheInit();

#endif // __TT_NODE_ADDRESS_CACHE_H__

