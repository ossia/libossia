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
#include <Network/Address.h>
#include <Editor/Value/Destination.h>
#include <cmath>
#if defined(OSSIA_BOOST)
#include <boost/container/flat_map.hpp>
template<typename... Args>
using curve_map = boost::container::flat_map<Args...>;
#else
#include <map>
template<typename... Args>
using curve_map = std::map<Args...>;
#endif
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
  Destination mInitialPointAbscissaDestination;

  Y mInitialPointOrdinate;
  mutable Y mInitialPointOrdinateCache;
  mutable bool mInitialPointOrdinateCacheUsed = false;
  Destination mInitialPointOrdinateDestination;

  using map_type = curve_map<X, pair<Y, CurveSegment<Y>>>;
  map_type mPointsMap;

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaCurve();

  JamomaCurve(const JamomaCurve *);

  shared_ptr<Curve<X,Y>> clone() const override;

  ~JamomaCurve();

  // Call me before execution to reset the cache.
  void reset() override
  {
      mInitialPointOrdinateCacheUsed = false;
  }


#if 0
# pragma mark -
# pragma mark Edition
#endif

  bool addPoint(CurveSegment<Y>, X, Y) override;

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

  OSSIA::CurveType getType() const override;

  X getInitialPointAbscissa() const override;

  Y getInitialPointOrdinate() const override;

  void setInitialPointAbscissa(X) override;

  void setInitialPointOrdinate(Y) override;

  const Destination& getInitialPointAbscissaDestination() const override;

  const Destination& getInitialPointOrdinateDestination() const override;

  void setInitialPointAbscissaDestination(const Destination&) override;

  void setInitialPointOrdinateDestination(const Destination&) override;

  std::map<X, std::pair<Y, CurveSegment<Y>>> getPointsMap() const override;

#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  static Y convertToTemplateTypeValue(const SafeValue&, vector<char>::const_iterator);
};
