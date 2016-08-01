#pragma once
#include <memory>

#include <ossia/editor/value/value.hpp>
#include <ossia_export.h>
#include <utility>

namespace ossia
{
namespace net
{
class address_base;
}
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
  std::reference_wrapper<ossia::net::address_base> address;
  ossia::value value;

  void launch() const;

  friend bool operator==(const message& lhs, const message& rhs)
  {
    return &lhs.address.get() == &rhs.address.get()
           && &lhs.value == &rhs.value;
  }

  friend bool operator!=(const message& lhs, const message& rhs)
  {
    return &lhs.address.get() != &rhs.address.get()
           || &lhs.value != &rhs.value;
  }
};
}
