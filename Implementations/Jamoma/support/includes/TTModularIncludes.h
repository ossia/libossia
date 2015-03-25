/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief TTModular Library
 *
 * @details
 *
 * @authors Théo de la Hogue
 *
 * @copyright Copyright © 2010, Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_MODULAR_INCLUDES_H__
#define __TT_MODULAR_INCLUDES_H__

#define TTMODULAR_VERSION_STRING "0.1"
#define TTMODULAR_XML_ENCODING "UTF-16"

#ifdef TT_PLATFORM_WIN
#include "windows.h"
	#ifdef TTMODULAR_EXPORTS
		#define TTMODULAR_EXPORT __declspec(dllexport)
	#else
	#ifdef TTSTATIC
		#define TTMODULAR_EXPORT
	#else
		#define TTMODULAR_EXPORT __declspec(dllimport)
	#endif
	#endif // _DLL_EXPORT

#else // TT_PLATFORM_MAC
	#ifdef TTMODULAR_EXPORTS
		#define TTMODULAR_EXPORT __attribute__((visibility("default")))
	#else
		#define TTMODULAR_EXPORT
	#endif
#endif

#include <math.h>

#include "TTFoundationAPI.h"

#ifndef TT_NO_DSP
#include "TTDSP.h"
#endif

#include "TTNode.h"
#include "TTNodeDirectory.h"
#include "TTAddressItem.h"

#include "TTModular.h"

#include "TTRegex.h"

#include "TTProtocol.h"
#include "TTClock.h"

#include "TTApplication.h"
#include "TTApplicationManager.h"
#include "TTContainer.h"
#include "TTCue.h"
#include "TTCueManager.h"
#include "TTData.h"
#include "TTExplorer.h"
#include "TTMapper.h"
#include "TTMapperManager.h"
#include "TTMirror.h"
#include "TTNodeInfo.h"
#include "TTPreset.h"
#include "TTPresetManager.h"
#include "TTRamp.h"
#include "TTReceiver.h"
#include "TTSender.h"
#include "TTScript.h"
#include "TTSubscriber.h"
#include "TTTextHandler.h"
#include "TTViewer.h"
#include "TTXmlHandler.h"

#if 0
#pragma mark -
#pragma mark Macros for TTObject declaration
#endif

#define TT_MODULAR_CONSTRUCTOR \
TTObjectBasePtr thisTTClass :: instantiate (TTSymbol name, TTValue arguments) {return new thisTTClass (arguments);} \
\
extern "C" void thisTTClass :: registerClass () {TTClassRegister( TTSymbol(thisTTClassName), thisTTClassTags, thisTTClass :: instantiate );} \
\
thisTTClass :: thisTTClass (const TTValue& arguments) : TTObjectBase(arguments)



#define TT_MODULAR_CONSTRUCTOR_EXPORT \
	\
	extern "C" TT_EXTENSION_EXPORT TTErr loadTTExtension(void);\
	TTErr loadTTExtension(void)\
	{\
		TTModularInit();\
		thisTTClass :: registerClass(); \
		return kTTErrNone;\
	}\
	\
	TT_MODULAR_CONSTRUCTOR

#if 0
#pragma mark -
#pragma mark Macros to ease applications and protocols access
#endif

/** Access to an application by name */
#define accessApplication(applicationName) TTModularApplicationManager->findApplication(applicationName)

/** Access to local application */
#define accessApplicationLocal TTModularApplicationManager->getApplicationLocal()

/** Access to any application using an address */
#define accessApplicationFrom(anAddress) TTModularApplicationManager->findApplicationFrom(anAddress)

/** Access to an application directory using an address */
#define	accessApplicationDirectoryFrom(anAddress) TTModularApplicationManager->findApplicationDirectoryFrom(anAddress)

/** Access to an application directory by name */
#define accessApplicationDirectory(applicationName) TTModularApplicationManager->findApplicationDirectory(applicationName)

/** Access to the local application directory */
#define	accessApplicationLocalDirectory TTModularApplicationManager->getApplicationLocal()->getDirectory()

/** Access to the local application debug status */
#define accessApplicationLocalDebug TTModularApplicationManager->getApplicationLocal()->getDebug()

/** Access to a protocol by name */
#define accessProtocol(protocolName) TTModularApplicationManager->findProtocol(protocolName)

/** Access to all protocol names of an application */
#define accessApplicationProtocolNames(applicationName) TTModularApplicationManager->getApplicationProtocolNames(applicationName)

#if 0
#pragma mark -
#pragma mark Macros to ease conversion
#endif

/** Convert a TTValue with tt names inside into a value with local application names inside */
#define	ToAppNames(ttNames, appNames) \
    accessApplicationLocal->sendMessage(kTTSym_ConvertToAppName, ttNames, appNames); \

/** Convert a TTValue with local application names inside into a value with tt names inside */
#define	ToTTNames(appNames, ttNames) \
    accessApplicationLocal->sendMessage(kTTSym_ConvertToTTName, appNames, ttNames); \

/** Convert a local application TTSymbol into a tt name */
#define	ToAppName(ttName) \
    accessApplicationLocal->convertTTNameToAppName(ttName) \

/** Convert a tt name TTSymbol into a local application name */
#define	ToTTName(appName) \
    accessApplicationLocal->convertAppNameToTTName(appName) \


#endif // __TT_MODULAR_INCLUDES_H__
