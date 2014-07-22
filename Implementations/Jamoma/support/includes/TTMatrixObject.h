/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief #TTMatrixObject is a #TTObject specialized for processing matrices
 *
 * @details
 *
 * @authors Timothy Place
 *
 * @copyright Copyright © 2014 by Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_MATRIX_OBJECT_H__
#define __TT_MATRIX_OBJECT_H__

#include "TTBase.h"
#include "TTObject.h"
#include "TTValue.h"

class TTMatrix;
class TTDataObjectBase;


/** Wrap TTMatrixBase instances. */
class TTFOUNDATION_EXPORT TTMatrixObject : public TTObject {
public:
	
	/** Constructor */
	TTMatrixObject(const TTSymbol aClassName, const TTValue arguments = kTTValNONE):
	TTObject(aClassName, arguments)
	{}

	
	/** Get a pointer to the wrapped #TTMatrixBase instance. */
	TTDataObjectBase* instance() const
	{
		return (TTDataObjectBase*)mObjectInstance;
	}
	
	TTErr calculate(TTMatrix x, TTMatrix y);

};


#endif // __TT_MATRIX_H__
