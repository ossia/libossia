#pragma once
#include <ossia/network/value/value.hpp>
namespace ossia
{
struct value_port;
struct OSSIA_EXPORT control_message
{
  ossia::value_port* port{};
  ossia::value value;

  void launch();

  friend bool operator==(const control_message& lhs, const control_message& rhs) noexcept
  {
    return lhs.port == rhs.port && lhs.value == rhs.value;
  }

  friend bool operator!=(const control_message& lhs, const control_message& rhs) noexcept
  {
    return !(lhs == rhs);
  }

};

}
