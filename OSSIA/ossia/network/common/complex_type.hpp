#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/network/common/extended_types.hpp>
#include <eggs/variant.hpp>

namespace ossia
{
namespace net { class node_base; }

/**
 * @typedef Used when an external information can be mapped to multiple levels of typing.
 *
 * For instance OSC typetags can provide information such as one int, three floats, RGB color...
 *
 * TODO maybe replace val_type by small_vector<val_type> ?
 */
using complex_type =
  eggs::variant<
    ossia::val_type,
    ossia::unit_t,
    ossia::extended_type
  >;

//! Get the implementation type of a high level type. If unknown will be ossia::impulse.
OSSIA_EXPORT
ossia::val_type underlying_type(const complex_type& t);

//! Creates an address in a node according to a complex type.
OSSIA_EXPORT
ossia::net::address_base* setup_address(
    const complex_type& t,
    ossia::net::node_base& node);

//! Change the type of an address according to a complex type.
OSSIA_EXPORT
void update_address_type(
    const complex_type& t,
    ossia::net::address_base& node);

}
