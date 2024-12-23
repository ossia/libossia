#include <ossia/detail/config.hpp>

#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>

#include <functional>
#include <memory>

int main(int argc, char** argv)
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  using conf = ossia::net::osc_protocol_configuration;
  ossia::net::generic_device device{
      ossia::net::make_osc_protocol(
          ctx, {conf::HOST, conf::OSC1_1, conf::SLIP, conf::NEVER_BUNDLE,
                ossia::net::ws_server_configuration{1234}}),
      "P"};

  auto cb = [&](ossia::string_view v, const ossia::value& val) {
    ossia::logger().info("{} => {}\n", v, ossia::value_to_pretty_string(val));
    device.get_protocol().push_raw({"/ok", ossia::value{true}});
  };
  device.on_unhandled_message.connect(&cb);

  ctx->run();
}
