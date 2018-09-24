
#pragma once

#include "device_parameter.hpp"
#include "artnet_protocol.hpp"
#include <artnet/artnet.h>
#include <cstdint>


namespace ossia
{
namespace net
{

  class artnet_parameter : public device_parameter {
    using dmx_buffer = artnet_protocol::dmx_buffer;

    public:
      artnet_parameter(
        net::node_base& node,
        dmx_buffer* buffer,
        const unsigned int channel); 
      ~artnet_parameter();
    
    private:
      void device_update_value() override;

      dmx_buffer& m_buffer;
      const unsigned int m_channel;
  };

}
}