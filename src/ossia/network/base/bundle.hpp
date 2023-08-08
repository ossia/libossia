#pragma once
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/value/value.hpp>

#include <tcb/span.hpp>

namespace ossia
{
struct bundle_element
{
  ossia::net::parameter_base* parameter{};
  value values;
};
using bundle = tcb::span<bundle_element>;
}
