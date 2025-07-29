// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/bundle.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/network_logger.hpp>

namespace ossia::net
{
protocol_base::~protocol_base() = default;

network_logger::network_logger() = default;
network_logger::~network_logger() = default;

void protocol_base::request(parameter_base&) { }

bool protocol_base::push(const parameter_base& param, value&& v)
{
  return push(param, (const value&)v);
}

bool protocol_base::push(const parameter_base& p)
{
  return push(p, p.value());
}

bool protocol_base::echo_incoming_message(
    const message_origin_identifier&, const parameter_base&, const value& v)
{
  return true;
}

bool protocol_base::push_bundle(const std::vector<const ossia::net::parameter_base*>& v)
{
  bool b = !v.empty();
  for(auto& addr : v)
  {
    b &= push(*addr);
  }
  return b;
}

bool protocol_base::push_bundle(std::span<ossia::bundle_element> v)
{
  bool b = !v.empty();
  for(auto& [addr, val] : v)
  {
    addr->push_value(val);
  }
  return b;
}

bool protocol_base::push_bundle_bounded(std::span<ossia::bundle_element> v)
{
  return push_bundle(v);
}

bool protocol_base::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& v)
{
  bool b = !v.empty();
  for(auto& addr : v)
  {
    b &= push_raw(addr);
  }
  return b;
}

std::future<void> protocol_base::update_async(node_base& node_base)
{
  // Mock implementation for devices which haven't been ported to async yet
  std::promise<void> promise;
  promise.set_value();
  update(node_base);
  return promise.get_future();
}

void protocol_base::set_device(device_base& dev) { }

bool protocol_base::connected() const noexcept
{
  return true;
}

void protocol_base::connect() { }

std::future<void> protocol_base::pull_async(parameter_base&)
{
  return {};
}

bool protocol_base::publish(const parameter_base&)
{
  return false;
}

bool protocol_base::unpublish(const parameter_base&)
{
  return false;
}

void protocol_base::set_feedback(bool feedback) 
{
}
}
