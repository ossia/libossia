/* 
 * Jamoma Class for accessing Addresses
 * Copyright © 2012, Timothy Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_ADDRESS_H__
#define __TT_ADDRESS_H__

#include "TTAddressBase.h"
#include "TTAddressTable.h"

class TTRegex;

extern TTFOUNDATION_EXPORT TTRegex* ttRegexForInstanceZero;	///< The global regex to parse instance .0

/****************************************************************************************************/
// Class Specifications

/**
	The TTAddress class is used to represent a string and efficiently pass and compare that string.
	It is essentially a TTSymbol extended with OSC parsing.
	An address follows the form:
		directory:/parent/name.instance:attribute
 */

class TTFOUNDATION_EXPORT TTAddress : public TTSymbol
{	
public:
	
	TTAddress()
	{	
		mSymbolPointer = gTTAddressTable.lookup("");
	}
	
	
	TTAddress(const char *cstr)
	{
        // check if there is a '0'
        const char* zero = strchr(cstr, C_ZERO);
        if (zero != 0) {
            
            // check if there is a '.' just before
            if (*(zero-1) == C_INSTANCE) {
                
                TTString sparsed;
                parseInstanceZero(cstr, sparsed);
                
                mSymbolPointer = gTTAddressTable.lookup(sparsed);
                
                return;
            }
        }
        
        mSymbolPointer = gTTAddressTable.lookup(cstr);
	}
	
	
	TTAddress(const int int_to_convert_to_string)
	{
		TTString s;
		
		s.append(int_to_convert_to_string);
		mSymbolPointer = gTTAddressTable.lookup(s);
	}
	
	
	TTAddress(TTPtr do_not_use_this_constructor_unless_you_absolutely_know_what_you_are_doing)
	{
        TTSymbolBase* base = (TTSymbolBase*)do_not_use_this_constructor_unless_you_absolutely_know_what_you_are_doing;
        
        // we have to check if this symbol base is part of the gTTAddressTable
        // (to not bind on a symbol base stored into gTTSymbolTable)
        if (base->getSymbolTableId() == TTPtrSizedInt(&gTTAddressTable))
            mSymbolPointer = base;
        else
            mSymbolPointer = ((TTAddressBase*)gTTAddressTable.lookup(base->getCString()));
	}
	
	
	/**	TTAddress constructor from directory, parent, name, instance and attribute part
	 @param	newDirectory					directory symbol
	 @param	newParent						parent address
	 @param	newName							name symbol
	 @param	newInstance						instance symbol
	 @param	newAttribute					attribute symbol */
	TTAddress(const TTSymbol newDirectory, const TTAddress newParent, const TTSymbol newName, const TTSymbol newInstance, const TTSymbol newAttribute)
	{
		mSymbolPointer = ((TTAddressBase*)gTTAddressTable.lookup(""))->edit(newDirectory, newParent.getBasePointer(), newName, newInstance, newAttribute);
	}
	
	
	//virtual ~TTAddress()
	//{;}
	
	
	TTAddressBase* getBasePointer() const
	{
		return static_cast<TTAddressBase*>(mSymbolPointer);
	}
	

	/** Get the directory part */
	TTSymbol& getDirectory()
	{
		return getBasePointer()->getDirectory();
	}
	
	/** Get a pointer to the parent address */
	TTAddress getParent()
	{
		return TTAddress(getBasePointer()->getParent());
	}
	
	/** Get the name part */
	TTSymbol& getName()
	{
		return getBasePointer()->getName();
	}
	
	/** Get the instance part */
	TTSymbol& getInstance()
	{
		return getBasePointer()->getInstance();
	}
	
	/** Get the attribute part */
	TTSymbol& getAttribute()
	{
		return getBasePointer()->getAttribute();
	}
	
	/** Get the type */
	TTAddressType getType()
	{
		return getBasePointer()->getType();
	}
	
	/** Get the name.instance part */
	TTSymbol getNameInstance()
	{
		return getBasePointer()->getNameInstance();
	}
	
	/** Normalize an address for lookup and other directory operations
	 This would return an address without directory and attribute	*/
	TTAddress normalize()
	{
		return TTAddress(getBasePointer()->normalize());
	}
	
	/** Return a new TTAddress without attribute part */
	TTAddress removeAttribute()
	{
		return TTAddress(getBasePointer()->removeAttribute());
	}
	
	/** Return a new TTAddress with attribute part */
	TTAddress appendAttribute(TTSymbol anAttribute)
	{
		return TTAddress(getBasePointer()->appendAttribute(anAttribute));
	}
	
	/** Return a new TTAddress with the appended part */
	TTAddress appendAddress(const TTAddress& toAppend)
	{
		return TTAddress(getBasePointer()->appendAddress( (static_cast<TTAddressBase*>(toAppend.mSymbolPointer)) ));
	}
	
	/** Return a new TTAddress with a instance part */
	TTAddress appendInstance(const TTSymbol anInstance)
	{
		return TTAddress(getBasePointer()->appendInstance(anInstance));
	}
	
	/**	A comparison tool
	 @param	toCompare					An address to compare (it doesn't compare attribute part)
	 @param	depthDifference				Return the number of level separating the compared addresses (> 0 for lower result, < 0 for upper result)
	 @return							An comparison flag */
	TTAddressComparisonFlag compare(const TTAddress& toCompare, TTInt8& depthDifference)
	{
		return getBasePointer()->compare((static_cast<TTAddressBase*>(toCompare.mSymbolPointer)), depthDifference);
	}
	
	/**	A parsing tool : split address in two part from a given '/' position
	 @param whereToSplit				An int to give the '/' id where to split
	 @param	returnedPart1				A pointer to upper part (before the given position) : /part1
	 @param	returnedPart2				A pointer to lower part (after the given position) : /part2
	 @return							kTTValueNotFound if the given '/' id doesn't exist */
	TTErr splitAt(TTUInt32 whereToSplit, TTAddress& returnedPart1, TTAddress& returnedPart2)
	{
		TTAddressBase* returnedPart1Ptr = NULL;
		TTAddressBase* returnedPart2Ptr = NULL;
		TTErr err;
		
		err = getBasePointer()->splitAt(whereToSplit, &returnedPart1Ptr, &returnedPart2Ptr);
		returnedPart1 = TTAddress(returnedPart1Ptr);
		returnedPart2 = TTAddress(returnedPart2Ptr);
		return err;
	}
	
	/**	A parsing tool : count how many C_SEPARATOR there is in the address
	 @return							The number of C_SEPARATOR */
	TTUInt32 countSeparator()
	{
		return getBasePointer()->countSeparator();
	}
	
	/**	A parsing tool : return a list containing all name.instance part (without any S_SEPARATOR)
	 @param								The list of name.instance part to fill */
	TTErr listNameInstance(TTList& nameInstanceList)
	{
        return getBasePointer()->listNameInstance(nameInstanceList);
	}
    
private:
	/** Parse ".0"
	 @return							An error code if the parsing failed */
	TTErr parseInstanceZero(const char* cstr, TTString& parsed);
    
};

#endif // __TT_ADDRESS_H__
