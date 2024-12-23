#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>

#include <boost/lexical_cast.hpp>

#include <functional>
#include <iostream>
#include <memory>

int main(int argc, char** argv)
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  using conf = ossia::net::osc_protocol_configuration;
  ossia::net::generic_device device{
      ossia::net::make_osc_protocol(
          ctx, {conf::MIRROR, conf::OSC1_1, conf::SLIP, conf::NEVER_BUNDLE,
                ossia::net::unix_dgram_configuration{
                    {ossia::net::receive_fd_configuration{{"/tmp/ossia_echo.a.socket"}},
                     ossia::net::send_fd_configuration{{"/tmp/ossia_echo.b.socket"}}}}}),
      "P"};

  ossia::net::full_parameter_data dat;

  device.get_protocol().push_raw({"/foo", std::vector<ossia::value>{123, 4.56}});
}
