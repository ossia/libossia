/*
 * Jamoma Class that maintains a table of addresses
 * Copyright © 2011, Théo de la Hogue
 *
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_ADDRESS_TABLE_H__
#define __TT_ADDRESS_TABLE_H__

#include "TTBase.h"
#include "TTSymbolBase.h"
#include "TTSymbolTable.h"
#include "TTAddressBase.h"


/****************************************************************************************************/
// Class Specifications

/**
	The TTAddressTable class is a second symbol table, 
	but used only for addresses to keep lookups fast.
 */

class TTFOUNDATION_EXPORT TTAddressTable : public TTSymbolTable {
public:
	
	TTAddressTable() :
		TTSymbolTable(true)
	{;}
	
	/**	Override the method used internally when constructing a new TTSymbolBase */
	virtual TTSymbolBase* createEntry(const TTString& aString, TTInt32 newSymbolId);
	
};

extern TTFOUNDATION_EXPORT TTAddressTable	gTTAddressTable;		///< The global table of addresses


#endif // __TT_ADDRESS_TABLE_H__

