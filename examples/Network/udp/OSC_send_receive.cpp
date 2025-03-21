#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>

std::atomic_bool done{};
void send_main()
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  using conf = ossia::net::osc_protocol_configuration;
  conf mirror_config{
      .mode = conf::MIRROR,
      .version = conf::OSC1_1,
      .transport = ossia::net::udp_configuration{
          {.local = std::nullopt,
           .remote = ossia::net::outbound_socket_configuration{
               .host = "127.0.0.1", .port = 1234}}}};

  ossia::net::generic_device device{
      ossia::net::make_osc_protocol(ctx, mirror_config), "P"};

  do
  {
    for(int i = 0; i < 10; i++)
    {
      device.get_protocol().push_raw({"/foo", i});
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } while(!done);
}

void receive_main()
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  using conf = ossia::net::osc_protocol_configuration;
  conf host_config{
      .mode = conf::HOST,
      .version = conf::OSC1_1,
      .transport = ossia::net::udp_configuration{
          {.local = ossia::net::inbound_socket_configuration{"0.0.0.0", 1234},
           .remote = std::nullopt}}};

  ossia::net::generic_device device{
      ossia::net::make_osc_protocol(ctx, host_config), "P"};

  auto cb = [&](ossia::string_view v, const ossia::value& val) {
    ossia::logger().info("{} => {}\n", v, val);
  };
  device.on_unhandled_message.connect(&cb);

  ctx->context.run_for(std::chrono::seconds(5));
  done = true;
}

int main()
{
  std::thread receive_thread{receive_main};

  send_main();

  receive_thread.join();
}
