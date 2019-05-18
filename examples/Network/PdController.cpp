#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/common/complex_type.hpp>
int main()
{
  using namespace ossia;
  using namespace ossia::net;
  // Create a device which will listen on the websocket port 5678 and osc port 1234
  generic_device device{
    std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678),
    "my_device"};

  // Create a few float parameters
  struct print_callback {
      ossia::net::parameter_base& p;
      void operator()(const ossia::value& v) {
          std::cout << p.get_node().osc_address() << " : " << v << std::endl;
      }
  };
  auto p0 = create_parameter(device.get_root_node(), "/play", "bool");
  p0->add_callback(print_callback{*p0});
  auto p1 = create_parameter(device.get_root_node(), "/modratio", "float");
  p1->add_callback(print_callback{*p1});
  auto p2 = create_parameter(device.get_root_node(), "/modgain", "float");
  p2->add_callback(print_callback{*p2});
  auto p3 = create_parameter(device.get_root_node(), "/note", "vec2");
  p3->add_callback(print_callback{*p3});
  auto p4 = create_parameter(device.get_root_node(), "/pulse", "impulse");
  p4->add_callback(print_callback{*p4});

  while (true)
  {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(200ms);

    p3->push_value(ossia::make_vec(rand() % 127, rand() % 127));
  }
}
