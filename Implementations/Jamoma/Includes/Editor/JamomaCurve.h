/*!
 * \file JamomaCurve.h
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

#include "Editor/Curve.h"

using namespace OSSIA;
using namespace std;

template <typename T>
class JamomaCurve : public Curve<T>
{

private:
  
# pragma mark -
# pragma mark Implementation specific
  
  T mInitialValue;
  Destination* mInitialDestination;
  map<const TimeValue, pair<T, shared_ptr<CurveSegment<T>>>> mPointsMap;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaCurve();
  
  JamomaCurve(const JamomaCurve *);
  
  shared_ptr<Curve<T>> clone() const override;
  
  ~JamomaCurve();
  
# pragma mark -
# pragma mark Edition
  
  bool addPoint(const TimeValue&, T, shared_ptr<CurveSegment<T>>) override;
  
  bool removePoint(const TimeValue&) override;
  
# pragma mark -
# pragma mark Execution
  
  T valueAt(const TimeValue&) const override;
  
# pragma mark -
# pragma mark Accessors
  
  T getInitialValue() const override;
  
  void setInitialValue(const T) override;
  
  const Destination* getInitialDestination() const override;
  
  void setInitialDestination(const Destination*) override;
  
  map<const TimeValue, pair<T, shared_ptr<CurveSegment<T>>>> getPointsMap() const override;

# pragma mark -
# pragma mark Implementation specific
  
  T convertToTemplateTypeValue(const Value *) const;
};
