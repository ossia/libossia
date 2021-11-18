#pragma once
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/osc/detail/osc_messages.hpp>
#include <ossia/network/osc/detail/osc_1_0_policy.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/buffer_pool.hpp>
#include <oscpack/osc/OscOutboundPacketStream.h>

#include <optional>


namespace ossia::net
{

template<typename OscPolicy>
struct bundle_common_policy
{
  template<typename Addr_T>
  void operator()(oscpack::OutboundPacketStream& str, ossia::value& val, const Addr_T& addr)
  {
    if (val = bound_value(addr, addr.value()); val.valid())
    {
      str << oscpack::BeginMessageN(osc_address(addr));
      val.apply(typename OscPolicy::dynamic_policy{{str, addr.get_unit()}});
      str << oscpack::EndMessage();
    }
  }
};

template<typename OscPolicy>
struct bundle_client_policy
{
  template<typename Addr_T>
  void operator()(oscpack::OutboundPacketStream& str, ossia::value& val, const Addr_T& addr)
  {
    if (addr.get_access() == ossia::access_mode::GET)
      return;

    bundle_common_policy<OscPolicy>{}(str, val, addr);
  }
};

template<typename OscPolicy>
using bundle_server_policy = bundle_common_policy<OscPolicy>;

static inline auto& access_parameter(const ossia::net::parameter_base* p) { return *p; }
static inline auto& access_parameter(const ossia::net::full_parameter_data& p) { return p; }

struct bundle
{
  ossia::buffer_pool::buffer data;
  bool critical{};
};

template<typename NetworkPolicy, typename Addresses>
std::optional<bundle> make_bundle(NetworkPolicy add_element_to_bundle, const Addresses &addresses)
try
{
  bundle ret{ossia::buffer_pool::instance().acquire(max_osc_message_size), false};
  {
    oscpack::OutboundPacketStream str(ret.data.data(), max_osc_message_size);
    str << oscpack::BeginBundleImmediate();

    ossia::value val;
    for (const auto& a : addresses)
    {
      auto& param = access_parameter(a);
      ret.critical |= param.get_critical();
      add_element_to_bundle(str, val, param);
    }
    str << oscpack::EndBundle();
    ret.data.resize(str.Size());

    // TODO useless condition for now.
    // But if we know that we are going through ws we can increase the size beyond 65k.
    // ret.critical |= str.Size() > max_osc_message_size;
  }
  return ret;
}
catch (const oscpack::OutOfBufferMemoryException&)
{
  ossia::logger().error("make_bundle_client: message too large (limit is {} bytes)", max_osc_message_size);
  return {};
}
catch (const std::runtime_error& e)
{
  ossia::logger().error("make_bundle_client: {}", e.what());
  return {};
}
catch (...)
{
  ossia::logger().error("make_bundle_client: unknown error");
  return {};
}

}
