#pragma once

#include <ossia/detail/config.hpp>

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

#include <functional>
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
static constexpr auto curve_segment_type_map()
{
  if constexpr(std::is_same_v<T, int>)
    return ossia::curve_segment_type::INT;
  else if constexpr(std::is_same_v<T, int64_t>)
    return ossia::curve_segment_type::INT64;
  else if constexpr(std::is_same_v<T, float>)
    return ossia::curve_segment_type::FLOAT;
  else if constexpr(std::is_same_v<T, double>)
    return ossia::curve_segment_type::DOUBLE;
  else if constexpr(std::is_same_v<T, bool>)
    return ossia::curve_segment_type::BOOL;
  else if constexpr(std::is_same_v<T, ossia::value>)
    return ossia::curve_segment_type::ANY;
  else
    return nullptr;
}

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
  curve(const curve& other)
  {
    m_x0 = other.m_x0;
    m_y0 = other.m_y0;
    m_y0_destination = other.m_y0_destination;

    m_points = other.m_points;

    m_y0_cacheUsed = false;
  }

  curve(curve&& other) noexcept
  {
    m_x0 = other.m_x0;
    m_y0 = other.m_y0;
    m_y0_destination = std::move(other.m_y0_destination);

    m_points = std::move(other.m_points);

    m_y0_cacheUsed = false;
  }

  curve& operator=(const curve& other)
  {
    m_x0 = other.m_x0;
    m_y0 = other.m_y0;
    m_y0_destination = other.m_y0_destination;

    m_points = other.m_points;

    m_y0_cacheUsed = false;
    return *this;
  }
  curve& operator=(curve&& other) noexcept
  {
    m_x0 = other.m_x0;
    m_y0 = other.m_y0;
    m_y0_destination = std::move(other.m_y0_destination);

    m_points = std::move(other.m_points);

    m_y0_cacheUsed = false;
    return *this;
  }
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

  /*! get initial point ordinate destination
 \return const Destination* */
  std::optional<destination> get_y0_destination() const;

  /*! set initial curve ordinate using a Destination
 \param const Destination* */
  void set_y0_destination(const ossia::destination& destination);

  /*! get points of the curve
  \return std::map<X, pair<Y, CurveSegment<Y>>> map of {abscissa, {value,
          previous segment}
  */
  map_type get_points() const;

  static Y convert_to_template_type_value(
      const ossia::value& value, ossia::destination_index::const_iterator idx);

  void reserve(std::size_t count) { m_points.reserve(count); }

private:
  mutable X m_x0;
  mutable Y m_y0;
  mutable std::optional<ossia::destination> m_y0_destination;

  mutable map_type m_points;

  mutable Y m_y0_cache;

  mutable bool m_y0_cacheUsed = false;
};

template <typename X, typename Y>
inline void curve<X, Y>::reset()
{
  m_y0_cacheUsed = false;
}

template <typename X, typename Y>
inline bool
curve<X, Y>::add_point(ossia::curve_segment<Y>&& segment, X abscissa, Y value)
{
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
  for(auto it = m_points.begin(); it != end; ++it)
  {
    if(abscissa > lastAbscissa && abscissa <= it->first)
    {
      lastValue = it->second.second(
          ((double)abscissa - (double)lastAbscissa)
              / ((double)it->first - (double)lastAbscissa),
          lastValue, it->second.first);
      break;
    }
    else if(abscissa > it->first)
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
  static constexpr auto type
      = std::make_pair(curve_segment_type_map<X>(), curve_segment_type_map<Y>());
  return type;
}

template <typename X, typename Y>
inline X curve<X, Y>::get_x0() const
{
  return m_x0;
}

template <typename X, typename Y>
inline Y curve<X, Y>::get_y0() const
{
  if(!m_y0_destination)
    return m_y0;
  else
  {
    if(m_y0_cacheUsed)
      return m_y0_cache;

    const destination& dest = *m_y0_destination;
    m_y0_cacheUsed = true;
    m_y0_cache = convert_to_template_type_value(
        dest.address().fetch_value(), dest.index.begin());

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
inline std::optional<destination> curve<X, Y>::get_y0_destination() const
{
  return m_y0_destination;
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
inline Y curve<X, Y>::convert_to_template_type_value(
    const value& v, ossia::destination_index::const_iterator idx)
{
  using namespace ossia;
  using namespace std;
  struct visitor
  {
    destination_index::const_iterator index;
    Y operator()(int32_t i) const { return i; }
    Y operator()(float f) const { return f; }
    Y operator()(bool b) const { return b; }
    Y operator()(char c) const { return c; }
    Y operator()(vec2f vec) const { return vec[*index]; }
    Y operator()(vec3f vec) const { return vec[*index]; }
    Y operator()(vec4f vec) const { return vec[*index]; }
    Y operator()(const std::vector<ossia::value>& t) const
    {
      auto& val = t[*index];
      return convert_to_template_type_value(val, index + 1);
    }
    Y operator()(const value_map_type& t) const
    {
#if defined(__cpp_exceptions)
      throw invalid_value_type_error(
          "curve_impl::convertToTemplateTypeValue: "
          "Cannot convert Map to a numeric type");
#endif
      return {};
    }

    Y operator()(impulse) const
    {
#if defined(__cpp_exceptions)
      throw invalid_value_type_error(
          "curve_impl::convertToTemplateTypeValue: "
          "Cannot convert Impulse to a numeric type");
#endif
      return {};
    }
    Y operator()(const std::string& str) const
    {
#if defined(__cpp_exceptions)
      throw invalid_value_type_error(
          "curve_impl::convertToTemplateTypeValue: "
          "Cannot convert String to a numeric type");
#endif
      return {};
    }
    Y operator()() const
    {
#if defined(__cpp_exceptions)
      throw invalid_value_type_error(
          "curve_impl::convertToTemplateTypeValue: "
          "No type provided");
#endif
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
 * while keeping the rest of the list intact.
 */
class OSSIA_EXPORT constant_curve final : public curve_abstract
{
public:
  constant_curve(ossia::value v)
      : mValue{std::move(v)}
  {
  }
  ~constant_curve() override;
  constant_curve() = default;
  constant_curve(const constant_curve&) = delete;
  constant_curve(constant_curve&&) = delete;
  constant_curve& operator=(const constant_curve&) = delete;
  constant_curve& operator=(constant_curve&&) = delete;

  [[nodiscard]] ossia::value value() const { return mValue; }

  [[nodiscard]] curve_type get_type() const override
  {
    return std::make_pair(
        ossia::curve_segment_type::DOUBLE, ossia::curve_segment_type::ANY);
  }

  void reset() override { }

private:
  const ossia::value mValue;
};
}
