/** @file
 *
 * @ingroup scoreLibrary
 *
 * @brief Add easily a specific time process or time container to the TimePluginLib
 *
 * @details The TimePluginLib allows to ... @n@n
 *
 * @see TTTimeProcess, TTTimeContainer
 *
 * @authors Théo de la Hogue & Clément Bossut
 *
 * @copyright Copyright © 2013, Théo de la Hogue & Clément Bossut @n
 * This code is licensed under the terms of the "CeCILL-C" @n
 * http://www.cecill.info
 */

#ifndef __TIME_PLUGIN_H__
#define __TIME_PLUGIN_H__

#include "TTScore.h"

#define TIME_PROCESS_PLUGIN_CONSTRUCTOR \
TTObjectBasePtr thisTTClass :: instantiate (TTSymbol name, TTValue arguments) {return new thisTTClass (arguments);} \
\
extern "C" void thisTTClass :: registerClass () {TTClassRegister( TTSymbol(thisTTClassName), thisTTClassTags, thisTTClass :: instantiate );} \
\
thisTTClass :: thisTTClass (const TTValue& arguments) : TimeProcessPlugin(arguments)

/**	The TimeProcessPlugin class allows to create specific time process plugin
 
 @see TimePluginLib, TTTimeProcess
 */
class TimeProcessPlugin : public TTTimeProcess {
    
    TTCLASS_SETUP(TimeProcessPlugin)
    
public:
	
	/** Get parameters names needed by this time process 
     @param	value           the returned parameter names
     @return                kTTErrNone */
	virtual TTErr   getParameterNames(TTValue& value) = 0;
};



#define TIME_CONTAINER_PLUGIN_CONSTRUCTOR \
TTObjectBasePtr thisTTClass :: instantiate (TTSymbol name, TTValue arguments) {return new thisTTClass (arguments);} \
\
extern "C" void thisTTClass :: registerClass () {TTClassRegister( TTSymbol(thisTTClassName), thisTTClassTags, thisTTClass :: instantiate );} \
\
thisTTClass :: thisTTClass (const TTValue& arguments) : TimeContainerPlugin(arguments)

/**	The TimeContainerPlugin class allows to create specific time container plugin
 
 @see TimePluginLib, TTTimeProcess
 */
class TimeContainerPlugin : public TTTimeContainer {
    
    TTCLASS_SETUP(TimeContainerPlugin)
    
public:
	
	/** Get parameters names needed by this time process
     @param	value           the returned parameter names
     @return                kTTErrNone */
	virtual TTErr   getParameterNames(TTValue& value) = 0;
};


#define TIME_PLUGIN_INITIALIZE \
registerAttribute(TTSymbol("ParameterNames"), kTypeLocalValue, NULL, (TTGetterMethod)& thisTTClass::getParameterNames); \
//addAttributeProperty(ParameterNames, readOnly, YES);



#endif // __TIME_PLUGIN_H__



#ifndef __TIME_PLUGIN_LIB_H__
#define __TIME_PLUGIN_LIB_H__

/**	The TimePluginLib class allows to ...
 
 @see TimeProcess
 */
class TT_EXTENSION_EXPORT TimePluginLib {
    
public:
    
	/**	Return a list of all available time plugins
     @param	value           the returned time plugins names*/
	static void getTimePluginNames(TTValue& timePluginNames);
};

#endif	//__TIME_PROCESS_LIB_H__
