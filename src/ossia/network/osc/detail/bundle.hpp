#pragma once
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/osc/detail/osc_messages.hpp>
#include <ossia/network/osc/detail/osc_1_0_policy.hpp>
#include <ossia/detail/logger.hpp>
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
    if (val = filter_value(addr, addr.value()); val.valid())
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


template<typename NetworkPolicy>
std::optional<buffer_packed_osc_stream> make_raw_bundle(NetworkPolicy add_element_to_bundle, const std::vector<full_parameter_data> &addresses)
try
{
  auto buffer{std::make_unique<char[]>(max_osc_message_size)};
  oscpack::OutboundPacketStream str(buffer.get(), max_osc_message_size);
  str << oscpack::BeginBundleImmediate();

  ossia::value val;
  for (const auto& addr : addresses)
  {
    add_element_to_bundle(str, val, addr);
  }
  str << oscpack::EndBundle();
  return buffer_packed_osc_stream{std::move(buffer), std::move(str)};
}
catch (const oscpack::OutOfBufferMemoryException&)
{
  ossia::logger().error("make_raw_bundle_client: message too large (limit is {} bytes)", max_osc_message_size);
  return {};
}
catch (const std::runtime_error& e)
{
  ossia::logger().error("make_raw_bundle_client: {}", e.what());
  return {};
}
catch (...)
{
  ossia::logger().error("make_raw_bundle_client: unknown error");
  return {};
}

template<typename NetworkPolicy>
std::optional<buffer_packed_osc_stream> make_bundle(NetworkPolicy add_element_to_bundle, const std::vector<const parameter_base *> &addresses)
try
{
  auto buffer{std::make_unique<char[]>(max_osc_message_size)};
  oscpack::OutboundPacketStream str(buffer.get(), max_osc_message_size);
  str << oscpack::BeginBundleImmediate();

  ossia::value val;
  for (auto a : addresses)
  {
    const ossia::net::parameter_base& addr = *a;
    add_element_to_bundle(str, val, addr);
  }
  str << oscpack::EndBundle();
  return buffer_packed_osc_stream{std::move(buffer), std::move(str)};
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
