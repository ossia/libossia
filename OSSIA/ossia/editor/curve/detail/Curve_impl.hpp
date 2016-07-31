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


namespace detail
{

template<typename... Args>
using curve_map = boost::container::flat_map<Args...>;

template <typename X, typename Y>
class curve_impl final : public ossia::curve<X,Y>
{

private:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  X mInitialPointAbscissa;
  ossia::Destination mInitialPointAbscissaDestination;

  Y mInitialPointOrdinate;
  mutable Y mInitialPointOrdinateCache;
  mutable bool mInitialPointOrdinateCacheUsed = false;
  ossia::Destination mInitialPointOrdinateDestination;

  using map_type = curve_map<X, std::pair<Y, ossia::curve_segment<Y>>>;
  map_type mPointsMap;

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  curve_impl();

  ~curve_impl();

  // Call me before execution to reset the cache.
  void reset() override
  {
      mInitialPointOrdinateCacheUsed = false;
  }


#if 0
# pragma mark -
# pragma mark Edition
#endif

  bool addPoint(ossia::curve_segment<Y>, X, Y) override;

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

  ossia::curve_type getType() const override;

  X getInitialPointAbscissa() const override;

  Y getInitialPointOrdinate() const override;

  void setInitialPointAbscissa(X) override;

  void setInitialPointOrdinate(Y) override;

  const ossia::Destination& getInitialPointAbscissaDestination() const override;

  const ossia::Destination& getInitialPointOrdinateDestination() const override;

  void setInitialPointAbscissaDestination(const ossia::Destination&) override;

  void setInitialPointOrdinateDestination(const ossia::Destination&) override;

  std::map<X, std::pair<Y, ossia::curve_segment<Y>>> getPointsMap() const override;

#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  static Y convertToTemplateTypeValue(const ossia::value&, ossia::destination_index::const_iterator);
};

template<typename T>
const constexpr std::nullptr_t OssiaType{};
template<>
const constexpr ossia::curve_segment_type OssiaType<int> = ossia::curve_segment_type::INT;
template<>
const constexpr ossia::curve_segment_type OssiaType<float> = ossia::curve_segment_type::FLOAT;
template<>
const constexpr ossia::curve_segment_type OssiaType<double> = ossia::curve_segment_type::DOUBLE;
template<>
const constexpr ossia::curve_segment_type OssiaType<bool> = ossia::curve_segment_type::BOOL;

}
