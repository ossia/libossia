
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

    public:
      artnet_parameter(
        net::node_base& node,
        artnet_node artnet_node,
        const unsigned int channel); 
      ~artnet_parameter();
    
    private:
      void device_update_value() override;

      artnet_node m_node;
      const unsigned int m_channel;
  };

}
}