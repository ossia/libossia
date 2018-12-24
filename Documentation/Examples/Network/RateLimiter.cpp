// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/rate_limiting_protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/osc/osc.hpp>
#include <iostream>
#include <memory>
#include <functional>


int main()
{
  using namespace std::literals;
  ossia::net::generic_device device{
    ossia::net::limit_output_rate<ossia::net::osc_protocol>(
          std::chrono::milliseconds(500),
          "127.0.0.1", 9996, 9997, std::string("mydevice")),
        "test"};

  auto p = ossia::create_parameter(device, "/foobar", "int");

  ossia::net::generic_device d2{
    std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996, std::string("mydevice")), "foo"};

  auto p_remote = ossia::create_parameter(d2, "/foobar", "int");
  p_remote->add_callback([] (const auto& v) {
    std::cout << "Callback: " << ossia::value_to_pretty_string(v) << std::endl;
  });

  // Push values very fast
  std::atomic_bool running = true;
  std::thread t{[&] {
    int i = 0;
    while (running)
    {
      p->push_value(i++);
    }
  }};

  getchar();
  running = false;
  t.join();
}

