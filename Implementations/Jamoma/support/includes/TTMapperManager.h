/** @file
 *
 * @ingroup modularLibrary
 *
 * @brief A MapperManager Object
 *
 * @details
 *
 * @authors Laurent Garnier
 *
 * @copyright © 2010, Laurent Garnier @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_MAPPER_MANAGER_H__
#define __TT_MAPPER_MANAGER_H__

#include "TTModularIncludes.h"

/**	For now the TTMapperManager allows to instanciate several TTMappers by reading a xml mapping
 *  configuration file using a TTXmlHandler
 *  TODO : writeAsText/readFromText
 */

class TTMapper;
typedef TTMapper* TTMapperPtr;

class TTXmlHandler;
typedef TTXmlHandler* TTXmlHandlerPtr;

class TTMODULAR_EXPORT TTMapperManager : public TTObjectBase
{
	TTCLASS_SETUP(TTMapperManager)
	
public:
	
	TTAddress	mAddress;						///< ATTRIBUTE : the address of the mapper manager in the directory

private:
	
	TTList		mMapperList;					///< a list containing <TTMapperPtr>
	
public:
	
	TTErr New();
	
private :
	
	/**  needed to be handled by a TTXmlHandler */
	TTErr WriteAsXml(const TTValue& inputValue, TTValue& outputValue);
	TTErr ReadFromXml(const TTValue& inputValue, TTValue& outputValue);

	/** A setter for the address attribute */
	TTErr setAddress(const TTValue& value);

};

typedef TTMapperManager* TTMapperManagerPtr;

#endif // __TT_MAPPER_MANAGER_H__
