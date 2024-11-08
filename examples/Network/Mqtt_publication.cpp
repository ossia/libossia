#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/protocols/mqtt/mqtt_protocol.hpp>

#include <boost/asio/post.hpp>

#include <iostream>
#include <memory>

void do_post(boost::asio::io_context& ctx, ossia::net::parameter_base& param)
{
  boost::asio::post(ctx, [&ctx, &param] {
    param.push_value("oy! " + std::to_string(rand() % 10));

    do_post(ctx, param);
  });
};

int main(int argc, char** argv)
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  ossia::net::mqtt5_configuration conf{
      ossia::net::tcp_client_configuration{{.host = "127.0.0.1", .port = 1883}}};
  auto proto = std::make_unique<ossia::net::mqtt5_protocol>(ctx, conf);
  ossia::net::generic_device device{std::move(proto), "P"};

  // libossia v3: we really need to prevent live modification of the tree
  // and have an api to update it async
  ossia::create_parameter(device.get_root_node(), "/foo", "string")
      ->set_access(ossia::access_mode::GET)
      .add_callback([](const ossia::value& val) {
    std::cerr << "Received: " << ossia::value_to_pretty_string(val) << "\n";
  });
  auto& bar = ossia::create_parameter(device.get_root_node(), "/bar", "string")
                  ->set_access(ossia::access_mode::SET);

  do_post(ctx->context, bar);
  ctx->run();
}
