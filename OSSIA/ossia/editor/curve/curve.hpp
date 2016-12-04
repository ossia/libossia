#pragma once

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/curve/curve_abstract.hpp>
#include <ossia/editor/curve/curve_segment.hpp>
#include <ossia/editor/value/destination.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/editor/value/value.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/optional.hpp>
#include <functional>
#include <map>
#include <memory>
#include <ossia_export.h>
#include <utility>
namespace ossia
{
class Destination;


template <typename T>
const constexpr std::nullptr_t curve_segment_type_map{};
template <>
const constexpr ossia::curve_segment_type
    curve_segment_type_map<int> = ossia::curve_segment_type::INT;
template <>
const constexpr ossia::curve_segment_type
    curve_segment_type_map<float> = ossia::curve_segment_type::FLOAT;
template <>
const constexpr ossia::curve_segment_type
    curve_segment_type_map<double> = ossia::curve_segment_type::DOUBLE;
template <>
const constexpr ossia::curve_segment_type
    curve_segment_type_map<bool> = ossia::curve_segment_type::BOOL;
template <>
const constexpr ossia::curve_segment_type
    curve_segment_type_map<ossia::value> = ossia::curve_segment_type::ANY;

template <typename... Args>
using curve_map = boost::container::flat_map<Args...>;

template <typename X, typename Y>
/**
 * @brief The curve class
 *
 * A curve is a succession of \ref curve_segment.
 * It is used in a \ref Behavior to drive \ref automation or \ref mapper.
 */
class curve final :
    public curve_abstract
{
public:
  using abscissa_type = X;
  using ordinate_type = Y;
  using curve_type = curve<X, Y>;

  curve() = default;
  curve(const curve&) = delete;
  curve(curve&&) = delete;
  curve& operator=(const curve&) = delete;
  curve& operator=(curve&&) = delete;
  /*! destructor */
  virtual ~curve() = default;

  void reset() override
  {
    mInitialPointOrdinateCacheUsed = false;
  }


  /*! add a segment to reach a target point to the curve
 \param std::shared_ptr<#CurveSegment<Y>> segment to target point
 \param X target point abscissa
 \param Y target point ordinate
 \return bool */
  bool addPoint(
      ossia::curve_segment<Y> segment, X abscissa, Y value)
  {
    mPointsMap.emplace(abscissa, std::make_pair(value, std::move(segment)));

    return true;
  }


  /*! remove a point from the curve
 \param X point abscissa
 \return bool */
  bool removePoint(X abscissa)
  {
    return mPointsMap.erase(abscissa) > 0;
  }


  /*! get value at an abscissa
 \param X abscissa.
 \return Y ordinate */
  Y valueAt(X abscissa) const
  {
    X lastAbscissa = getInitialPointAbscissa();
    Y lastValue = getInitialPointOrdinate();

    auto end = mPointsMap.end();
    for (auto it = mPointsMap.begin(); it != end; ++it)
    {
      if (abscissa > lastAbscissa && abscissa <= it->first)
      {
        lastValue = it->second.second(
            ((double)abscissa - (double)lastAbscissa)
                / ((double)it->first - (double)lastAbscissa),
            lastValue, it->second.first);
        break;
      }
      else if (abscissa > it->first)
      {
        lastAbscissa = it->first;
        lastValue = it->second.first;
      }
      else
        break;
    }

    return lastValue;
  }

  ossia::curve_type getType() const override
  {
    return std::make_pair(curve_segment_type_map<X>, curve_segment_type_map<Y>);
  }

  /*! get initial point abscissa
 \details if there is an initial abcissa destination, it will return the value
 of the address
 \return X value */
  X getInitialPointAbscissa() const
  {
    if(!mInitialPointAbscissaDestination)
      return mInitialPointAbscissa;
    else
    {
      auto& address = mInitialPointAbscissaDestination->value.get();
      address.pullValue();
      auto val = address.cloneValue();
      auto res = convertToTemplateTypeValue(
          val, mInitialPointAbscissaDestination->index.begin());
      return res;
    }
  }


  /*! get initial point ordinate
 \details if there is an initial ordinate destination, it will return the value
 of the address
 \return Y value */
  Y getInitialPointOrdinate() const
  {
    if(!mInitialPointOrdinateDestination)
      return mInitialPointOrdinate;
    else
    {
      if (mInitialPointOrdinateCacheUsed)
        return mInitialPointOrdinateCache;

      const Destination& dest = *mInitialPointOrdinateDestination;
      mInitialPointOrdinateCacheUsed = true;
      mInitialPointOrdinateCache = convertToTemplateTypeValue(
          dest.value.get().fetchValue(), dest.index.begin());
      return mInitialPointOrdinateCache;
    }
  }

  /*! set initial point abscissa
 \details if there is an initial abscissa destination, this accessor is useless
 \param X abscissa */
  void setInitialPointAbscissa(X value)
  {
    mInitialPointAbscissa = value;
  }

  /*! set initial point ordinate
 \details if there is an initial ordinate destination, this accessor is useless
 \param Y ordinate */
  void setInitialPointOrdinate(Y value)
  {
    mInitialPointOrdinate = value;
  }

  /*! get initial point abscissa destination
 \return const Destination* */
  boost::optional<Destination> getInitialPointAbscissaDestination() const
  {
    return mInitialPointAbscissaDestination;
  }


  /*! get initial point ordinate destination
 \return const Destination* */
  boost::optional<Destination> getInitialPointOrdinateDestination() const
  {
    return mInitialPointOrdinateDestination;
  }


  /*! set initial curve abscissa using a Destination
 \param const Destination* */
  void setInitialPointAbscissaDestination(
      const ossia::Destination& destination)
  {
    mInitialPointAbscissaDestination = destination;
  }

  /*! set initial curve ordinate using a Destination
 \param const Destination* */
  void setInitialPointOrdinateDestination(
      const ossia::Destination& destination)
  {
    mInitialPointOrdinateDestination = destination;
  }

  /*! get points of the curve
  \return std::map<X, pair<Y, CurveSegment<Y>>> map of {abscissa, {value,
          previous segment}
  */
  std::map<X, std::pair<Y, curve_segment<Y>>> getPointsMap() const
  {
    return {mPointsMap.cbegin(), mPointsMap.cend()};
  }


  static Y convertToTemplateTypeValue(
      const ossia::value& value, ossia::destination_index::const_iterator idx)
  {
    using namespace ossia;
    using namespace std;
    struct visitor
    {
      destination_index::const_iterator index;
      Y operator()(int32_t i) const
      {
        return i;
      }
      Y operator()(float f) const
      {
        return f;
      }
      Y operator()(bool b) const
      {
        return b;
      }
      Y operator()(char c) const
      {
        return c;
      }
      Y operator()(Vec2f vec) const
      {
        return vec[*index];
      }
      Y operator()(Vec3f vec) const
      {
        return vec[*index];
      }
      Y operator()(Vec4f vec) const
      {
        return vec[*index];
      }
      Y operator()(const std::vector<ossia::value>& t) const
      {
        auto& val = t[*index];
        return convertToTemplateTypeValue(val, index + 1);
      }

      Y operator()(Impulse) const
      {
        throw invalid_value_type_error("curve_impl::convertToTemplateTypeValue: "
                                       "Cannot convert Impulse to a numeric type");
        return {};
      }
      Y operator()(const std::string& str) const
      {
        throw invalid_value_type_error("curve_impl::convertToTemplateTypeValue: "
                                       "Cannot convert String to a numeric type");
        return {};
      }
      Y operator()(const Destination& d) const
      {
        throw invalid_value_type_error("curve_impl::convertToTemplateTypeValue: "
                                       "Cannot convert Destination to a numeric type");
        return {};
        ;
      }
      Y operator()() const
      {
        throw invalid_value_type_error("curve_impl::convertToTemplateTypeValue: "
                                       "No type provided");
        return {};
      }
    } vis{idx};

    return value.apply(vis);
  }

private:
  X mInitialPointAbscissa;
  boost::optional<ossia::Destination> mInitialPointAbscissaDestination;

  Y mInitialPointOrdinate;
  boost::optional<ossia::Destination> mInitialPointOrdinateDestination;

  using map_type = curve_map<X, std::pair<Y, ossia::curve_segment<Y>>>;
  map_type mPointsMap;

  mutable Y mInitialPointOrdinateCache;
  mutable bool mInitialPointOrdinateCacheUsed = false;

};

/**
 * @brief The constant_curve class
 * A curve that always return a same single value.
 *
 * It is useful if for instance one wants to automate on the
 * first value of the following tuple :
 *
 * [ 1, "a string", [ "another", 'c' ] ]
 *
 * while keeping the reste of the tuple intact.
 */
class constant_curve final : public curve_abstract
{
public:
  constant_curve(ossia::value v): mValue{std::move(v)} { }
  constant_curve() = default;
  constant_curve(const constant_curve&) = delete;
  constant_curve(constant_curve&&) = delete;
  constant_curve& operator=(const constant_curve&) = delete;
  constant_curve& operator=(constant_curve&&) = delete;

  ossia::value value() const
  {
    return mValue;
  }

  curve_type getType() const override
  {
    return std::make_pair(
          ossia::curve_segment_type::DOUBLE,
          ossia::curve_segment_type::ANY);
  }

  void reset() override
  { }

private:
  const ossia::value mValue;
};
}
