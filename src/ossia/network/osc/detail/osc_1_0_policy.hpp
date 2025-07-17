#pragma once
#include <ossia/network/osc/detail/osc_common_policy.hpp>
#include <ossia/network/osc/detail/osc_utils.hpp>
#include <ossia/network/value/value.hpp>

#include <boost/endian/conversion.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscTypes.h>

namespace ossia::net
{
// This class is an implementation detail. It is used to send OSC 1.0 things.
// Used for the dynamic std::vector<ossia::value> case
struct osc_1_0_outbound_dynamic_policy : osc_common_outbound_dynamic_policy
{
  using osc_common_outbound_dynamic_policy::operator();
  // Note: infinitum is not in OSC 1.0.
  // But then we have no way to represent an array of impulses, which is
  // *technically* possible in ossia
  void operator()(impulse) const { p << int32_t(0); }

  void operator()(bool b) const { p << int32_t(b); }

  void operator()(char c) const { p << int32_t(c); }

  // Arrays are flattened
  void operator()(const std::vector<value>& t) const
  {
    for(const auto& val : t)
    {
      val.apply(*this);
    }
  }

  void operator()(const value_map_type& t) const
  {
    for(const auto& [k, v] : t)
    {
      (*this)(k);
      v.apply(*this);
    }
  }
};

struct osc_1_0_outbound_stream_visitor : osc_1_0_outbound_dynamic_policy
{
  using osc_1_0_outbound_dynamic_policy::operator();
  void operator()(impulse) const { }

  void operator()(const std::vector<value>& t) const
  {
    // We separate this case because an ossia::impulse type on its own
    // should not have anything but a vector{ossia::impulse, ossia::impulse}
    // should be [00] in osc 1.0
    static_cast<const osc_1_0_outbound_dynamic_policy&>(*this)(t);
  }
};

struct osc_1_0_outbound_static_policy : osc_common_outbound_static_policy
{
  using osc_common_outbound_static_policy::operator();
  std::size_t operator()(char* buffer, ossia::impulse v, const auto&...) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = '\0';
    buffer[2] = '\0';
    buffer[3] = '\0';

    return 4;
  }

  std::size_t operator()(char* buffer, bool v) const noexcept
  {
    return osc_common_outbound_static_policy::operator()(buffer, int32_t{v});
  }

  std::size_t operator()(char* buffer, char v) const noexcept
  {
    return osc_common_outbound_static_policy::operator()(buffer, int32_t{v});
  }
};

struct osc_1_0_policy
{
  using static_policy = osc_1_0_outbound_static_policy;
  using dynamic_policy = osc_1_0_outbound_dynamic_policy;
};

}
