
#include <artnet/artnet.h>

#include "artnet_protocol.hpp"
#include "artnet_parameter.hpp"

#define ARTNET_NODE_SHORT_NAME "libossia"
#define ARTNET_NODE_LONG_NAME "Libossia Artnet Protocol"

namespace ossia
{
namespace net
{

////

artnet_protocol::artnet_protocol()
{
  //  Do not specify ip adress for now, will choose one
  m_node = artnet_new(NULL, 1);

  if (m_node == NULL)
    throw std::runtime_error("Artnet new failed");

  artnet_set_short_name(m_node, ARTNET_NODE_SHORT_NAME);
  artnet_set_long_name(m_node, ARTNET_NODE_LONG_NAME);
  artnet_set_node_type(m_node, ARTNET_RAW);

  artnet_set_port_type(m_node, 0, ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX);
  artnet_set_port_addr(m_node, 0, ARTNET_INPUT_PORT, 0);

  if (artnet_start(m_node) != ARTNET_EOK)
    throw std::runtime_error("Artnet Start failed");
}

artnet_protocol::~artnet_protocol()
{
  artnet_destroy(m_node);
}

void artnet_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;

  auto& root = dev.get_root_node();

  //for (unsigned int i = 0; i < DMX_CHANNEL_COUNT; ++i)
  //  device_parameter::create_device_parameter<artnet_parameter>(
  //    root, "Channel-" + std::to_string(i+1), 255, m_node, i);

  const unsigned int i = 19;
  device_parameter::create_device_parameter<artnet_parameter>(
      root, "Channel-" + std::to_string(i+1), 255, m_node, i);
}

bool artnet_protocol::pull(net::parameter_base& param)
{
  return true;
}

bool artnet_protocol::push(const net::parameter_base& param)
{
  return true;
}

bool artnet_protocol::observe(net::parameter_base& param, bool enable)
{
  return false;
}

bool artnet_protocol::push_raw(const ossia::net::full_parameter_data& data)
{
  return false;
}

bool artnet_protocol::update(ossia::net::node_base&)
{
  return true;
}


}
}

