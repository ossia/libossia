// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/logger.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/local/local.hpp>

using lock_guard = std::lock_guard<ossia::audio_spin_mutex>;

namespace ossia::net
{
static void observe_rec(protocol_base& proto, ossia::net::node_base& n)
{
  for(auto& cld : n.children())
  {
    if(auto addr = cld->get_parameter())
    {
      if(!addr->callbacks_empty())
      {
        proto.observe_quietly(*addr, true);
      }
    }
    observe_rec(proto, *cld);
  }
}

multiplex_protocol::multiplex_protocol()
    : protocol_base{}
{
}

multiplex_protocol::~multiplex_protocol()
{

}

bool multiplex_protocol::pull(ossia::net::parameter_base& address)
{
  return false;
}

bool multiplex_protocol::push(
    const ossia::net::parameter_base& address, const ossia::value& v)
{
  bool b = true;
  lock_guard guard(m_protocols_mutex);
  for(auto& proto : m_protocols)
    b &= proto->push(address, v);

  return b;
}

bool multiplex_protocol::push_raw(const full_parameter_data& dat)
{
  bool b = true;
  lock_guard guard(m_protocols_mutex);
  for(auto& proto : m_protocols)
    b &= proto->push_raw(dat);

  return b;
}

bool multiplex_protocol::observe(ossia::net::parameter_base& address, bool enable)
{
  bool b = true;
  lock_guard guard(m_protocols_mutex);
  for(auto& proto : m_protocols)
    b &= proto->observe_quietly(address, enable);

  return b;
}

bool multiplex_protocol::update(ossia::net::node_base& node)
{
  return false;
}

bool multiplex_protocol::echo_incoming_message(
    const message_origin_identifier& id, const parameter_base& param, const value& v)
{
  bool b = true;
  lock_guard guard(m_protocols_mutex);
  for(auto& proto : m_protocols)
    b &= proto->echo_incoming_message(id, param, v);
  return b;
}

void multiplex_protocol::stop()
{
  lock_guard guard(m_protocols_mutex);
  for(auto& proto : m_protocols)
    proto->stop();
}

void multiplex_protocol::set_device(device_base& dev)
{
  m_device = &dev;
  lock_guard guard(m_protocols_mutex);
  for(auto& p : m_protocols_to_register)
  {
    p->set_device(*m_device);

    // Expose all the addresses with callbacks
    observe_rec(*p, m_device->get_root_node());

    m_protocols.push_back(std::move(p));
  }
  m_protocols_to_register.clear();
}

void multiplex_protocol::expose_to(std::unique_ptr<protocol_base> p)
{
  if(p)
  {
    if(!p->test_flag(SupportsMultiplex))
    {
      auto& proto = *p;
#if defined(__cpp_rtti)
      ossia::logger().error(
          "Cannot multiplex a protocol of type: {}", typeid(proto).name());
#endif
      return;
    }

    lock_guard guard(m_protocols_mutex);
    if(m_device)
    {
      p->set_device(*m_device);

      // Expose all the addresses with callbacks
      observe_rec(*p, m_device->get_root_node());

      m_protocols.push_back(std::move(p));
    }
    else
    {
      m_protocols_to_register.push_back(std::move(p));
    }
  }
}

void multiplex_protocol::stop_expose_to(const protocol_base& p)
{
  lock_guard guard(m_protocols_mutex);
  ossia::remove_erase_if(
      m_protocols_to_register, [&](const auto& ptr) { return ptr.get() == &p; });
  ossia::remove_erase_if(m_protocols, [&](const auto& ptr) { return ptr.get() == &p; });
}

void ossia::net::multiplex_protocol::clear()
{
  lock_guard guard(m_protocols_mutex);
  m_protocols.clear();
}
}
