#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/extended_types.hpp>
#include <ossia/detail/any_map.hpp>

namespace ossia
{
/**
 * @typedef Used when an external information can be mapped to multiple levels of typing.
 *
 * For instance OSC typetags can provide information such as one int, three floats, RGB color...
 *
 * TODO maybe replace val_type by small_vector<val_type> ?
 */
using complex_type = eggs::variant<ossia::val_type, ossia::unit_t, ossia::net::extended_type>;

//! Get the implementation type of a high level type. If unknown will be ossia::impulse.
OSSIA_EXPORT ossia::val_type underlying_type(const complex_type& t);

namespace net
{
/**
 * @brief The data that can be found inside an address
 *
 * Used to create a new address / node.
 * Unlike address_base and its child classes, can be copied, moved, etc.
 */
struct address_data
{
  address_data() = default;
  address_data(const address_data&) = default;
  address_data(address_data&&) = default;
  address_data& operator=(const address_data&) = default;
  address_data& operator=(address_data&&) = default;

  address_data(std::string n):
    node_name{std::move(n)}
  {

  }

  std::string node_name;

  ossia::value value;
  ossia::complex_type type;
  ossia::optional<ossia::net::domain> domain;
  ossia::optional<ossia::access_mode> access;
  ossia::optional<ossia::bounding_mode> bounding;
  ossia::optional<ossia::repetition_filter> repetition_filter;
  unit_t unit;

  extended_attributes extended;

  operator const ossia::extended_attributes&() const { return extended; }
  operator ossia::extended_attributes&() { return extended; }
};

}
}
