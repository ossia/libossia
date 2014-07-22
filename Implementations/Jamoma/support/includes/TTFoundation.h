/*
 * Jamoma Foundation
 * Copyright © 2008, Timothy Place
 *
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_FOUNDATION_H__
#define __TT_FOUNDATION_H__

#define TTFOUNDATION_VERSION_STRING "0.6"

#include "TTBase.h"
#include "TTString.h"
#include "TTThread.h"
#include "TTFifo.h"
#include "TTSymbolCache.h"
#include "TTNodeLib.h"


/*
	If we could do virtual static methods then we would.  But we can't.
	So enforce the static register() and instantiate() methods we will use the macros to
	define our classes.

	This also enforces the protection of the constructor and destructor methods so that they
	cannot be manipulated directly.
 */

	/** TODO Doxygen: need more comments here	
		@ingroup macros
	*/
#define TT_OBJECT_CONSTRUCTOR \
	TTObjectBasePtr thisTTClass :: instantiate (TTSymbol name, const TTValue arguments) {return new thisTTClass (arguments);} \
	\
	extern "C" void thisTTClass :: registerClass () {TTClassRegister( thisTTClassName, thisTTClassTags, thisTTClass :: instantiate );} \
	\
	thisTTClass :: thisTTClass (const TTValue& arguments) : TTDataObjectBase(arguments)

	/** TODO Doxygen: need more comments here	
		@ingroup macros
	*/
#define TT_BASE_OBJECT_CONSTRUCTOR \
	TTObjectBasePtr thisTTClass :: instantiate (TTSymbol name, const TTValue arguments) {return new thisTTClass (arguments);} \
	\
	extern "C" void thisTTClass :: registerClass () {TTClassRegister( thisTTClassName, thisTTClassTags, thisTTClass :: instantiate );} \
	\
	thisTTClass :: thisTTClass (const TTValue& arguments) : TTObjectBase(arguments)

	/** TODO Doxygen: need more comments here	
		@ingroup macros
	*/
#define TTCLASS_SETUP(className)												\
		friend class TTEnvironment;												\
	public:																		\
		static void registerClass();											\
	protected:																	\
		static TTObjectBasePtr instantiate (TTSymbol name, const TTValue arguments);	\
		/** Constructor */														\
		className (const TTValue& arguments);											\
		/** Destructor */														\
		virtual ~className ();													\

// ON WINDOWS THERE CAN BE NO SPACE BETWEEN ~ and CLASSNAME like
//		virtual ~ className ();

extern	TTFOUNDATION_EXPORT	TTString TTFoundationBinaryPath;
void    TTFOUNDATION_EXPORT TTFoundationInit(const char* pathToTheJamomaFolder = NULL);


#endif // __TT_FOUNDATION_H__
