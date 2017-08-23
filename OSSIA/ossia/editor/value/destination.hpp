#pragma once
#include <ossia/detail/destination_index.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/parameter_properties.hpp>
#include <functional>
#include <initializer_list>
#include <memory>
#include <ossia_export.h>
#include <utility>
#include <vector>
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
 * @class Destination destination.hpp ossia/editor/value/value.hpp
 *
 * \see \ref node_base
 * \see \ref destination_index
 */
class OSSIA_EXPORT Destination final
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

  Destination(net::parameter_base& v);
  Destination(net::parameter_base& v, destination_index);
  Destination(net::parameter_base& v, destination_index, const ossia::unit_t&);
  Destination(net::parameter_base& v, const ossia::unit_t&);

  Destination(const Destination& other);
  Destination(Destination&& other);
  Destination& operator=(const Destination&);
  Destination& operator=(Destination&&);

  bool operator==(const ossia::value&) const;
  bool operator!=(const ossia::value&) const;
  bool operator>(const ossia::value&) const;
  bool operator>=(const ossia::value&) const;
  bool operator<(const ossia::value&) const;
  bool operator<=(const ossia::value&) const;
};

OSSIA_EXPORT bool operator==(const Destination&, const Destination&);
OSSIA_EXPORT bool operator!=(const Destination&, const Destination&);
OSSIA_EXPORT bool
operator==(const Destination&, const ossia::net::parameter_base&);
OSSIA_EXPORT bool
operator!=(const Destination&, const ossia::net::parameter_base&);

OSSIA_EXPORT std::string to_pretty_string(const Destination& d);
}
