/** @file
 *
 * @ingroup scoreLibrary
 *
 * @brief Unit test for Score library
 *
 * @details The unit test allows to ... @n@n
 *
 * @see TTImeEvent, TTTimeProcess
 *
 * @authors Théo de la Hogue & Clément Bossut
 *
 * @copyright Copyright © 2013, Théo de la Hogue & Clément Bossut @n
 * This code is licensed under the terms of the "CeCILL-C" @n
 * http://www.cecill.info
 */

#ifndef __TT_SCORETEST_H__
#define __TT_SCORETEST_H__

#include "TTObjectBase.h"
#include "TTUnitTest.h"

/**	Provide unit tests for Score framework */
class TTScoreTest : public TTObjectBase {
	TTCLASS_SETUP(TTScoreTest)
		
	virtual TTErr test(TTValue& returnedTestInfo);
};


#endif // __TT_SCORETEST_H__
