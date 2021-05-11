#include <ossia/detail/config.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <memory>
#include <functional>

int main(int argc, char** argv)
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  using conf = ossia::net::osc_protocol_configuration;
  ossia::net::generic_device device{
    ossia::net::make_osc_protocol(ctx,
          {
            conf::UNIX_DGRAM,
            conf::HOST,
            conf::OSC1_1,
            conf::SLIP,
            ossia::net::fd_configuration{"/tmp/ossia_echo.client.socket","/tmp/ossia_echo.server.socket"}
          }),
        "P"};

  auto cb = [&] (ossia::string_view v, const ossia::value& val) {
    std::cout << v << " => " << ossia::value_to_pretty_string(val) << std::endl;
  };
  device.on_unhandled_message.connect(&cb);

  ctx->run();
}
