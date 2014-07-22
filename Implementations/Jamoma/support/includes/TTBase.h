/** @file
 *
 * @ingroup foundationLibrary 
 *
 * @brief Jamoma's lowest-level base class and related infrastructure.
 *
 * @details The Jamoma base class.
 *
 * @authors Timothy Place, Trond Lossius, Nils Peters
 *
 * @copyright Copyright © 2008, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_BASE_H__
#define __TT_BASE_H__

// Platform Sniffing
// Ideally the platform would already be set with a -D option to gcc...

#if defined( __APPLE__ )
// TARGET_OS_IPHONE is defined on the mac as 0, so use if, not ifdef below
	#include "TargetConditionals.h"

	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#ifndef TT_PLATFORM_IOS
			#define TT_PLATFORM_IOS
		#endif
	#else // TARGET_OS_MAC
		#ifndef TT_PLATFORM_MAC
			#define TT_PLATFORM_MAC
		#endif
	#endif
// win64 must come before win32
#elif defined ( _WIN64 )
#ifndef TT_PLATFORM_WIN
	#define TT_PLATFORM_WIN
#endif
#elif _WIN32
#ifndef TT_PLATFORM_WIN
	#define TT_PLATFORM_WIN
#endif
#elif __linux
	#define TT_PLATFORM_LINUX
#endif

#ifndef TT_PLATFORM_LINUX
	#ifndef TT_PLATFORM_IOS
		#ifndef TT_PLATFORM_WIN
			#ifdef WIN_VERSION
				#define TT_PLATFORM_WIN
			#else
				#ifndef TT_PLATFORM_MAC
					#define TT_PLATFORM_MAC
				#endif
			#endif
		#endif
	#endif
#endif

#include <algorithm>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <complex>
#include <sstream>
#include <iterator>
#include <stdexcept>

#ifdef TT_PLATFORM_LINUX
#include <stdarg.h>
#endif

#ifdef TT_PLATFORM_WIN
	#include "windows.h"
	#include <time.h>

	#pragma warning(disable:4244) // to avoid possible data lost warning with MSVC

	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif
	#define snprintf _snprintf
	#ifdef TTFOUNDATION_EXPORTS
		#define TTFOUNDATION_EXPORT __declspec(dllexport)
	#else
		#ifdef TTSTATIC
			#define TTFOUNDATION_EXPORT
		#else
			#define TTFOUNDATION_EXPORT __declspec(dllimport)
		#endif
	#endif // _DLL_EXPORT

#else // TT_PLATFORM_MAC
	#ifdef TTFOUNDATION_EXPORTS
		#define TTFOUNDATION_EXPORT __attribute__((visibility("default")))
	#else
		#define TTFOUNDATION_EXPORT
	#endif
#endif


#ifndef NO
#define NO 0
#endif

#ifndef YES
#define YES 1
#endif


#ifdef TT_DEBUG
#define TT_ENABLE_ASSERTIONS
#endif

#ifdef TT_ENABLE_ASSERTIONS
#define TT_ASSERT(name, result) \
				if (!result) {\
					char* nullPtr = 0;\
					TTLogError("%s:%ld ASSERTION %s FAILED\n", __FILE__, __LINE__, #name );\
					*nullPtr = 1;\
				}
#else
#define TT_ASSERT(name, result) ((void)(0));
#endif

/**	Supress warnings about use of deprecated methods.
	This macro should be defined at the top of a file before any includes in order to be made active.
	In general, the use of this macro should be viewed with suspicion.
	If it used a comment should accompany its use to explain the special situation that warrants the usage.
 */
#ifndef TT_NO_DEPRECATION_WARNINGS
#ifdef __GNUC__
#define TT_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define TT_DEPRECATED(func) __declspec(deprecated) func
#endif
#endif // !TT_NO_DEPRECATION_WARNINGS
#ifndef TT_DEPRECATED
#define TT_DEPRECATED(func) func
#endif


/****************************************************************************************************/
// Memory alignment

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define TT_ALIGN_16 __declspec(align(16))
#else // assuming gcc
#define TT_ALIGN_16 __attribute__((aligned (16)))
#endif


/****************************************************************************************************/
// Type Definitions

// Note http://developer.apple.com/mac/library/documentation/Darwin/Conceptual/64bitPorting/MakingCode64-BitClean/MakingCode64-BitClean.html#//apple_ref/doc/uid/TP40001064-CH226-SW2

typedef bool				TTBoolean;				///< Boolean flag, same as Boolean on the Mac
typedef unsigned char		TTByte;					///< Byte value
typedef TTByte*				TTBytePtr;				///< Data is a pointer to some bytes
typedef char*				TTCString;
typedef const char*			TTImmutableCString;

typedef signed char			TTInt8;					///< 8 bit signed integer (char)
typedef unsigned char		TTUInt8;				///< 8 bit unsigned integer (char)
typedef signed short		TTInt16;				///< 16 bit signed integer
typedef unsigned short		TTUInt16;				///< 16 bit unsigned integer

#ifdef USE_PROBLEMATIC_FOR_CPP_DUE_TO_AMBIGUITY_INT_TYPE
#ifdef __LP64__		// Mac 64-bit
	typedef signed int			TTInt32;			///< 32 bit signed integer
	typedef unsigned int		TTUInt32;			///< 32 bit unsigned integer
#else				// Mac 32-bit, Win32 32-bit
	typedef signed long			TTInt32;			///< 32 bit signed integer
	typedef unsigned long		TTUInt32;			///< 32 bit unsigned integer
#endif
#else
#define TTInt32 int
#ifdef __LP64__		// Mac 64-bit
typedef unsigned int		TTUInt32;			///< 32 bit unsigned integer
#else				// Mac 32-bit, Win32 32-bit
typedef unsigned long		TTUInt32;			///< 32 bit unsigned integer
#endif

#endif

#if defined(_MSC_VER) || defined(__BORLANDC__)
	typedef __int64				TTInt64;			///< 64 bit signed integer
	typedef unsigned __int64	TTUInt64;			///< 64 bit unsigned integer
#else
	#if defined(__LP64__)	// Mac 64-bit
		typedef signed long			TTInt64;		///< 64 bit unsigned integer
		typedef unsigned long		TTUInt64;		///< 64 bit unsigned integer
	#else // Max 32-bit
		typedef signed long long	TTInt64;		///< 64 bit unsigned integer
		typedef unsigned long long	TTUInt64;		///< 64 bit unsigned integer
	#endif
#endif

// can't do the follow -- conflicts on the mac with Carbon headers
//#ifndef uint
//#define uint unsigned int
//#endif

typedef float					TTFloat32;			///< 32 bit floating point number
typedef double					TTFloat64;			///< 64 bit floating point number
typedef std::complex<double>	TTComplex;			///< Conmplex number


/**	@typedef TTRowID
	Datatype for any number used to indicate a row index within a matrix.
	Three typedefs ( #TTRowID, #TTColumnID & #TTElementID ) are used so that we can easily distinguish between these important matrix attributes, 
	have consistent datatypes throughout #TTMatrixBase and quickly change those datatypes should the need arise in the future.
 
	Although these values should always be positive, we have intentionally avoided unsigned numbers because of boundary checking considerations in the TTMatrixBase::makeInBounds() method.
	Negative, signed integers have the potential to become very large numbers when casting to an unsigned integers. 
	This can cause errors during a boundary check, such as values clipping to the high boundary instead of the low boundary or numerous iterations of loop to bring a wrapped value back into the acceptable range.
 
	They can potentially be used to override functions that take the numbers in either order. 
	For example, linear algebra-related matrices will likely access elements in TTRowID, TTColumnID order. 
	However, video processing objects will likely access elements in TTColumnID, TTRowID order.
 
	@ingroup typedefs
 */
typedef TTInt32 TTRowID;


/**	@typedef TTColumnID
	Datatype for any number used to indicate a column index within the matrix.
 
	@ingroup typedefs
	@see TTRowID
 */
typedef TTInt32 TTColumnID;


/**	@typedef TTElementID
 @brief Datatype for any number used to indicate an element index within the matrix.
 @see TTRowID
 @ingroup typedefs
 */
typedef TTInt16 TTElementID;
// TODO: should there be a similar typedef for results of math operations that combine these values, i.e. TTIndexMathType?



/** A value representing a single audio sample.  TTSampleValue should be used any place a sample value is what the value represents.  This will enable us to change the type in the future if needed.  For example, to use 64-bit floats. */
typedef TTFloat64			TTSampleValue;

/** A TTSampleVector is simply a pointer to the first of an array of TTSampleValues. */
typedef std::vector<TTSampleValue>	TTSampleVector;
typedef TTSampleVector::iterator	TTSampleIter;

typedef TTSampleValue*				TTSampleValuePtr;
typedef TTSampleVector*				TTSampleVectorPtr;

/** An integer that is the same size as a pointer.	*/
typedef long				TTPtrSizedInt;				// this works for both 32 and 64 bit code on the Mac

/** An integer that can be used for atomic operations. TODO: consider if other type should be used here	*/
#ifdef TT_PLATFORM_WIN
typedef TT_ALIGN_16 volatile int			TTAtomicInt;
typedef TT_ALIGN_16 volatile unsigned int	TTAtomicUInt;
#else
typedef TT_ALIGN_16 volatile int32_t		TTAtomicInt;
typedef TT_ALIGN_16 volatile u_int32_t		TTAtomicUInt;
#endif

/** A generic pointer. */
typedef void*				TTPtr;
typedef TTPtr*				TTHandle;

/**	A simple/generic function pointer with no args.	*/
typedef void (*TTFunctionPtr)();

/**	A simple/generic function pointer with one arg.	*/
typedef void (*TTFunctionWithArgPtr)(TTPtr);

class TTValue;
typedef std::vector<TTValue>	TTVector;
typedef TTVector::iterator		TTVectorIter;

/** Use for finding stuff in a list or a hash or a dictionary. */
typedef void (*TTFunctionMatch)(const TTValue& valueToCheck, TTPtr baton, TTBoolean& found);


/**	\ingroup enums
 TTBlue Data Types
 Enumeration of data types used through out TTBlue, including the TTValue class and declaring the types of
 TTAttribute objects.																			*/
enum TTDataType {
	kTypeNone = 0,
	kTypeFloat32,		///< 32-bit floating point
	kTypeFloat64,		///< 64-bit floating point
	kTypeInt8,			///< 8-bit signed integer, range is -128 through 127.
	kTypeUInt8,			///< 8-bit unsigned integer, range is 0 through 255.
	kTypeInt16,			///< 16-bit signed integer, range is −32,768 through 32,767.
	kTypeUInt16,		///< 16-bit unsigned integer, range is 0 through 65,535.
	kTypeInt32,			///< 32-bit signed integer, range is -2,147,483,648 through 2,147,483,647.
	kTypeUInt32,		///< 32-bit unsigned integer, range is 0 through 4,294,967,295.
	kTypeInt64,			///< 64-bit signed integer, ragne is −9,223,372,036,854,775,808 through 9,223,372,036,854,775,807
	kTypeUInt64,		///< 64-bit unsigned integer, range is 0 through 18,446,744,073,709,551,615.
	kTypeBoolean,		///< Boolean (1/0) or (true/false) flag.
	kTypeSymbol,		///< Symbol type
	kTypeObject,		///< Object type
	kTypePointer,		///< Pointer type
	kTypeString,		///< String type
	kTypeLocalValue,	///< This is a special type used by TTAttribute to indicate that a value is a TTValue and is locally maintained.
	kTypeMatrix,		///< An instance of a TTMatrix object
	kTypeDictionary,	///< Dictionary type
    // TODO: JamomaCore #281 : review the use of TTAddress
	//kTypeAddress,
	kNumTTDataTypes
};


class TTSymbol;		// forward declaration
class TTDataInfo;
typedef TTDataInfo* TTDataInfoPtr;

/**	An array, indexed by values from TTDataType, containing information about those data types.	*/
extern TTFOUNDATION_EXPORT TTDataInfoPtr	ttDataTypeInfo[kNumTTDataTypes];


// from TTSymbolCache.h:
extern TTFOUNDATION_EXPORT TTSymbol	kTTSymEmpty;

class TTFOUNDATION_EXPORT TTDataInfo {
public:
	TTSymbol*	name;			///< Pointer to the name of type in global symbol table, e.g. float32, float64, etc.
	TTBoolean	isNumerical;	///< Is this type numeric?
	TTInt8		bitdepth;		///< Negative numbers indicate dynamic or unknown bitdepth.

	TTDataInfo() :
		name(NULL)
	{;}
	
	static TTDataInfoPtr getInfoForType(TTDataType type)
	{
		return ttDataTypeInfo[type];
	}
	
	static TTDataInfoPtr getInfoForType(TTSymbol& typeAsSymbol)
	{		
		TTDataType type = matchSymbolToDataType(typeAsSymbol);
		return getInfoForType(type);
	}

	static TTBoolean getIsNumerical(TTDataType type)
	{
		return ttDataTypeInfo[type]->isNumerical;
	}

	static void addDataInfoForType(TTDataType type);
	
	static TTDataType matchSymbolToDataType(TTSymbol& typeAsSymbol);

};

/**	\ingroup enums
 Jamoma Error Codes
 Enumeration of error codes that might be returned by any of the TTBlue functions and methods.	*/
enum TTErr {
	kTTErrNone = 0,			///< No Error.
	kTTErrGeneric,			///< Something went wrong, but what exactly is not known.  Typically used for context-specific problems.
	kTTErrAllocFailed,		///< Couldn't get memory.
	kTTErrFreeFailed,		///< Couldn't free memory.
	kTTErrInvalidType,		///< Bad DataType for the context.
	kTTErrInvalidAttribute,	///< Bad Attribute specified.
	kTTErrInvalidValue,		///< An inappropriate value was specified for an attribute or variable.
	kTTErrWrongNumValues,	///< The wrong number of values were passed to a method or attribute.
	kTTErrMethodNotFound,	///< Method not found.  Typically returned by the TTObject::sendMessage() function.
	kTTErrValueNotFound,	///< A value was not found when doing a look up for it (in a TTHash, TTList, or other class).
	kTTErrBadChannelConfig,	///< An invalid number of audio channels for a given context was encountered.
	kTTErrReadOnly,			///< Attempted a write to a read-only entity.
	kTTErrOutOfBounds,		///< Attempted to access memory outside a matrix or array (in a TTMatrix & TTSampleMatrix).
	kTTErrInstantiateFailed,///< Couldn't instantiate the Jamoma object requested.
	kTTErrInvalidFilepath 	///< Couldn't resolve the filepath as submitted.
};

/**	A simple/generic function pointer with a baton as TTValueRef and the value to send back as a TTValueRef.	*/
typedef TTErr (*TTFunctionWithBatonAndValue)(const TTValue&, const TTValue&);

/****************************************************************************************************/
// Class Specifications


/**	A TTBlue exception is thown with this object. */
class TTException {
	TTImmutableCString	reason;
public:
	TTException(TTImmutableCString aReason)
	: reason(aReason)
	{}
	
	const char* getReason()
	{
		return reason;
	}
};


#ifdef TT_PLATFORM_MAC
	#include <libkern/OSAtomic.h>
#elif defined (TT_PLATFORM_WIN)
	#pragma intrinsic (_InterlockedIncrement)
	#pragma intrinsic (_InterlockedDecrement)
	#pragma intrinsic (_InterlockedCompareExchange)
#endif

	
inline void TTAtomicIncrement(TTAtomicInt& value)
{
#ifdef TT_PLATFORM_MAC
	OSAtomicIncrement32(&value);
#elif defined (TT_PLATFORM_WIN)
	_InterlockedIncrement((volatile long*)&value);
#else // what should we do for thread safety on Linux and iOS?
	value++;
#endif
}


inline void TTAtomicIncrement(TTAtomicUInt& value)
{
#ifdef TT_PLATFORM_MAC
	OSAtomicIncrement32((int32_t*)&value);
#elif defined (TT_PLATFORM_WIN)
	_InterlockedIncrement((volatile long*)&value);
#else // what should we do for thread safety on Linux and iOS?
	value++;
#endif
}


inline void TTAtomicDecrement(TTAtomicInt& value)
{
#ifdef TT_PLATFORM_MAC
	OSAtomicDecrement32(&value);
#elif defined (TT_PLATFORM_WIN)
	_InterlockedDecrement((volatile long*)&value);
#else // what should we do for thread safety on Linux and iOS?
	value--;
#endif
}


inline void TTAtomicDecrement(TTAtomicUInt& value)
{
#ifdef TT_PLATFORM_MAC
	OSAtomicDecrement32((int32_t*)&value);
#elif defined (TT_PLATFORM_WIN)
	_InterlockedDecrement((volatile long*)&value);
#else // what should we do for thread safety on Linux and iOS?
	value--;
#endif
}

	
inline void TTAtomicIncrementWithBarrier(TTAtomicUInt& value)
{
#ifdef TT_PLATFORM_MAC
	OSAtomicIncrement32Barrier((int32_t*)&value);
#elif defined (TT_PLATFORM_WIN)
	_InterlockedIncrement((volatile long*)&value); // on windows there is always a barrier
#else // what should we do for thread safety on Linux and iOS?
	value++;
#endif
}


inline void TTAtomicDecrementWithBarrier(TTAtomicUInt& value)
{
#ifdef TT_PLATFORM_MAC
	OSAtomicDecrement32Barrier((int32_t*)&value);
#elif defined (TT_PLATFORM_WIN)
	_InterlockedDecrement((volatile long*)&value); // on windows there is always a barrier
#else // what should we do for thread safety on Linux and iOS?
	value++;
#endif
}

	
inline void TTAtomicAssign(TTAtomicInt& value, const TTAtomicInt& newValue, const TTAtomicInt& oldValue)
{
#ifdef TT_PLATFORM_MAC
	OSAtomicCompareAndSwap32(oldValue, newValue, &value);
#elif defined (TT_PLATFORM_WIN)
	_InterlockedCompareExchange((volatile long*)&value, (long) newValue, (long) oldValue);
#else // what should we do for thread safety on Linux and iOS?
	value = newValue;
#endif
}

	
	/**	Return the current system time in milliseconds.
		Although it is a global kind of function, we include it as a method of TTBase
		so that it can be defined in the header file and then inlined in other libraries.	*/
inline TTFloat64 TTGetTimeInMilliseconds()
{
	// On the Mac, CLOCKS_PER_SEC is 1000000, so we optimize
#if	CLOCKS_PER_SEC == 1000000
	return clock() / 1000.0;	
#else
	return (clock() * 1000.0) / CLOCKS_PER_SEC;
#endif
}

	
	/**	Return the current system time in microseconds.	*/
inline TTFloat64 TTGetTimeInMicroseconds()
{
	// On the Mac, CLOCKS_PER_SEC is 1000000, so we optimize
#if	CLOCKS_PER_SEC == 1000000
	return clock();	
#else
	return (clock() * 1000000.0) / CLOCKS_PER_SEC;
#endif
}
	

/**	Produces a random-valued 64-bit floating-point number in the range [0.0, 1.0]	*/
TTFOUNDATION_EXPORT TTFloat64 TTRandom64();


/** \ingroup consts
 Equal Power lookup table, 512 elements 
 */
TTFOUNDATION_EXPORT extern const TTFloat32 kTTLookupEqualPower[];

/** \ingroup consts
 Square Root lookup table, 512 elements
 */
TTFOUNDATION_EXPORT extern const TTFloat32 kTTLookupSquareRoot[];

/** \ingroup consts
 Equal Power lookup table with 0.701 at element 256
 */
TTFOUNDATION_EXPORT extern const TTFloat32 kTTLookupEqualPowerSymetric[];

/** \ingroup consts
 Square Root lookup table with 0.701 at element 256
 */
TTFOUNDATION_EXPORT extern const TTFloat32 kTTLookupSquareRootSymetric[];

/** \ingroup consts
 256 point window table (the first half of it)
 */
TTFOUNDATION_EXPORT extern const TTFloat32 kTTLookupHalfPaddedwWelch[];

/** \ingroup consts
 Quarter Sine lookup table
 */
TTFOUNDATION_EXPORT extern const TTFloat32 kTTLookupQuarterSine[];

/** \ingroup consts
 Pre-calculated value of pi     (3.1416).
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTPi;

/** \ingroup consts
 Pre-calculated value of pi/2.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTHalfPi;

/** \ingroup consts
 Pre-calculated value of pi/4.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTQuarterPi;

/** \ingroup consts
 Pre-calculated value of pi * 2.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTTwoPi;

/** \ingroup consts
 Constant used by the ttantidenormal function.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTAntiDenormalValue;

/** \ingroup consts
 Pre-calculated square-root of 2 (1.4142).
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTSqrt2;

/** \ingroup consts
 Pre-calculated value of sqrt(2)/2 (0.701).
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTHalfSqrt2;

/** \ingroup consts
 Pre-calculated value of 2 * sqrt(2) (2.8284).
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTTwoSqrt2;

/** \ingroup consts
 A very very small value, used for float equality comaprisments.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTEpsilon;

/** \ingroup consts
 Factor constant for converting radians to degrees.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTRadiansToDegrees;

/** \ingroup consts
 Factor constant for converting degrees to radians.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTDegreesToRadians;

/** \ingroup consts
 Power constant used when calculating MID gain.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTGainMidiPower;

/** \ingroup consts
 Invverse power constant used when calculating MID gain.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTGainMidiPowerInv;

/** \ingroup consts
 Constant for color representation when converting from char8 to float representation.
 */
TTFOUNDATION_EXPORT extern const TTFloat64 kTTInv255;

/** Platform and host independent method for posting log messages. 
 @param message		The message to post.
 */
void TTFOUNDATION_EXPORT TTLogMessage(TTImmutableCString message, ...);

/** Platform and host independent method for posting warnings. 
 @param message		The message to post.
 */
void TTFOUNDATION_EXPORT TTLogWarning(TTImmutableCString message, ...);

/** Platform and host independent method for posting errors. 
 @param message		The message to post.
 */
void TTFOUNDATION_EXPORT TTLogError(TTImmutableCString message, ...);

/** Platform and host independent method for posting messages only when debugging is enabled in the environment. 
 @param message		The message to post.
 */
void TTFOUNDATION_EXPORT TTLogDebug(TTImmutableCString message, ...);




/**	Allocate memory from the heap aligned to 16-byte boundaries.
	This memory MUST be freed using TTFree16().
 
	@see #TT_ALIGN_16
	@see http://stackoverflow.com/questions/1919183/how-to-allocate-and-free-aligned-memory-in-c
	@see http://bytes.com/topic/c/answers/591742-overload-new-delete-memalign-malloc
*/
TTPtr TTFOUNDATION_EXPORT TTMalloc16(size_t numBytes);


/** Free memory allocated using TTMalloc16(). */
void TTFOUNDATION_EXPORT TTFree16(TTPtr ptr);


#endif // __TT_BASE_H__
