/*!
 * \file Curve_impl.h
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

#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/value/destination.hpp>
#include <ossia/network/base/address.hpp>
#include <cmath>

#include <boost/container/flat_map.hpp>


namespace impl
{

template<typename... Args>
using curve_map = boost::container::flat_map<Args...>;

template <typename X, typename Y>
class JamomaCurve final : public OSSIA::Curve<X,Y>
{

private:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  X mInitialPointAbscissa;
  OSSIA::Destination mInitialPointAbscissaDestination;

  Y mInitialPointOrdinate;
  mutable Y mInitialPointOrdinateCache;
  mutable bool mInitialPointOrdinateCacheUsed = false;
  OSSIA::Destination mInitialPointOrdinateDestination;

  using map_type = curve_map<X, std::pair<Y, OSSIA::CurveSegment<Y>>>;
  map_type mPointsMap;

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaCurve();

  JamomaCurve(const JamomaCurve *);

  std::shared_ptr<OSSIA::Curve<X,Y>> clone() const override;

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

  bool addPoint(OSSIA::CurveSegment<Y>, X, Y) override;

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

  const OSSIA::Destination& getInitialPointAbscissaDestination() const override;

  const OSSIA::Destination& getInitialPointOrdinateDestination() const override;

  void setInitialPointAbscissaDestination(const OSSIA::Destination&) override;

  void setInitialPointOrdinateDestination(const OSSIA::Destination&) override;

  std::map<X, std::pair<Y, OSSIA::CurveSegment<Y>>> getPointsMap() const override;

#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  static Y convertToTemplateTypeValue(const OSSIA::Value&, OSSIA::DestinationIndex::const_iterator);
};

template<typename T>
const constexpr std::nullptr_t OssiaType{};
template<>
const constexpr OSSIA::CurveSegmentType OssiaType<int> = OSSIA::CurveSegmentType::INT;
template<>
const constexpr OSSIA::CurveSegmentType OssiaType<float> = OSSIA::CurveSegmentType::FLOAT;
template<>
const constexpr OSSIA::CurveSegmentType OssiaType<double> = OSSIA::CurveSegmentType::DOUBLE;
template<>
const constexpr OSSIA::CurveSegmentType OssiaType<bool> = OSSIA::CurveSegmentType::BOOL;

}
