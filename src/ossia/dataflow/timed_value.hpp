#pragma once
#include <ossia/network/value/value.hpp>

namespace ossia
{
struct timed_value
{
  explicit timed_value(ossia::value&& v) : value{std::move(v)}
  {
  }
  explicit timed_value(const ossia::value& v) : value{v}
  {
  }
  explicit timed_value(ossia::value&& v, int64_t ts)
      : value{std::move(v)}, timestamp{ts}
  {
  }
  explicit timed_value(const ossia::value& v, int64_t ts)
      : value{v}, timestamp{ts}
  {
  }

  timed_value() = default;
  ~timed_value() = default;
  timed_value(const timed_value&) = default;
  timed_value(timed_value&&) = default;
  timed_value& operator=(const timed_value&) = default;
  timed_value& operator=(timed_value&&) = default;

  ossia::value value{};
  int64_t timestamp{};
};
}
