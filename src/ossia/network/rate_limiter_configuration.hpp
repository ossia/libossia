#pragma once
#include <ossia/detail/config.hpp>

#include <chrono>

namespace ossia::net
{
struct rate_limiter_configuration
{
  using clock = std::chrono::steady_clock;

  // What is the rate limit
  std::chrono::milliseconds duration{};

  // Put things in e.g. OSC bundles
  bool bundle{};

  // When sending, send every parameters from the device, not just the last ones
  bool send_all{};

  // Always send the last sent values on every tick
  bool repeat{};
};

}
