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

#include "math.h"

using namespace OSSIA;
using namespace std;

template <typename X, typename Y>
class JamomaCurve final : public Curve<X,Y>
{

private:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  X mInitialPointAbscissa;
  Destination* mInitialPointAbscissaDestination;
  
  Y mInitialPointOrdinate;
  Destination* mInitialPointOrdinateDestination;
  
  map<X, pair<Y, shared_ptr<CurveSegment<Y>>>> mPointsMap;

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaCurve();

  JamomaCurve(const JamomaCurve *);

  shared_ptr<Curve<X,Y>> clone() const override;

  ~JamomaCurve();

#if 0
# pragma mark -
# pragma mark Edition
#endif

  bool addPoint(shared_ptr<CurveSegment<Y>>, X, Y) override;

  bool removePoint(X) override;

#if 0
# pragma mark -
# pragma mark Execution
#endif

  Y valueAt(X) const override;

#if 0
# pragma mark -
# pragma mark Accessors
#endif
  
  X getInitialPointAbscissa() const override;
  
  Y getInitialPointOrdinate() const override;
  
  void setInitialPointAbscissa(X) override;

  void setInitialPointOrdinate(Y) override;

  const Destination* getInitialPointAbscissaDestination() const override;
  
  const Destination* getInitialPointOrdinateDestination() const override;
  
  void setInitialPointAbscissaDestination(const Destination*) override;
  
  void setInitialPointOrdinateDestination(const Destination*) override;

  map<X, pair<Y, shared_ptr<CurveSegment<Y>>>> getPointsMap() const override;

#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  Y convertToTemplateTypeValue(const Value *, vector<char>::iterator) const;
};
