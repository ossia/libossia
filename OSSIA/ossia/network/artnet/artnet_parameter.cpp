#include <cstdio>
#include "artnet_parameter.hpp"

namespace ossia
{
namespace net
{

    artnet_parameter::artnet_parameter(
        net::node_base& node,
        artnet_node artnet_node,
        const unsigned int channel)
    :   device_parameter(
            node, val_type::INT, 
            bounding_mode::CLIP, 
            access_mode::SET,
            make_domain(0, 255)), 
        m_node(artnet_node), m_channel(channel)
    {
    }

    artnet_parameter::~artnet_parameter()
    {
    }

    void artnet_parameter::device_update_value()
    {
        uint8_t data[DMX_CHANNEL_COUNT] = {0u};
        const int v = value().get<int>();
        
        std::printf("Sending channel %u = %d\n", m_channel, v);

        data[m_channel] = 
            static_cast<uint8_t>(v);

        artnet_send_dmx(
            m_node, 0, DMX_CHANNEL_COUNT, data);

    }
}
}
