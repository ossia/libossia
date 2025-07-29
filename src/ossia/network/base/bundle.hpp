#pragma once
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/value/value.hpp>

#include <span>

namespace ossia
{
struct bundle_element
{
  ossia::net::parameter_base* parameter{};
  value values;
};
using bundle = std::span<bundle_element>;
}
