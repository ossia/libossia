/*!
 * \file JamomaCurveSegmentLinear.h
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

#include "Editor/CurveSegment/CurveSegmentLinear.h"

using namespace OSSIA;
using namespace std;

template <typename Y>
class JamomaCurveSegmentLinear final : public CurveSegmentLinear<Y>
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<CurveAbstract> mParent;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaCurveSegmentLinear(shared_ptr<CurveAbstract>);

  JamomaCurveSegmentLinear(const JamomaCurveSegmentLinear *);

  shared_ptr<CurveSegmentLinear<Y>> clone() const override;

  ~JamomaCurveSegmentLinear();

# pragma mark -
# pragma mark Execution

  Y valueAt(double, Y, Y) const override;

# pragma mark -
# pragma mark Accessors

  shared_ptr<CurveAbstract> getParent() const override;

};
