#include <ossia/network/local/local.hpp>
#include <ossia/protocols/joystick/joystick_protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/value/format_value.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/detail/fmt.hpp>

int main()
{
  auto joysticks = ossia::net::joystick_info::get_joystick_count();
  if(joysticks == 0)
  {
    fmt::print("No joystick detected - exiting.\n");
    return 1;
  }

  fmt::print("Using joystick {}\n", ossia::net::joystick_info::get_joystick_name(0));

  auto ctx = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device source_dev{
    std::make_unique<ossia::net::multiplex_protocol>(
          std::make_unique<ossia::net::joystick_protocol>(ctx, 0, 0),
          std::make_unique<ossia::oscquery::oscquery_server_protocol>(5579, 5589)
    ),
    "joystick"
  };

  auto on_message = [] (const ossia::net::parameter_base& param) {
    fmt::print("{}: {}\n", ossia::net::osc_address(param), param.value());
  };
  source_dev.on_message.connect(on_message);
  ctx->run();
}
