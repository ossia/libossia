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

template <typename X, typename Y>
class JamomaCurve : public Curve<X,Y>
{

private:
  
# pragma mark -
# pragma mark Implementation specific
  
  Y mInitialValue;
  Destination* mInitialDestination;
  vector<char> mInitialDestinationIndex;
  map<X, pair<Y, shared_ptr<CurveSegment<Y>>>> mPointsMap;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaCurve();
  
  JamomaCurve(const JamomaCurve *);
  
  shared_ptr<Curve<X,Y>> clone() const override;
  
  ~JamomaCurve();
  
# pragma mark -
# pragma mark Edition
  
  bool addPoint(X, Y, shared_ptr<CurveSegment<Y>>) override;
  
  bool removePoint(X) override;
  
# pragma mark -
# pragma mark Execution
  
  Y valueAt(X) const override;
  
# pragma mark -
# pragma mark Accessors
  
  Y getInitialValue() const override;
  
  void setInitialValue(Y) override;
  
  const Destination* getInitialDestination() const override;
  
  void setInitialDestination(const Destination*) override;

  vector<char> getInitialDestinationIndex() const override;

  void setInitialDestinationIndex(std::initializer_list<char>) override;
  
  map<X, pair<Y, shared_ptr<CurveSegment<Y>>>> getPointsMap() const override;

# pragma mark -
# pragma mark Implementation specific

  Y convertToTemplateTypeValue(const Value *, char*) const;
};
