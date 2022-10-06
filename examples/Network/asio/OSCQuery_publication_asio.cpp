#include <ossia/detail/timer.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

namespace
{
void setup_small(ossia::net::network_context& ctx, ossia::net::generic_device& device)
{
  using namespace ossia::net;

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
  ossia::timer timer{ctx.context};
  timer.set_delay(100ms);
  timer.start([&] {
    for(auto param : my_params)
    {
      const auto v = param->value().get<float>();
      param->push_value(3.7f * v * (1.f - v));
    }
  });
  ctx.run();
}

void setup_large(
    ossia::net::network_context& ctx, ossia::net::generic_device& device, int ns_size)
{
  static int64_t pushed_messages{};
  static float current_value{};
  using namespace ossia::net;

  // Create a few float parameters
  std::vector<parameter_base*> my_params;
  for(int i = 0; i < ns_size; i++)
  {
    auto& node = find_or_create_node(device, "/tes t/ fo o." + std::to_string(i));
    auto param = node.create_parameter(ossia::val_type::FLOAT);
    param->push_value(0.1 + 0.01 * i);

    my_params.push_back(param);
  }

  using namespace std::chrono_literals;
  ossia::timer timer{ctx.context};
  timer.set_delay(1ms);
  timer.start([&] {
    for(auto param : my_params)
    {
      param->push_value(current_value += 1.f);
      pushed_messages++;
    }
  });

  ossia::timer print_timer{ctx.context};
  print_timer.set_delay(1000ms);
  print_timer.start([&] { ossia::logger().info("Sent: {} messages", pushed_messages); });
  ctx.run();
}
}

int main(int argc, char** argv)
{
  using namespace ossia::net;
  using namespace std::literals;

  bool large_ns{};
  int ns_size = 1000;
  if(argc >= 2)
  {
    if(argv[1] == "--large"s)
      large_ns = true;

    if(argc >= 3)
      ns_size = strtol(argv[2], nullptr, 10);
  }

  auto ctx = std::make_shared<ossia::net::network_context>();
  // Create a device which will listen on the websocket port 5678 and osc port 1234
  generic_device device{
      std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(ctx, 1234, 5678),
      "my_device"};

  if(large_ns)
    setup_large(*ctx, device, ns_size);
  else
    setup_small(*ctx, device);
}
