/** @file
	
	@ingroup foundationLibrary
	
	@brief Implementation of template code exported by TTObject
 
	@author Timothy Place
	
	@copyright Copyright © 2014, Timothy Place @n
	This code is licensed under the terms of the "New BSD License" @n
	http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_OBJECT_TEMPLATES_H__
#define __TT_OBJECT_TEMPLATES_H__

#include "TTObject.h"
#include "TTObjectBase.h"


template <class T>
TTErr TTObject::set(const TTSymbol aName, T aValue)
{
	return mObjectInstance->setAttributeValue(aName, aValue);
}


template <class T>
TTErr TTObject::get(const TTSymbol aName, T& aReturnedValue) const
{
	return mObjectInstance->getAttributeValue(aName, aReturnedValue);
}

#include "TTMatrixBase.h"

template<typename T>
TTErr TTMatrix::get2d(TTRowID i, TTColumnID j, T& data) const
{
	return instance()->get2d(i, j, data);
}


template<typename T>
TTErr TTMatrix::get2d(TTRowID i, TTColumnID j, TTElementID e, T& data) const
{
	return instance()->get2d(i, j, e, data);
}


template<typename T>
TTErr TTMatrix::set2d(TTRowID i, TTColumnID j, T data)
{
	return instance()->set2d(i, j, data);
}


template<typename T>
TTErr TTMatrix::set2d(TTRowID i, TTColumnID j, TTElementID e, T data)
{
	return instance()->set2d(i, j, e, data);
}


#endif // __TT_OBJECT_TEMPLATES_H__
