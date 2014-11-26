/** @file
 *
 * @ingroup foundationLibrary
 *
 * @brief #TTMatrix wraps a TTMatrixBase for use.
 *
 * @details
 *
 * @author Timothy Place, Nathan Wolek
 *
 * @copyright Copyright © 2014 by Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __TT_MATRIX_H__
#define __TT_MATRIX_H__

#include "TTBase.h"
#include "TTObject.h"

class TTMatrixBase;


/** Wrap TTMatrixBase instances. */
class TTFOUNDATION_EXPORT TTMatrix : public TTObject {
public:
	
	/** Constructor */
	TTMatrix():
	TTObject("matrix")
	{}
	
	
	/** Get a pointer to the wrapped #TTMatrixBase instance. */
	TTMatrixBase* instance() const
	{
		return (TTMatrixBase*)mObjectInstance;
	}
	
	
	/** Set all components of a matrix to zero. */
	void clear();
	
	
	/**	You must proceed to set the various attributes, dimensions, etc. to match the data format of the matrix you are referencing.
	 
		One caveat regards data alignment.  Jitter, for example, aligns rows on 16-byte boundaries.
		In this case, a 4x10 matrix (using the m-by-n convention rather than Jitter's width-by-height convention) of 32-bit ints,
		all with a value of "4" will look like this:

		4, 4, 4, 4,   4, 4, 4, 4,   4, 4, 0, 0
		4, 4, 4, 4,   4, 4, 4, 4,   4, 4, 0, 0
		4, 4, 4, 4,   4, 4, 4, 4,   4, 4, 0, 0
		4, 4, 4, 4,   4, 4, 4, 4,   4, 4, 0, 0

		Thus, the rows are really of a dimension length 12 instead of 10 and the total size of the matrix scales as well.

		For the time being, we do not handle this case.
		Jitter users must dimension their matrices so that the row size in bytes is a multiple of 16.
		This is not actually hard to do for most purposes.  For example:

		float64, element count of 1, width is a multiple of 2 (an even number)
		float32 or int32, element count of 1, width is a multiple of 4
		uint8, element count of 1, width is a multiple of 16 (which includes 80, 160, 320, 640, ...), but
		element count of 4 (i.e. color pixels) width should be a multiple of values.
	 */
	void referenceExternalData(TTPtr aDataPointer);

	
	/** Return a pointer to the matrix data, and lock the matrix so that others cannot access the data.
	 If matrix is already locked, this function waits until it becomes free. */
	TTByte* getLockedPointer();
	
	
	/**	Release a locked pointer obtained using getLockedPointer().	*/
	void releaseLockedPointer();
	
	
	
	/**	Number of rows in the matrix.
		@return	TTRowID		the value stored at mRowCount
	 */
	TTRowID getRowCount() const;

	
	/**	Number of columns in the matrix.
		@return	TTColumnID	the value stored at mColumnCount
	 */
	TTColumnID getColumnCount() const;

	
	/** Return number of bytes from one the beginning one matrix component to the next. */
	TTUInt32 getComponentStride() const;

		
	/**	Get the value of a component located at (i,j) in a 2-dimensional matrix.
	 Remember that the first component in the matrix is (0,0).
	 
	 In order to provide some degree of efficiency, the data passed-in is not bounds checked --
	 you must ensure that you are passing memory that is at least mComponentStride bytes large.
	 
	 In fact, you should pass a compound type if you want more than one of the primitive types.
	 For example, pass a pointer to a TTComplex if you want two doubles.
	 
	 @param[in]	i			row in matrix of desired component
	 @param[in]	j			column in matrix of desired component
	 @param[out]	data		reference to where method should return value
	 @return 	TTErr		always returns kTTErrNone
	 */
	template<typename T>
	TTErr get2d(TTRowID i, TTColumnID j, T& data) const;
	
	
	/**	Get the value of element e of the component located at (i,j) in a 2-dimensional matrix.
	 Remember that the first component in the matrix is (0,0) and its first element is 0.
	 
	 In order to provide some degree of efficiency, the data passed-in is not bounds checked --
	 you must ensure that you are passing memory that is at least mComponentStride bytes large.
	 
	 In fact, you should pass a compound type if you want more than one of the primitive types.
	 For example, pass a pointer to a TTComplex if you want two doubles.
	 
	 @param[in]	i			row in matrix of desired component
	 @param[in]	j			column in matrix of desired component
	 @param[in]	e			element within matrix component
	 @param[out]	data		reference to where method should return value
	 @return 	TTErr		always returns kTTErrNone
	 */
	template<typename T>
	TTErr get2d(TTRowID i, TTColumnID j, TTElementID e, T& data) const;
	
		
	/**	Set the value of a component located at (i,j) in a 2-dimensional matrix.
	 Remember that the first component in the matrix is (0,0).
	 
	 In order to provide some degree of efficiency, the data passed-in is not bounds checked --
	 you must ensure that you are passing memory that is at least mComponentStride bytes large.
	 
	 In fact, you should pass a compound type if you want more than one of the primitive types.
	 For example, pass a pointer to a TTComplex if you want two doubles.
	 
	 @param[in]	i			row in matrix of component to be set
	 @param[in]	j			column in matrix of component to be set
	 @param[out]	data		reference to where method should return value
	 @return 	TTErr		always returns kTTErrNone
	 */
	template<typename T>
	TTErr set2d(TTRowID i, TTColumnID j, T data);
	
	
	/**	Set the value of element e of the component located at (i,j) in a 2-dimensional matrix.
	 Remember that the first component in the matrix is (0,0) and its first element is 0.
	 
	 In order to provide some degree of efficiency, the data passed-in is not bounds checked --
	 you must ensure that you are passing memory that is at least mComponentStride bytes large.
	 
	 In fact, you should pass a compound type if you want more than one of the primitive types.
	 For example, pass a pointer to a TTComplex if you want two doubles.
	 
	 @param[in]	i			row in matrix of component to be set
	 @param[in]	j			column in matrix of component to be set
	 @param[in]	e			element within matrix component
	 @param[out]	data		reference to where method should return value
	 @return 	TTErr		always returns kTTErrNone
	 */
	template<typename T>
	TTErr set2d(TTRowID i, TTColumnID j, TTElementID e, T data);
	
};



#endif // __TT_MATRIX_H__
