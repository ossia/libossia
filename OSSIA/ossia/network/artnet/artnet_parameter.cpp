#include <cstdio>
#include "artnet_parameter.hpp"

namespace ossia
{
namespace net
{

    artnet_parameter::artnet_parameter(
        net::node_base& node,
        dmx_buffer* buffer,
        const unsigned int channel)
    :   device_parameter(
            node, val_type::INT, 
            bounding_mode::CLIP, 
            access_mode::SET,
            make_domain(0, 255)), 
        m_buffer(*buffer), 
        m_channel(channel)
    {
    }

    artnet_parameter::~artnet_parameter()
    {
    }

    void artnet_parameter::device_update_value()
    {
        const int v = value().get<int>();
        m_buffer.data[m_channel] = v;
        m_buffer.dirty = true;
    }
}
}
