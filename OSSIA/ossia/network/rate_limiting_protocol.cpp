// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/rate_limiting_protocol.hpp>

namespace ossia::net
{
rate_limiting_protocol::rate_limiting_protocol(
    rate_limiting_protocol::duration d
    , std::unique_ptr<protocol_base> arg)
  : m_duration{d}
  , m_protocol{std::move(arg)}

{
  m_thread = std::thread{[this] {
    while(m_running)
    {
      std::this_thread::sleep_for(m_duration);
      received_value v;
      while(m_queue.try_dequeue(v))
      {
        m_messages[v.address].first = v.value;
      }

      // TODO this is unsafe
      const ossia::net::parameter_base* p{};
      while(m_removedQueue.try_dequeue(p))
      {
        m_messages.erase(p);
      }

      for(auto& v : m_messages)
      {
        m_protocol->push(*v.first, v.second.first);
      }
    }
  }};

}

rate_limiting_protocol::~rate_limiting_protocol()
{
}

bool rate_limiting_protocol::pull(ossia::net::parameter_base& address)
{
  return false;
}

bool rate_limiting_protocol::push(const ossia::net::parameter_base& address, const ossia::value& v)
{
  m_queue.enqueue(received_value{const_cast<ossia::net::parameter_base*>(&address), v});
  return true;
}

bool rate_limiting_protocol::push_raw(const full_parameter_data& address)
{
  return m_protocol->push_raw(address);
}

bool rate_limiting_protocol::observe(
    ossia::net::parameter_base& address, bool enable)
{
  return m_protocol->observe_quietly(address, enable);
}

bool rate_limiting_protocol::update(ossia::net::node_base& node)
{
  return m_protocol->update(node);
}

void rate_limiting_protocol::stop()
{
  return m_protocol->stop();
}

void rate_limiting_protocol::set_device(device_base& dev)
{
  m_device = &dev;
  m_protocol->set_device(dev);
}

void rate_limiting_protocol::parameter_removed(const parameter_base& b)
{
  m_removedQueue.enqueue(&b);
}

}
