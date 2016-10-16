#pragma once
#include <memory>

#include <ossia/editor/value/value.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia_export.h>
#include <utility>
#include <bitset>

namespace ossia
{
namespace net
{
class address_base;
}

struct OSSIA_EXPORT destination_qualifiers
{
  ossia::destination_index accessors;
  ossia::unit_t unit;

  bool operator==(const destination_qualifiers &a) const
  {
      return accessors == a.accessors && unit == a.unit;
  }
  
  bool operator!=(const destination_qualifiers &a) const
  {
      return accessors != a.accessors || unit != a.unit;
  }
};

/**
 * @brief The message struct
 *
 * A message allows to store something to send to the network.
 * For instance : a:/b 234
 *
 * \see \ref state_element
 */
struct OSSIA_EXPORT message
{
  ossia::Destination destination;
  ossia::value value;
  ossia::unit_t unit;

  void launch() const;

  friend bool operator==(const message& lhs, const message& rhs)
  {
    return lhs.destination == rhs.destination &&
           lhs.value == rhs.value &&
           lhs.unit == rhs.unit;
  }

  friend bool operator!=(const message& lhs, const message& rhs)
  {
      return lhs.destination != rhs.destination ||
             lhs.value != rhs.value ||
             lhs.unit != rhs.unit;
  }
};

struct OSSIA_EXPORT piecewise_message
{
  std::reference_wrapper<ossia::net::address_base> address;
  Tuple value;
  ossia::unit_t unit;

  void launch() const;

  friend bool operator==(const piecewise_message& lhs, const piecewise_message& rhs)
  {
    return &lhs.address.get() == &rhs.address.get()
           && lhs.value == rhs.value
           && lhs.unit == rhs.unit;
  }

  friend bool operator!=(const piecewise_message& lhs, const piecewise_message& rhs)
  {
    return &lhs.address.get() != &rhs.address.get()
         || lhs.value != rhs.value
         || lhs.unit != rhs.unit;
  }
};

template<std::size_t N>
struct OSSIA_EXPORT piecewise_vec_message
{
  std::reference_wrapper<ossia::net::address_base> address;
  Vec<float, N> value;
  ossia::unit_t unit;
  std::bitset<N> used_values; // True for each value that has been set.

  void launch() const;

  friend bool operator==(const piecewise_vec_message& lhs, const piecewise_vec_message& rhs)
  {
    return &lhs.address.get() == &rhs.address.get()
        && lhs.value == rhs.value
        && lhs.unit == rhs.unit
        && lhs.used_values == rhs.used_values;
  }

  friend bool operator!=(const piecewise_vec_message& lhs, const piecewise_vec_message& rhs)
  {
    return &lhs.address.get() != &rhs.address.get()
        || lhs.value != rhs.value
        || lhs.unit != rhs.unit
        || lhs.used_values != rhs.used_values;
  }
};
}
