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

template <typename T>
class JamomaCurveSegmentLinear : public CurveSegmentLinear<T>
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<Curve<T>> mParent;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaCurveSegmentLinear(shared_ptr<Curve<T>>);

  ~JamomaCurveSegmentLinear();
  
# pragma mark -
# pragma mark Execution

  T valueAt(double, T) const override;
  
# pragma mark -
# pragma mark Accessors
  
  shared_ptr<Curve<T>> getParent() const override;

};
