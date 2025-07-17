#pragma once
#include <ossia/network/osc/detail/osc_common_policy.hpp>
#include <ossia/network/osc/detail/osc_utils.hpp>
#include <ossia/network/value/value.hpp>

#include <boost/endian/conversion.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscTypes.h>

// OSC 1.1 adds T, F, I, N
namespace ossia::net
{

struct osc_1_1_outbound_array_policy : osc_common_outbound_dynamic_policy
{
  using osc_common_outbound_dynamic_policy::operator();
  void operator()(impulse) const { p << oscpack::Infinitum(); }

  void operator()(bool b) const { p << b; }

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

struct osc_1_1_outbound_value_policy : osc_common_outbound_static_policy
{
  using osc_common_outbound_static_policy::operator();
  std::size_t operator()(
      char* buffer, ossia::impulse v, const ossia::extended_type& t) const noexcept
  {
    // NOTE: this is a change wrt the old ossia::oscquery::osc_outbound_visitor
    buffer[0] = ',';
    if(t.empty())
      buffer[1] = oscpack::INFINITUM_TYPE_TAG;
    else if(t == "nil")
      buffer[1] = oscpack::NIL_TYPE_TAG;
    else if(t == "empty")
      buffer[1] = '\0';
    buffer[2] = '\0';
    buffer[3] = '\0';

    return 4;
  }

  std::size_t operator()(char* buffer, ossia::impulse v) const noexcept
  {
    // NOTE: this is a change wrt the old ossia::oscquery::osc_outbound_visitor
    buffer[0] = ',';
    buffer[1] = '\0';
    buffer[2] = '\0';
    buffer[3] = '\0';

    return 4;
  }

  std::size_t operator()(char* buffer, bool v) const noexcept
  {
    buffer[0] = ',';
    buffer[1] = v ? oscpack::TRUE_TYPE_TAG : oscpack::FALSE_TYPE_TAG;
    buffer[2] = '\0';
    buffer[3] = '\0';

    return 4;
  }

  std::size_t operator()(char* buffer, char v) const noexcept
  {
    return osc_common_outbound_static_policy::operator()(buffer, int32_t{v});
  }
};

struct osc_1_1_policy
{
  using static_policy = osc_1_1_outbound_value_policy;
  using dynamic_policy = osc_1_1_outbound_array_policy;
};

}
