
#include "artnet_protocol.hpp"
#include <chrono>

#define ARTNET_NODE_SHORT_NAME "libossia"
#define ARTNET_NODE_LONG_NAME "Libossia Artnet Protocol"

namespace ossia
{
namespace net
{

////

artnet_protocol::artnet_protocol()
{
  //  Initialize channels values buffer
  m_channel_value.fill(0u);

  //  Do not specify ip adress for now, will choose one
  m_node = artnet_new(NULL, 1);

  artnet_set_short_name(ARTNET_NODE_SHORT_NAME);
  artnet_set_long_name(ARTNET_NODE_LONG_NAME)

}

artnet_protocol::~artnet_protocol()
{
}

void artnet_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;

  auto& root = dev.get_root_node();

  m_update_thread = 
    std::thread(update_loop, this);
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

void artnet_protocol::update_loop(artnet_protocol *instance)
{
  auto& artnet = *instance;

  for(;;) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::printf("DMX Update\n");
/*
    // Reading ArtNet available data
    artnet_read(artnet.m_node, 0);

    // Sending dmx datas to all channels
    artnet_send_dmx(
      artnet.m_node, 0, 
      artnet.m_channel_value.size(), 
      &(artnet.m_channel_value[0]));
*/
  }
}

}
}

