#include <ossia/network/generic/generic_device.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/detail/timer.hpp>

int main()
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();
  // Create a device which will listen on the websocket port 5678 and osc port 1234
  generic_device device{
    std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(ctx, 1234, 5678),
    "my_device"};

  // Create a few float parameters
  std::vector<parameter_base*> my_params;
  for(int i = 0; i < 10; i++)
  {
    auto& node = find_or_create_node(device, "/tes t/ fo o." + std::to_string(i));
    auto param = node.create_parameter(ossia::val_type::FLOAT);
    param->push_value(0.1 + 0.01 * i);

    my_params.push_back(param);
  }

  using namespace std::chrono_literals;
  ossia::timer timer{ctx->context};
  timer.set_delay(100ms);
  timer.start([&] {
    for(auto param : my_params)
    {
      const auto v = param->value().get<float>();
      param->push_value(3.7f * v * (1.f - v));
    }
  });
  ctx->run();
}
