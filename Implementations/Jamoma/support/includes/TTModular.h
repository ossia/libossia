/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief the Modular Application Programming Interface
 *
 * @details The Modular API allows to use Modular inside any application @n@n
 *
 * @see TTModularIncludes
 *
 * @authors Théo de la Hogue
 *
 * @copyright Copyright © 2013, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_MODULAR_H__
#define __TT_MODULAR_H__

#include "TTModularIncludes.h"

class TTApplicationManager;
typedef TTApplicationManager* TTApplicationManagerPtr;

#if 0
#pragma mark -
#pragma mark Initialisation
#endif

/** Initialize the Modular library and intanciate the TTModular object
 @param binaries                    path to the Jamoma libraries and extensions binaries folder to load them */
void TTMODULAR_EXPORT TTModularInit(const char* binaries = NULL);

#if 0
#pragma mark -
#pragma mark Application management
#endif

/** Export a pointer to a #TTApplicationManager instance
 @details this pointer is automatically filled when a #TTApplicationManager is instanciated */
extern TTMODULAR_EXPORT TTApplicationManagerPtr TTModularApplicationManager;

#if 0
#pragma mark -
#pragma mark Selection management
#endif

extern TTMODULAR_EXPORT TTHashPtr TTModularSelections;

/** Get a selection or create one if it doesn't exist yet 
 @param selectionName               a symbol */
TTAddressItemPtr TTMODULAR_EXPORT TTModularSelectionLookup(const TTSymbol selectionName);
    
#if 0
#pragma mark -
#pragma mark Addresses edition
#endif
    
/** Edit a specific integer instance address using an integer format address
 @param integerFormatAddress        a symbol as integer format address : /any/level/name.%d
 @param instanceNumber              an unsigned integer
 @return #TTAddress like /any/level/name.1 */
TTAddress TTMODULAR_EXPORT TTModularAddressEditNumericInstance(const TTSymbol integerFormatAddress,
                                                      const TTUInt32 instanceNumber);
    
/** Edit a specific symbol instance address using an symbol format address
 @param symbolFormatAddress         a symbol as symbol format address : /any/level/name.%s
 @param instanceSymbol              a symbol
 @return #TTAddress like /any/level/name.foo */
TTAddress TTMODULAR_EXPORT TTModularAddressEditSymbolInstance(const TTSymbol symbolFormatAddress,
                                                     const TTSymbol instanceSymbol);
    
/** Get all intances at an address 
 @param address                     an address without instance part :
                                        - /any/level/name
                                        - distantApp:/any/level/name 
 @param instances                   the returned instances symbols 
 @return #kTTErrGeneric if the address doesn't exist */
TTErr TTMODULAR_EXPORT TTModularAddressGetInstances(const TTAddress address,
                                           TTValue& instances);

#endif  // __TT_MODULAR_H__
