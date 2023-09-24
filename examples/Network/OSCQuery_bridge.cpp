#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

int main()
{
  using namespace ossia::net;
  // Create a device which will listen on the websocket port 5678 and osc port 1234

  auto multiplex = std::make_unique<ossia::net::multiplex_protocol>();
  multiplex->expose_to(std::make_unique<ossia::oscquery::oscquery_server_protocol>());
  multiplex->expose_to(std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 5567, 5568));

  generic_device device{std::move(multiplex), "my_device"};
  device.set_echo(true);

  // Create a few float parameters
  std::vector<parameter_base*> my_params;
  for(int i = 0; i < 10; i++)
  {
    auto& node = find_or_create_node(device, "/test/foo." + std::to_string(i));
    auto param = node.create_parameter(ossia::val_type::FLOAT);
    param->push_value(0.1 + 0.01 * i);

    my_params.push_back(param);
  }

  while(true)
  {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);
  }
}
