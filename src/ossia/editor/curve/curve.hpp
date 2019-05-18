#pragma once

#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/curve/curve_abstract.hpp>
#include <ossia/editor/curve/curve_segment.hpp>
#include <ossia/editor/curve/curve_segment/easing.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/value/destination.hpp>
#include <ossia/network/value/value.hpp>

#include <ossia_export.h>

#include <functional>
#include <map>
#include <memory>
#include <utility>
#include <vector>

/**
 * \file curve.hpp
 */

namespace ossia
{
class destination;

template <typename T>
const constexpr std::nullptr_t curve_segment_type_map = nullptr;
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

template <typename K, typename V>
using curve_map = ossia::flat_map<K, V>;

template <typename X, typename Y>
/**
 * @brief The curve class
 *
 * A curve is a succession of \ref curve_segment.
 * It is used in a \ref Behavior to drive \ref automation or \ref mapper.
 */
class curve final : public curve_abstract
{
public:
  using abscissa_type = X;
  using ordinate_type = Y;
  using curve_type = curve<X, Y>;
  using map_type = curve_map<X, std::pair<Y, ossia::curve_segment<Y>>>;

  curve() = default;
  curve(const curve&) = delete;
  curve(curve&&) = delete;
  curve& operator=(const curve&) = delete;
  curve& operator=(curve&&) = delete;
  /*! destructor */
  virtual ~curve() = default;

  void reset() override;

  /*! add a segment to reach a target point to the curve
 \param std::shared_ptr<#CurveSegment<Y>> segment to target point
 \param X target point abscissa
 \param Y target point ordinate
 \return bool */
  bool add_point(ossia::curve_segment<Y>&& segment, X abscissa, Y value);

  /*! remove a point from the curve
 \param X point abscissa
 \return bool */
  bool remove_point(X abscissa);

  /*! get value at an abscissa
 \param X abscissa.
 \return Y ordinate */
  Y value_at(X abscissa) const;

  ossia::curve_type get_type() const override;

  /*! get initial point abscissa
 \details if there is an initial abcissa destination, it will return the value
 of the address
 \return X value */
  X get_x0() const;

  /*! get initial point ordinate
 \details if there is an initial ordinate destination, it will return the value
 of the address
 \return Y value */
  Y get_y0() const;

  /*! set initial point abscissa
 \details if there is an initial abscissa destination, this accessor is useless
 \param X abscissa */
  void set_x0(X value);

  /*! set initial point ordinate
 \details if there is an initial ordinate destination, this accessor is useless
 \param Y ordinate */
  void set_y0(Y value);

  /*! get initial point abscissa destination
 \return const Destination* */
  ossia::optional<destination> get_x0_destination() const;

  /*! get initial point ordinate destination
 \return const Destination* */
  ossia::optional<destination> get_y0_destination() const;

  /*! set initial curve abscissa using a Destination
 \param const Destination* */
  void set_x0_destination(const ossia::destination& destination);

  /*! set initial curve ordinate using a Destination
 \param const Destination* */
  void set_y0_destination(const ossia::destination& destination);

  /*! get points of the curve
  \return std::map<X, pair<Y, CurveSegment<Y>>> map of {abscissa, {value,
          previous segment}
  */
  map_type get_points() const;

  /**
   * @brief set_scale_bounds
   *
   * Use this if the curve ordinate is given between [0; 1] and has to be
   * rescaled to
   * the correct bounds when the first value is received.
   *
   * The arguments are the known bounds of the points at the time of creation.
   */
  void set_scale_bounds(Y min, Y max, Y end);

  static Y convert_to_template_type_value(
      const ossia::value& value, ossia::destination_index::const_iterator idx);

private:
  mutable X m_x0;
  mutable Y m_y0;
  mutable ossia::optional<ossia::destination> m_x0_destination;
  mutable ossia::optional<ossia::destination> m_y0_destination;

  mutable map_type m_points;

  mutable Y m_y0_cache;
  struct scale_info
  {
    Y min;
    Y max;
    Y start;
    Y end;
  };
  mutable optional<scale_info> m_scaleBounds;
  mutable optional<map_type> m_originalPoints;

  mutable bool m_y0_cacheUsed = false;
};

template <typename X, typename Y>
inline void curve<X, Y>::reset()
{
  m_y0_cacheUsed = false;
  if (m_originalPoints)
    m_points = *m_originalPoints;
}

template <typename X, typename Y>
inline bool
curve<X, Y>::add_point(ossia::curve_segment<Y>&& segment, X abscissa, Y value)
{
  if (m_scaleBounds)
    m_originalPoints->emplace(abscissa, std::make_pair(value, segment));
  m_points.emplace(abscissa, std::make_pair(value, std::move(segment)));

  return true;
}

template <typename X, typename Y>
inline bool curve<X, Y>::remove_point(X abscissa)
{
  return m_points.erase(abscissa) > 0;
}

template <typename X, typename Y>
inline Y curve<X, Y>::value_at(X abscissa) const
{
  X lastAbscissa = get_x0();
  Y lastValue = get_y0();

  auto end = m_points.end();
  for (auto it = m_points.begin(); it != end; ++it)
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

template <typename X, typename Y>
inline curve_type curve<X, Y>::get_type() const
{
  return std::make_pair(curve_segment_type_map<X>, curve_segment_type_map<Y>);
}

template <typename X, typename Y>
inline X curve<X, Y>::get_x0() const
{
  if (!m_x0_destination)
    return m_x0;
  else
  {
    auto& address = m_x0_destination->value.get();
    address.pull_value();
    auto val = address.value();
    auto res
        = convert_to_template_type_value(val, m_x0_destination->index.begin());
    return res;
  }
}

template <typename X, typename Y>
inline Y curve<X, Y>::get_y0() const
{
  if (!m_y0_destination)
    return m_y0;
  else
  {
    if (m_y0_cacheUsed)
      return m_y0_cache;

    const destination& dest = *m_y0_destination;
    m_y0_cacheUsed = true;
    m_y0_cache = convert_to_template_type_value(
        dest.address().fetch_value(), dest.index.begin());

    if (m_scaleBounds)
    {
      scale_info& bounds = *m_scaleBounds;
      bounds.start = m_y0_cache;

      // Rescale the whole curve with the acquired information.
      // It is currently in [0; 1].
      if (bounds.start < bounds.min)
        bounds.min = bounds.start;
      else if (bounds.start > bounds.max)
        bounds.max = bounds.start;

      if (bounds.start < bounds.end)
      {
        for (auto& pt : m_points.container)
        {
          Y& y = pt.second.first;
          y = ossia::easing::ease{}(bounds.min, bounds.max, y);
        }
      }
      else if (bounds.start == bounds.end)
      {
        m_points.clear();
        m_x0 = 0;
      }
      else // start > end
      {
        for (auto& pt : m_points.container)
        {
          auto& y = pt.second.first;
          const auto fun = [=](Y val) -> Y {
            return bounds.max - val * (bounds.max - bounds.min);
          };
          y = fun(y);
        }
      }
    }
    return m_y0_cache;
  }
}

template <typename X, typename Y>
inline void curve<X, Y>::set_x0(X value)
{
  m_x0 = value;
}

template <typename X, typename Y>
inline void curve<X, Y>::set_y0(Y value)
{
  m_y0 = value;
}

template <typename X, typename Y>
inline ossia::optional<destination> curve<X, Y>::get_x0_destination() const
{
  return m_x0_destination;
}

template <typename X, typename Y>
inline ossia::optional<destination> curve<X, Y>::get_y0_destination() const
{
  return m_y0_destination;
}

template <typename X, typename Y>
inline void curve<X, Y>::set_x0_destination(const destination& destination)
{
  m_x0_destination = destination;
}

template <typename X, typename Y>
inline void curve<X, Y>::set_y0_destination(const destination& destination)
{
  m_y0_destination = destination;
}

template <typename X, typename Y>
inline typename curve<X, Y>::map_type curve<X, Y>::get_points() const
{
  return m_points;
}

template <typename X, typename Y>
inline void curve<X, Y>::set_scale_bounds(Y min, Y max, Y end)
{
  m_scaleBounds = scale_info{min, max, Y{}, end};
  m_originalPoints = map_type{};
}

template <typename X, typename Y>
inline Y curve<X, Y>::convert_to_template_type_value(
    const value& v, ossia::destination_index::const_iterator idx)
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
    Y operator()(vec2f vec) const
    {
      return vec[*index];
    }
    Y operator()(vec3f vec) const
    {
      return vec[*index];
    }
    Y operator()(vec4f vec) const
    {
      return vec[*index];
    }
    Y operator()(const std::vector<ossia::value>& t) const
    {
      auto& val = t[*index];
      return convert_to_template_type_value(val, index + 1);
    }

    Y operator()(impulse) const
    {
      throw invalid_value_type_error(
          "curve_impl::convertToTemplateTypeValue: "
          "Cannot convert Impulse to a numeric type");
      return {};
    }
    Y operator()(const std::string& str) const
    {
      throw invalid_value_type_error(
          "curve_impl::convertToTemplateTypeValue: "
          "Cannot convert String to a numeric type");
      return {};
    }
    Y operator()() const
    {
      throw invalid_value_type_error(
          "curve_impl::convertToTemplateTypeValue: "
          "No type provided");
      return {};
    }
  } vis{idx};

  return v.apply(vis);
}

/**
 * @brief The constant_curve class
 * A curve that always return a same single value.
 *
 * It is useful if for instance one wants to automate on the
 * first value of the following list :
 *
 * [ 1, "a string", [ "another", 'c' ] ]
 *
 * while keeping the reste of the list intact.
 */
class OSSIA_EXPORT constant_curve final : public curve_abstract
{
public:
  constant_curve(ossia::value v) : mValue{std::move(v)}
  {
  }
  ~constant_curve() override;
  constant_curve() = default;
  constant_curve(const constant_curve&) = delete;
  constant_curve(constant_curve&&) = delete;
  constant_curve& operator=(const constant_curve&) = delete;
  constant_curve& operator=(constant_curve&&) = delete;

  ossia::value value() const
  {
    return mValue;
  }

  curve_type get_type() const override
  {
    return std::make_pair(
        ossia::curve_segment_type::DOUBLE, ossia::curve_segment_type::ANY);
  }

  void reset() override
  {
  }

private:
  const ossia::value mValue;
};
}
