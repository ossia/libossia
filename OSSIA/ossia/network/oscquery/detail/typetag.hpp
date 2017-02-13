#pragma once
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/extended_types.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/value/value.hpp>
#include <oscpack/osc/OscTypes.h>
namespace ossia
{
namespace oscquery
{
using complex_type = eggs::variant<ossia::val_type, ossia::unit_t, ossia::net::extended_type>;

OSSIA_EXPORT std::string get_osc_typetag_impl(const ossia::net::address_base& addr);

OSSIA_EXPORT optional<std::string> get_osc_typetag(const ossia::net::node_base& n);

OSSIA_EXPORT complex_type get_type_from_osc_typetag(ossia::string_view str);

OSSIA_EXPORT ossia::net::address_base*
setup_address(const complex_type& t, ossia::net::node_base& node);

OSSIA_EXPORT void set_osc_typetag(ossia::net::node_base& n, ossia::string_view tag);
}
}
