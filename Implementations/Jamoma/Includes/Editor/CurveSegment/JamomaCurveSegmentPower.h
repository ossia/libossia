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

template <typename T>
class JamomaCurveSegmentPower : public CurveSegmentPower<T>
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<Curve<T>>  mParent;
  
  double                mPower;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaCurveSegmentPower(shared_ptr<Curve<T>>);
  
  JamomaCurveSegmentPower(const JamomaCurveSegmentPower *);
  
  shared_ptr<CurveSegmentPower<T>> clone() const override;

  ~JamomaCurveSegmentPower();
  
# pragma mark -
# pragma mark Execution

  T valueAt(const TimeValue&, T, T) const override;
  
# pragma mark -
# pragma mark Accessors
  
  shared_ptr<Curve<T>> getParent() const override;
  
  double getPower() const override;

  CurveSegmentPower<T> & setPower(double) override;

};
