#pragma once
#include <ossia/detail/string_view.hpp>
#include <ossia/network/common/parameter_properties.hpp>

#include <ossia/detail/nullable_variant.hpp>
namespace ossia
{
struct unit_t;
class value;
using extended_type = std::string;

namespace net
{
class node_base;
class parameter_base;
struct parameter_data;
}

/**
 * @typedef Used when an external information can be mapped to multiple levels
 * of typing.
 *
 * For instance OSC typetags can provide information such as one int, three
 * floats, RGB color...
 *
 * TODO maybe replace val_type by small_vector<val_type> ?
 */
using complex_type = ossia::nullable_variant<ossia::val_type, ossia::unit_t, ossia::extended_type>;

//! Get the implementation type of a high level type. If unknown will be
//! ossia::impulse.
OSSIA_EXPORT
ossia::val_type underlying_type(const complex_type& t);

//! Creates a parameter in a node according to a complex type.
OSSIA_EXPORT
ossia::net::parameter_base*
setup_parameter(const complex_type& t, ossia::net::node_base& node);

//! Creates a parameter in a node according to heuristics
OSSIA_EXPORT
ossia::net::parameter_base*
try_setup_parameter(std::string t, ossia::net::node_base& node);

OSSIA_EXPORT
ossia::net::parameter_base* create_parameter(
    ossia::net::node_base& parent, std::string name, std::string type);

//! Change the type of a parameter according to a complex type.
OSSIA_EXPORT
void update_parameter_type(
    const complex_type& t, ossia::net::parameter_base& node);

OSSIA_EXPORT
const ossia::net::parameter_data*
default_parameter_for_type(std::string_view type);

OSSIA_EXPORT
ossia::value convert(
    const ossia::value& v, const ossia::complex_type& source_t,
    const ossia::complex_type& dest_t);
}
