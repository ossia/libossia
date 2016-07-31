#pragma once
#include <ossia_export.h>
#include <vector>
#include <memory>
#include <initializer_list>
#include <ossia/detail/destination_index.hpp>
#include <ossia/network/common/address_properties.hpp>
namespace ossia
{
class value;
namespace net
{
class node_base;
}
/*! \details Destination to an Address value and optionnally to several index of this value */
class OSSIA_EXPORT Destination final
{
  public:
    net::node_base* value{};
    destination_index index;

    Destination();
    Destination(net::node_base& v);
    Destination(net::node_base& v, destination_index);

    Destination(const Destination& other);
    Destination(Destination&& other);
    Destination& operator=(const Destination&);
    Destination& operator=(Destination&&);

    bool operator== (const ossia::value&) const;
    bool operator!= (const ossia::value&) const;
    bool operator>  (const ossia::value&) const;
    bool operator>= (const ossia::value&) const;
    bool operator<  (const ossia::value&) const;
    bool operator<= (const ossia::value&) const;
};

}
