
#include "artnet_protocol.hpp"

#include "artnet_parameter.hpp"

#include <artnet/artnet.h>

#include <chrono>

#define ARTNET_NODE_SHORT_NAME "libossia"
#define ARTNET_NODE_LONG_NAME "Libossia Artnet Protocol"

namespace ossia
{
namespace net
{

artnet_protocol::dmx_buffer::dmx_buffer() : dirty(false)
{
  std::memset(data, 0, DMX_CHANNEL_COUNT);
}

int artnet_protocol::dmx_buffer::send(artnet_node node)
{
  return artnet_send_dmx(node, 0, DMX_CHANNEL_COUNT, data);
}

////

artnet_protocol::artnet_protocol(const unsigned int update_frequency)
    : m_running(true), m_update_frequency(update_frequency)
{
  if (update_frequency < 1 || update_frequency > 44)
    throw std::runtime_error(
        "DMX 512 update frequencie must be in the range [0, 44] Hz"); 

  //  44  hz limit apply because we send 512 byte frames.
  //  It seem to be possible to send only some value and thus 
  //   update at higher frequencies => Work TODO

  //  Do not specify ip adress for now, artnet will choose one
  m_node = artnet_new(NULL, 1);

  if (m_node == NULL)
    throw std::runtime_error("Artnet new failed");

  artnet_set_short_name(m_node, ARTNET_NODE_SHORT_NAME);
  artnet_set_long_name(m_node, ARTNET_NODE_LONG_NAME);
  artnet_set_node_type(m_node, ARTNET_RAW);

  if (artnet_start(m_node) != ARTNET_EOK)
    throw std::runtime_error("Artnet Start failed");

  std::printf("ARTNET CTOR OK\n");
}

artnet_protocol::~artnet_protocol()
{
  m_running = false;
  if (m_update_thread.joinable())
    m_update_thread.join();
  artnet_destroy(m_node);
}

void artnet_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;

  auto& root = dev.get_root_node();

  for (unsigned int i = 0; i < DMX_CHANNEL_COUNT; ++i)
    device_parameter::create_device_parameter<artnet_parameter>(
        root, "Channel-" + std::to_string(i + 1), 0, &m_buffer, i);

  m_update_thread = std::thread(update_function, this);
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

void artnet_protocol::update_function(artnet_protocol* instance)
{
  auto& buffer = instance->m_buffer;
  auto node = instance->m_node;
  const auto delay = std::chrono::milliseconds(static_cast<int>(
      1000.0f / static_cast<float>(instance->m_update_frequency)));

  while (instance->m_running)
  {

    if (buffer.dirty)
    {
      std::printf("Updating\n");
      buffer.send(node);
      buffer.dirty = false;
    }

    std::this_thread::sleep_for(delay);
  }
}
}
}
