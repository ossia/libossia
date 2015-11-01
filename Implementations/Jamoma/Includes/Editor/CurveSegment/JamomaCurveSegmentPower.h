/*!
 * \file JamomaCurveSegmentPower.h
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

#include <cmath>

#include "Editor/CurveSegment/CurveSegmentPower.h"

using namespace OSSIA;
using namespace std;

template <typename Y>
class JamomaCurveSegmentPower final : public CurveSegmentPower<Y>
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<CurveAbstract> mParent;
  
  double                    mPower;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaCurveSegmentPower(shared_ptr<CurveAbstract>);
  
  JamomaCurveSegmentPower(const JamomaCurveSegmentPower *);
  
  shared_ptr<CurveSegmentPower<Y>> clone() const override;

  ~JamomaCurveSegmentPower();
  
# pragma mark -
# pragma mark Execution

  Y valueAt(double, Y, Y) const override;
  
# pragma mark -
# pragma mark Accessors
  
  shared_ptr<CurveAbstract> getParent() const override;
  
  double getPower() const override;

  CurveSegmentPower<Y> & setPower(double) override;

};
