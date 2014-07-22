/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief Jamoma Foundation class for representing values
 *
 * @details
 *
 * @authors Tim Place, Théo de la Hogue, Nathan Wolek, Julien Rabin, Nils Peters, Trond Lossius
 *
 * @copyright Copyright © 2008, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_ELEMENT_H__
#define __TT_ELEMENT_H__

#include "TTBase.h"
#include "TTString.h"
#include "TTLimits.h"
#include "TTSymbol.h"
#include "TTSymbolTable.h"
#include "TTObject.h"
#include "TTMatrix.h"

#ifndef DISABLE_NODELIB
#include "TTAddress.h"
#include "TTAddressTable.h"
#include "TTAddressCache.h"
#endif


class TTDictionary;


//#define USE_TTInt32				// to -- To easily change for TTInt32 instead of int in order to make test

// macro for converting from one type to another regardless of type
#define	TTELEMENT_CONVERT switch(mType) {\
			case kTypeInt8:\
				value = mValue.int8;\
				break;\
			case kTypeUInt8:\
				value = mValue.uint8;\
				break;\
			case kTypeInt16:\
				value = mValue.int16;\
				break;\
			case kTypeUInt16:\
				value = mValue.uint16;\
				break;\
			case kTypeFloat32:\
				value = mValue.float32;\
				break;\
			case kTypeFloat64:\
				value = mValue.float64;\
				break;\
			case kTypeInt32:\
				value = mValue.int32;\
				break;\
			case kTypeUInt32:\
				value = mValue.uint32;\
				break;\
			case kTypeInt64:\
				value = mValue.int64;\
				break;\
			case kTypeUInt64:\
				value = mValue.uint64;\
				break;\
			case kTypeBoolean:\
				value = mValue.boolean;\
				break;\
			default:\
				value = 0;\
				break;\
		}

#define	CONVERT(dType) switch(*(type+index)) {\
case kTypeInt8:\
value = (dType)(data+index)->int8;\
break;\
case kTypeUInt8:\
value = (dType)(data+index)->uint8;\
break;\
case kTypeInt16:\
value = (dType)(data+index)->int16;\
break;\
case kTypeUInt16:\
value = (dType)(data+index)->uint16;\
break;\
case kTypeFloat32:\
value = (dType)(data+index)->float32;\
break;\
case kTypeFloat64:\
value = (dType)(data+index)->float64;\
break;\
case kTypeInt32:\
value = (dType)(data+index)->int32;\
break;\
case kTypeUInt32:\
value = (dType)(data+index)->uint32;\
break;\
case kTypeInt64:\
value = (dType)(data+index)->int64;\
break;\
case kTypeUInt64:\
value = (dType)(data+index)->uint64;\
break;\
case kTypeBoolean:\
value = (dType)(data+index)->boolean;\
break;\
default:\
value = (dType)-1;\
break;\
}


/****************************************************************************************************/
// Class Specification

/** Individual items found in a #TTValue
 */
class TTFOUNDATION_EXPORT TT_ALIGN_16 TTElement {
	friend class TTDictionary;
	
	/** The data value of TTValue is stored using a union, which means that 
	    the size of TTDataValue is the size of the largest type in this list.
	 
	    For performance and interoperability we need to align each TTElement on 16-byte boundaries.
		Therefore we need to be careful to keep the size of TTDataValue + TTDataType to a maximum of 16-bytes.
	 
		For non-trivial data types we store pointers (8 bytes on 64-bit), but we do so by 
		creating a new instance and making a copy (as done in STL containers).
		That means also be careful in how we refcount and free these cases.
	 
		@see http://en.wikipedia.org/wiki/Data_structure_alignment
	 */
	union TTDataValue {
		TTFloat32		float32;
		TTFloat64		float64;
		TTInt8			int8;
		TTUInt8			uint8;
		TTInt16			int16;
		TTUInt16		uint16;
		TTInt32			int32;
		TTUInt32		uint32;
		TTInt64			int64;
		TTUInt64		uint64;
		TTBoolean		boolean;
		TTSymbol*		mSymbol;
		TTAddress*		mAddress;
		TTString*		stringPtr;
		TTObject*		mObject;
		TTMatrix*		mMatrix;
		TTPtr			ptr;
		TTSymbolBase*	dictionary;	///< dictionaries are referenced by name
	};
	
	TTDataValue		mValue;
	TTDataType		mType;

public:
	
	/** We use custom #new and #delete operators for TTElement to ensure that all memory
		allocated on the heap is aligned on 16-byte boundaries.
		
		For memory allocated on the stack we rely on the #TT_ALIGN_16 macro used in the class definition.
	 */
	void* operator new(size_t size)
	{
		void *mem = TTMalloc16(size);
		
		if (!mem)
            throw "allocation fail : no free memory";
		return mem;
	}
	

	/** We use custom #new and #delete operators for TTElement to ensure that all memory
		allocated on the heap is aligned on 16-byte boundaries.
		 
		For memory allocated on the stack we rely on the #TT_ALIGN_16 macro used in the class definition.
	 */
	void operator delete(void* mem)
	{
		TTFree16(mem);
	}
	
	
	TTElement() :
	mType(kTypeNone)
	{
		mValue.ptr = NULL;	// windows doesn't permit using an initializer for a union?
	}
	
	
	template<class T>
	TTElement(const T& anInitialValue) :
	mType(kTypeNone)
	{
		*this = anInitialValue;
	}
	
	/** Copy constructor. */
	TTElement(const TTElement& anOtherElement) :
	mType(kTypeNone)
	{
		*this = anOtherElement;
	}
	
	virtual ~TTElement();

private:
	/** Internal use only: Free memory of the item if it is a non-trivial type. */
	void chuck()
	{
		if (mType == kTypeSymbol)
			delete mValue.mSymbol;
        // TODO: JamomaCore #281 : review the use of TTAddress
		//else if (mType == kTypeAddress)
		//	delete mValue.mAddress;
		else if (mType == kTypeObject)
			delete mValue.mObject;
		mValue.ptr = NULL;
		mType = kTypeNone;
	}
	
public:
	/**	query an element for its type */
	TTDataType type() const
	{
		return mType;
	}
	
#if 0
#pragma mark -
#pragma mark casting
#endif
	
	operator TTFloat32() const
	{
		if (mType == kTypeFloat32)
			return mValue.float32;
		else {
			TTFloat32 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}

	operator TTFloat64() const
	{
		if (mType == kTypeFloat64)
			return mValue.float64;
		else {
			TTFloat64 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}

	operator TTInt8() const
	{
		if (mType == kTypeInt8)
			return mValue.int8;
		else {
			TTInt8 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}
	
	operator TTUInt8() const
	{
		if (mType == kTypeUInt8)
			return mValue.uint8;
		else {
			TTUInt8 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}

	operator TTInt16() const
	{
		if (mType == kTypeInt16)
			return mValue.int16;
		else {
			TTInt16 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}

	operator TTUInt16() const
	{
		if (mType == kTypeUInt16)
			return mValue.uint16;
		else {
			TTUInt16 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}

	operator TTInt32() const
	{
		if (mType == kTypeInt32)
			return mValue.int32;
		else {
			TTInt32 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}

	operator TTUInt32() const
	{
		if (mType == kTypeUInt32)
			return mValue.uint32;
		else {
			TTUInt32 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}
	
	operator TTInt64() const
	{
		if (mType == kTypeInt64)
			return mValue.int64;
		else {
			TTInt64 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}
	
	operator TTUInt64() const
	{
		if (mType == kTypeUInt64)
			return mValue.uint64;
		else {
			TTUInt64 value;
			TTELEMENT_CONVERT;
			return value;
		}
	}
	
	operator TTBoolean() const
	{
		if (mType == kTypeBoolean)
			return mValue.boolean;
		else {
			// setting as int and then casting after the macro (with the extra logic) is done to silence warnings on MSVC
			int value;
			TTELEMENT_CONVERT;
			return (TTBoolean)(value != 0);
		}
	}
		
	operator TTSymbol() const
	{
		if (mType == kTypeSymbol)
			return *mValue.mSymbol;
		else
			return kTTSymEmpty;
	}

#ifndef DISABLE_NODELIB
	operator TTAddress() const
	{
        // TODO: JamomaCore #281 : review the use of TTAddress
		//if (mType == kTypeAddress)
		//	return *mValue.mAddress;
		if (mType == kTypeSymbol)
			return TTAddress(*mValue.mSymbol);
        else
			return kTTAdrsEmpty;
	}
#endif
	
	operator TTString() const
	{
		TT_ASSERT(ttvalue_cast_to_string_ref, (mType == kTypeString));
		return *mValue.stringPtr;
	}

	// OBJECT
	operator TTObject() const
	{
		TT_ASSERT(ttvalue_cast_to_object, (mType == kTypeObject));
		return *mValue.mObject;
	}
		
	operator TTMatrix() const
	{
		TT_ASSERT(ttvalue_cast_to_matrix, (mType == kTypeMatrix));
		return *mValue.mMatrix;
	}
	
	operator TTPtr() const
	{
		if (mType == kTypePointer)
			return mValue.ptr;
		else
			return NULL;
	}
	
	operator TTDictionary() const;

	
#if 0
#pragma mark -
#pragma mark assignment
#endif
	
	TTElement& operator = (const TTElement& anOtherValue)
	{
		chuck();

		mType = anOtherValue.mType;
	
		if (anOtherValue.mType == kTypeSymbol)
			mValue.mSymbol = new TTSymbol(*anOtherValue.mValue.mSymbol);
        // TODO: JamomaCore #281 : review the use of TTAddress
		//else if (anOtherValue.mType == kTypeAddress)
		//	mValue.mAddress = new TTAddress(*anOtherValue.mValue.mAddress);
		else if (anOtherValue.mType == kTypeObject)
			mValue.mObject = new TTObject(*anOtherValue.mValue.mObject);
		else
			mValue = anOtherValue.mValue;
		
		return *this;
	}
	
	TTElement& operator = (TTFloat32 value)
	{
		chuck();
		mType = kTypeFloat32;
		mValue.float32 = value;
		return *this;
	}
		
	TTElement& operator = (TTFloat64 value)
	{
		chuck();
		mType = kTypeFloat64;
		mValue.float64 = value;
		return *this;
	}
	
	TTElement& operator = (TTInt8 value)
	{
		chuck();
		mType = kTypeInt8;
		mValue.int8 = value;
		return *this;
	}

	TTElement& operator = (TTUInt8 value)
	{
		chuck();
		mType = kTypeUInt8;
		mValue.uint8 = value;
		return *this;
	}

	TTElement& operator = (TTInt16 value)
	{
		chuck();
		mType = kTypeInt16;
		mValue.int16 = value;
		return *this;
	}

	TTElement& operator = (TTUInt16 value)
	{
		chuck();
		mType = kTypeUInt16;
		mValue.uint16 = value;
		return *this;
	}

	TTElement& operator = (TTInt32 value)
	{
		chuck();
		mType = kTypeInt32;
		mValue.int32 = value;
		return *this;
	}

	TTElement& operator = (TTUInt32 value)
	{
		chuck();
		mType = kTypeUInt32;
		mValue.uint32 = value;
		return *this;
	}

	TTElement& operator = (TTInt64 value)
	{
		chuck();
		mType = kTypeInt64;
		mValue.int64 = value;
		return *this;
	}

	TTElement& operator = (TTUInt64 value)
	{
		chuck();
		mType = kTypeUInt64;
		mValue.uint64 = value;
		return *this;
	}

	TTElement& operator = (TTBoolean value)
	{
		chuck();
		mType = kTypeBoolean;
		mValue.boolean = value;
		return *this;
	}
    
    TTElement& operator = (const char* value)
	{
		return *this = TTSymbol(value);
	}
	
	TTElement& operator = (const TTSymbol value)
	{
		chuck();
		mType = kTypeSymbol;
		mValue.mSymbol = new TTSymbol(value);
		return *this;
	}

#ifndef DISABLE_NODELIB
	TTElement& operator = (const TTAddress value)
	{
		chuck();
        // TODO: JamomaCore #281 : review the use of TTAddress
		//mType = kTypeAddress;
        mType = kTypeSymbol;
        // TODO: JamomaCore #281 : review the use of TTAddress
		//mValue.mAddress = new TTAddress(value);
        mValue.mSymbol = new TTAddress(value);
		return *this;
	}
#endif
	
	TTElement& operator = (const TTString value)
	{
		chuck();
		//		if (!stringsPresent && *type != kTypeString)
		//			data->stringPtr = new TTString;
		if (mType != kTypeString)
			mValue.stringPtr = new TTString;
		mType = kTypeString;
		//		stringsPresent = true;
		*mValue.stringPtr = value;
		return *this;
	}
	
	TTElement& operator = (const TTObject value)
	{
		chuck();
		mType = kTypeObject;
		mValue.mObject = new TTObject(value);
		return *this;
	}
		
	TTElement& operator = (const TTMatrix value)
	{
		chuck();
		mType = kTypeMatrix;
		mValue.mMatrix = new TTMatrix(value);
		return *this;
	}
	
	TTElement& operator = (TTPtr value)
	{
		chuck();
		mType = kTypePointer;
		mValue.ptr = value;
		return *this;
	}
	
	// TODO: an assignment to a different type (like the above) will leak the dictionary!
	
	TTElement& operator = (const TTDictionary value);

	
#if 0
#pragma mark -
#pragma mark conversion
#endif
	
#define TTELEMENT_TEMP_STRINGLEN 32
	
	void string(TTString& aString, TTBoolean quotes = YES) const
	{
		char		temp[TTELEMENT_TEMP_STRINGLEN];
		TTBoolean	addQuotes;
		

		temp[0] = 0;
		
		switch (mType) {
			case kTypeFloat32:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%f", mValue.float32);
				break;
			case kTypeFloat64:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%lf", mValue.float64);
				break;
			case kTypeInt8:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%i", mValue.int8);
				break;
			case kTypeUInt8:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%u", mValue.uint8);
				break;
			case kTypeInt16:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%i", mValue.int16);
				break;
			case kTypeUInt16:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%u", mValue.uint16);
				break;
			case kTypeInt32:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%i", (int)mValue.int32);
				break;
			case kTypeUInt32:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%iu", (unsigned int)mValue.uint32);
				break;
			case kTypeInt64:
#ifdef __LP64__ // Mac 64-Bit
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%ld", mValue.int64);
#else
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%lld", mValue.int64);
#endif
				break;
			case kTypeUInt64:
#ifdef __LP64__ // Mac 64-Bit
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%lu", mValue.int64);
#else
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "%llu", mValue.uint64);
#endif
				break;
			case kTypeBoolean:
				if (mValue.boolean)
					aString.append("1");
				else
					aString.append("0");
				break;
			case kTypeSymbol:
				addQuotes = quotes && strchr(mValue.mSymbol->c_str(), ' ') != 0;
				if (addQuotes)
					aString.append("\"");
				aString.append(mValue.mSymbol->c_str());
				if (addQuotes)
					aString.append("\"");
				break;
			case kTypeString:
				aString.append(*mValue.stringPtr);
				break;
			case kTypeObject:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "<object %p>", mValue.mObject);
				break;
			case kTypePointer:
				snprintf(temp, TTELEMENT_TEMP_STRINGLEN, "<pointer %p>", mValue.ptr);
				break;
			default:
				break;
		}
		
		if (temp[0])
			aString.append(temp);
	}

	
	
	// comparison overloads
	
	
	// make sure this is a friend so that it can access the private members of the other element
	friend bool operator == (const TTElement& a1, const TTElement& a2)
	{
		if (a1.mType != a2.mType)
			return false;
		else {
			switch (a1.mType) {
				case kTypeInt8:
					if ( a1.mValue.int8 != a2.mValue.int8 )
						return false;
					break;
				case kTypeUInt8:
					if ( a1.mValue.uint8 != a2.mValue.uint8 )
						return false;
					break;
				case kTypeInt16:
					if ( a1.mValue.int16 != a2.mValue.int16 )
						return false;
					break;
				case kTypeUInt16:
					if ( a1.mValue.uint16 != a2.mValue.uint16 )
						return false;
					break;
				case kTypeInt32:
					if ( a1.mValue.int32 != a2.mValue.int32 )
						return false;
					break;
				case kTypeUInt32:
					if ( a1.mValue.uint32 != a2.mValue.uint32 )
						return false;
					break;
				case kTypeInt64:
					if ( a1.mValue.int64 != a2.mValue.int64 )
						return false;
					break;
				case kTypeUInt64:
					if ( a1.mValue.uint64 != a2.mValue.uint64 )
						return false;
					break;
				case kTypeFloat32:
					if ( a1.mValue.float32 != a2.mValue.float32 )
						return false;
					break;
				case kTypeFloat64:
					if ( a1.mValue.float64 != a2.mValue.float64 )
						return false;
					break;
				case kTypeBoolean:
					if ( a1.mValue.boolean != a2.mValue.boolean )
						return false;
					break;
				case kTypeSymbol:
					if ( *a1.mValue.mSymbol != *a2.mValue.mSymbol )
						return false;
					break;
				case kTypeString:
					if ( *a1.mValue.stringPtr != *a2.mValue.stringPtr )
						return false;
					break;
				case kTypeObject:
					if ( *a1.mValue.mObject != *a2.mValue.mObject )
						return false;
					break;
				case kTypePointer:
					if ( a1.mValue.ptr != a2.mValue.ptr )
						return false;
					break;
				default: // the type is not currently handled
					return false;
			}
		}
		return true;
	}

	friend bool operator != (const TTElement& a1, const TTElement& a2)
	{
		return !(a1 == a2);
	}
	
	friend bool operator == (const TTElement& a1, const int& i)
	{
		switch (a1.mType) {
			case kTypeInt8:
				if ( a1.mValue.int8 != i )
					return false;
				break;
			case kTypeUInt8:
				if ( a1.mValue.uint8 != i )
					return false;
				break;
			case kTypeInt16:
				if ( a1.mValue.int16 != i )
					return false;
				break;
			case kTypeUInt16:
				if ( a1.mValue.uint16 != i )
					return false;
				break;
			case kTypeInt32:
				if ( a1.mValue.int32 != i )
					return false;
				break;
			case kTypeUInt32:
				if ( a1.mValue.uint32 != i )
					return false;
				break;
			case kTypeInt64:
				if ( a1.mValue.int64 != i )
					return false;
				break;
			case kTypeUInt64:
				if ( a1.mValue.uint64 != i )
					return false;
				break;
			default: // the type is not currently handled
				return false;
		}
		return true;
	}

	
	friend bool operator == (const TTElement& a1, const double& f)
	{
		switch (a1.mType) {
			case kTypeFloat64:
				if ( a1.mValue.float64 != f )
					return false;
				break;
			case kTypeFloat32:
				if ( a1.mValue.float32 != f )
					return false;
				break;
			default: // the type is not currently handled
				return false;
		}
		return true;
	}

	
	friend bool operator == (const TTElement& a1, const float& f)
	{
		switch (a1.mType) {
			case kTypeFloat32:
				if ( a1.mValue.float32 != f )
					return false;
				break;
			case kTypeFloat64:
				if ( a1.mValue.float64 != f )
					return false;
				break;
			default: // the type is not currently handled
				return false;
		}
		return true;
	}

	
	/**
	 We define the < operator for sorting of linked-list and other STL calls that require sorting ability of TTValue.
	 */
	
	// make sure this is a friend so that it can access the private members of the other atom
	friend bool operator < (const TTElement& a1, const TTElement& a2)
	{
		switch(a1.mType) {
			case kTypeInt8:
				if ( a1.mValue.int8 >= a2.mValue.int8 )
					return false;
				break;
			case kTypeUInt8:
				if ( a1.mValue.uint8 >= a2.mValue.uint8 )
					return false;
				break;
			case kTypeInt16:
				if ( a1.mValue.int16 >= a2.mValue.int16 )
					return false;
				break;
			case kTypeUInt16:
				if ( a1.mValue.uint16 >= a2.mValue.uint16 )
					return false;
				break;
			case kTypeInt32:
				if ( a1.mValue.int32 >= a2.mValue.int32 )
					return false;
				break;
			case kTypeUInt32:
				if ( a1.mValue.uint32 >= a2.mValue.uint32 )
					return false;
				break;
			case kTypeInt64:
				if ( a1.mValue.int64 >= a2.mValue.int64 )
					return false;
				break;
			case kTypeUInt64:
				if ( a1.mValue.uint64 >= a2.mValue.uint64 )
					return false;
				break;
			case kTypeFloat32:
				if ( a1.mValue.float32 >= a2.mValue.float32 )
					return false;
				break;
			case kTypeFloat64:
				if ( a1.mValue.float64 >= a2.mValue.float64 )
					return false;
				break;
			case kTypeBoolean:
				if ( a1.mValue.boolean >= a2.mValue.boolean )
					return false;
				break;
			case kTypeSymbol:
				if ( strcmp( a1.mValue.mSymbol->c_str(), a2.mValue.mSymbol->c_str() ) >= 0 )
					return false;
				break;
			case kTypeString:
				if ( strcmp( a1.mValue.stringPtr->c_str(), a2.mValue.stringPtr->c_str() ) >= 0 )
					return false;
				break;
			case kTypeObject: // TODO: how should we actually be sorting objects, if at all?
				if ( a1.mValue.mObject >= a2.mValue.mObject )
					return false;
				break;
			case kTypePointer:
				if ( a1.mValue.ptr >= a2.mValue.ptr )
					return false;
				break;
			default:
				return false;
		}
		return true;
	}

	
#if 0
#pragma mark -
#pragma mark transformation
#endif
	
	void clip(const TTFloat64& lowBound, const TTFloat64& highBound)
	{
		if (TTDataInfo::getIsNumerical(mType)) {
			// TODO: find a way to make this routine faster
			switch (mType) {
				case kTypeFloat32:
					TTLimit<TTFloat32>(mValue.float32, lowBound, highBound);
					break;
				case kTypeFloat64:
					TTLimit<TTFloat64>(mValue.float64, lowBound, highBound);
					break;
				case kTypeInt8:
					TTLimit<TTInt8>(mValue.int8, lowBound, highBound);
					break;
				case kTypeUInt8:
					TTLimit<TTUInt8>(mValue.uint8, lowBound, highBound);
					break;
				case kTypeInt16:
					TTLimit<TTInt16>(mValue.int16, lowBound, highBound);
					break;
				case kTypeUInt16:
					TTLimit<TTUInt16>(mValue.uint16, lowBound, highBound);
					break;
				case kTypeInt32:
					TTLimit<TTInt32>(mValue.int32, lowBound, highBound);
					break;
				case kTypeUInt32:
					TTLimit<TTUInt32>(mValue.uint32, lowBound, highBound);
					break;
				case kTypeInt64:
					TTLimit<TTInt64>(mValue.int64, lowBound, highBound);
					break;
				case kTypeUInt64:
					TTLimit<TTUInt64>(mValue.uint64, lowBound, highBound);
					break;
				default:
					break;
			}
		}
	}
	

	void cliplow(const TTFloat64& lowBound)
	{
		if (TTDataInfo::getIsNumerical(mType)) {
			// TODO: find a way to make this routine faster
			switch (mType) {
				case kTypeFloat32:
					TTLimitMin<TTFloat32>(mValue.float32, lowBound);
					break;
				case kTypeFloat64:
					TTLimitMin<TTFloat64>(mValue.float64, lowBound);
					break;
				case kTypeInt8:
					TTLimitMin<TTInt8>(mValue.int8, lowBound);
					break;
				case kTypeUInt8:
					TTLimitMin<TTUInt8>(mValue.uint8, lowBound);
					break;
				case kTypeInt16:
					TTLimitMin<TTInt16>(mValue.int16, lowBound);
					break;
				case kTypeUInt16:
					TTLimitMin<TTUInt16>(mValue.uint16, lowBound);
					break;
				case kTypeInt32:
					TTLimitMin<TTInt32>(mValue.int32, lowBound);
					break;
				case kTypeUInt32:
					TTLimitMin<TTUInt32>(mValue.uint32, lowBound);
					break;
				case kTypeInt64:
					TTLimitMin<TTInt64>(mValue.int64, lowBound);
					break;
				case kTypeUInt64:
					TTLimitMin<TTUInt64>(mValue.uint64, lowBound);
					break;
				default:
					break;
			}
		}
	}
	
	
	void cliphigh(const TTFloat64& highBound)
	{
		if (TTDataInfo::getIsNumerical(mType)) {
			// TODO: find a way to make this routine faster
			switch (mType) {
				case kTypeFloat32:
					TTLimitMax<TTFloat32>(mValue.float32, highBound);
					break;
				case kTypeFloat64:
					TTLimitMax<TTFloat64>(mValue.float64, highBound);
					break;
				case kTypeInt8:
					TTLimitMax<TTInt8>(mValue.int8, highBound);
					break;
				case kTypeUInt8:
					TTLimitMax<TTUInt8>(mValue.uint8, highBound);
					break;
				case kTypeInt16:
					TTLimitMax<TTInt16>(mValue.int16, highBound);
					break;
				case kTypeUInt16:
					TTLimitMax<TTUInt16>(mValue.uint16, highBound);
					break;
				case kTypeInt32:
					TTLimitMax<TTInt32>(mValue.int32, highBound);
					break;
				case kTypeUInt32:
					TTLimitMax<TTUInt32>(mValue.uint32, highBound);
					break;
				case kTypeInt64:
					TTLimitMax<TTInt64>(mValue.int64, highBound);
					break;
				case kTypeUInt64:
					TTLimitMax<TTUInt64>(mValue.uint64, highBound);
					break;
				default:
					break;
			}
		}
	}
	
	
	void round()
	{
		if (TTDataInfo::getIsNumerical(mType)) {
			// TODO: find a way to make this routine faster
			switch (mType) {
				case kTypeFloat32:
					mValue.float32 = (TTFloat32)TTRound(mValue.float32);
					break;
				case kTypeFloat64:
					mValue.float64 = TTRound(mValue.float64);
					break;
				case kTypeInt8:
					mValue.int8 = (TTInt8)TTRound(mValue.int8);
					break;
				case kTypeUInt8:
					mValue.uint8 = (TTUInt8)TTRound(mValue.uint8);
					break;
				case kTypeInt16:
					mValue.int16 = (TTInt16)TTRound(mValue.int16);
					break;
				case kTypeUInt16:
					mValue.uint16 = (TTUInt16)TTRound(mValue.uint16);
					break;
				case kTypeInt32:
					mValue.int32 = TTRound(mValue.int32);
					break;
				case kTypeUInt32:
					mValue.uint32 = TTRound(mValue.uint32);
					break;
				case kTypeInt64:
					mValue.int64 = TTRound(mValue.int64);
					break;
				case kTypeUInt64:
					mValue.uint64 = TTRound(mValue.uint64);
					break;
				default:
					break;
			}
		}
	}
	
	
	void truncate()
	{
		if (TTDataInfo::getIsNumerical(mType)) {
			// TODO: find a way to make this routine faster
			switch (mType) {
				case kTypeFloat32:
					mValue.int32 = (TTInt32)mValue.float32;
					mType = kTypeInt32;
					break;
				case kTypeFloat64:
					mValue.int32 = (TTInt32)mValue.float64;
					mType = kTypeInt32;
					break;
				default:
					break;
			}
		}
	}
	
	
	void booleanize()
	{
		if (TTDataInfo::getIsNumerical(mType)) {
			// TODO: find a way to make this routine faster
			switch (mType) {
				case kTypeFloat32:
					mValue.boolean = mValue.float32 != 0.;
					mType = kTypeBoolean;
					break;
				case kTypeFloat64:
					mValue.boolean = mValue.float64 != 0;
					mType = kTypeBoolean;
					break;
				case kTypeInt8:
					mValue.boolean = mValue.int8 != 0;
					mType = kTypeBoolean;
					break;
				case kTypeUInt8:
					mValue.boolean = mValue.uint8 != 0;
					mType = kTypeBoolean;
					break;
				case kTypeInt16:
					mValue.boolean = mValue.int16 != 0;
					mType = kTypeBoolean;
					break;
				case kTypeUInt16:
					mValue.boolean = mValue.uint16 != 0;
					mType = kTypeBoolean;
					break;
				case kTypeInt32:
					mValue.boolean = mValue.int32 != 0;
					mType = kTypeBoolean;
					break;
				case kTypeUInt32:
					mValue.boolean = mValue.uint32 != 0;
					mType = kTypeBoolean;
					break;
				case kTypeInt64:
					mValue.boolean = mValue.int64 != 0;
					mType = kTypeBoolean;
					break;
				case kTypeUInt64:
					mValue.boolean = mValue.uint64 != 0;
					mType = kTypeBoolean;
					break;
				default:
					break;
			}
		}
	}

};


typedef std::vector<TTElement>		TTElementVector;
typedef TTElementVector::iterator	TTElementIter;

	
#endif // __TT_ELEMENT_H__

