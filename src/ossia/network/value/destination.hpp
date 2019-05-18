#pragma once
#include <ossia/detail/destination_index.hpp>
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/value/value.hpp>

#include <ossia_export.h>

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include <initializer_list>
/**
 * \file destination.hpp
 */
namespace ossia
{
class value;
namespace net
{
class parameter_base;
}
/**
 * @class Destination destination.hpp ossia/network/value/value.hpp
 *
 * \see \ref node_base
 * \see \ref destination_index
 */
class OSSIA_EXPORT destination final
{
public:
  std::reference_wrapper<net::parameter_base> value;
  destination_index index;
  ossia::unit_t unit;

  ossia::value pull() const;
  ossia::net::parameter_base& address()
  {
    return value.get();
  }
  ossia::net::parameter_base& address() const
  {
    return value.get();
  }

  destination(net::parameter_base& v) noexcept;
  destination(net::parameter_base& v, destination_index) noexcept;
  destination(
      net::parameter_base& v, destination_index,
      const ossia::unit_t&) noexcept;
  destination(net::parameter_base& v, const ossia::unit_t&) noexcept;

  destination(const destination& other) noexcept;
  destination(destination&& other) noexcept;
  destination& operator=(const destination&) noexcept;
  destination& operator=(destination&&) noexcept;

  bool operator==(const ossia::value&) const noexcept;
  bool operator!=(const ossia::value&) const noexcept;
  bool operator>(const ossia::value&) const noexcept;
  bool operator>=(const ossia::value&) const noexcept;
  bool operator<(const ossia::value&) const noexcept;
  bool operator<=(const ossia::value&) const noexcept;
};

OSSIA_EXPORT bool operator==(const destination&, const destination&) noexcept;
OSSIA_EXPORT bool operator!=(const destination&, const destination&) noexcept;
OSSIA_EXPORT bool
operator==(const destination&, const ossia::net::parameter_base&) noexcept;
OSSIA_EXPORT bool
operator!=(const destination&, const ossia::net::parameter_base&) noexcept;

OSSIA_EXPORT std::string to_pretty_string(const destination& d) noexcept;
}
