#pragma once
#include <ossia/detail/destination_index.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <initializer_list>
#include <memory>
#include <ossia_export.h>
#include <vector>
#include <functional>
#include <utility>
namespace ossia
{
class value;
namespace net
{
class address_base;
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
  std::reference_wrapper<net::address_base> value;
  destination_index index;

  Destination(net::address_base& v);
  Destination(net::address_base& v, destination_index);

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
}
