// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>

namespace ossia
{
namespace net
{
multiplex_protocol::multiplex_protocol()
{
}

multiplex_protocol::~multiplex_protocol()
{
}

bool multiplex_protocol::pull(ossia::net::parameter_base& address)
{
  return false;
}

bool multiplex_protocol::push(const ossia::net::parameter_base& address)
{
  bool b = true;
  for (auto& proto : m_protocols)
    b &= proto->push(address);

  return b;
}

bool multiplex_protocol::push_raw(const full_parameter_data& dat)
{
  bool b = true;
  for (auto& proto : m_protocols)
    b &= proto->push_raw(dat);

  return b;
}

bool multiplex_protocol::observe(
    ossia::net::parameter_base& address, bool enable)
{
  bool b = true;
  for (auto& proto : m_protocols)
    b &= proto->observe_quietly(address, enable);

  return b;
}

bool multiplex_protocol::update(ossia::net::node_base& node)
{
  return false;
}

void multiplex_protocol::stop()
{
  for(auto& proto : m_protocols)
    proto->stop();
}

void multiplex_protocol::set_device(device_base& dev)
{
  m_device = &dev;
}

static void observe_rec(protocol_base& proto, ossia::net::node_base& n)
{
  for (auto& cld : n.children())
  {
    if (auto addr = cld->get_parameter())
    {
      if (!addr->callbacks_empty())
      {
        proto.observe_quietly(*addr, true);
      }
    }
    observe_rec(proto, *cld);
  }
}

void multiplex_protocol::expose_to(std::unique_ptr<protocol_base> p)
{
  if (p)
  {
    p->set_device(*m_device);

    // Expose all the adresses with callbacks
    observe_rec(*p, m_device->get_root_node());

    m_protocols.push_back(std::move(p));
  }
}

void multiplex_protocol::stop_expose_to(const protocol_base& p)
{
  m_protocols.erase(
      ossia::remove_if(
          m_protocols, [&](const auto& ptr) { return ptr.get() == &p; }),
              m_protocols.end());
}

void ossia::net::multiplex_protocol::clear()
{
  m_protocols.clear();
}

}
}
