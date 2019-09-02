#pragma once
#include <ossia/dataflow/timed_value.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/detail/destination_index.hpp>
namespace ossia
{


struct typed_value
{
  explicit typed_value(ossia::value&& v) : value{std::move(v)}
  {
  }
  explicit typed_value(ossia::timed_value&& v)
      : value{std::move(v.value)}, timestamp{v.timestamp}
  {
  }
  typed_value(ossia::value&& v, const destination_index& d)
      : value{std::move(v)}, index{d}
  {
  }
  typed_value(
      ossia::value&& v, const destination_index& d,
      const ossia::complex_type& u)
      : value{std::move(v)}, index{d}, type{u}
  {
  }
  typed_value(
      ossia::timed_value&& v, const destination_index& d,
      const ossia::complex_type& u)
      : value{std::move(v.value)}, timestamp{v.timestamp}, index{d}, type{u}
  {
  }

  explicit typed_value(const ossia::value& v) : value{v}
  {
  }
  explicit typed_value(const ossia::timed_value& v)
      : value{v.value}, timestamp{v.timestamp}
  {
  }
  typed_value(const ossia::value& v, const destination_index& d)
      : value{v}, index{d}
  {
  }
  typed_value(
      const ossia::value& v, const destination_index& d,
      const ossia::complex_type& u)
      : value{v}, index{d}, type{u}
  {
  }
  typed_value(
      const ossia::timed_value& v, const destination_index& d,
      const ossia::complex_type& u)
      : value{v.value}, timestamp{v.timestamp}, index{d}, type{u}
  {
  }

  typed_value() = default;
  ~typed_value() = default;
  typed_value(const typed_value&) = default;
  typed_value(typed_value&&) = default;
  typed_value& operator=(const typed_value&) = default;
  typed_value& operator=(typed_value&&) = default;

  ossia::value value{};
  int64_t timestamp{};
  destination_index index{};
  ossia::complex_type type{};
};


}
