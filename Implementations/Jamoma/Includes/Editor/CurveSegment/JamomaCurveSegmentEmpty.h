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

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<CurveAbstract> mParent;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaCurveSegmentEmpty(shared_ptr<CurveAbstract>);

  JamomaCurveSegmentEmpty(const JamomaCurveSegmentEmpty *);

  shared_ptr<CurveSegmentEmpty<Y>> clone() const override;

  ~JamomaCurveSegmentEmpty();

# pragma mark -
# pragma mark Execution

  Y valueAt(double, Y, Y) const override;

# pragma mark -
# pragma mark Accessors

  shared_ptr<CurveAbstract> getParent() const override;

};
