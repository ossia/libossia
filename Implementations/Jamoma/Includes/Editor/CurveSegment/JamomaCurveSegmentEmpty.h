/*!
 * \file JamomaCurveSegmentEmpty.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Editor/CurveSegment/CurveSegmentEmpty.h"

using namespace OSSIA;
using namespace std;

template <typename Y>
class JamomaCurveSegmentEmpty final : public CurveSegmentEmpty<Y>
{

private:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif

  shared_ptr<CurveAbstract> mParent;

public:
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaCurveSegmentEmpty(shared_ptr<CurveAbstract>);

  JamomaCurveSegmentEmpty(const JamomaCurveSegmentEmpty *);

  shared_ptr<CurveSegmentEmpty<Y>> clone() const override;

  ~JamomaCurveSegmentEmpty();
#if 0
# pragma mark -
# pragma mark Execution
#endif

  Y valueAt(double, Y, Y) const override;
#if 0
# pragma mark -
# pragma mark Accessors
#endif

  shared_ptr<CurveAbstract> getParent() const override;

};
