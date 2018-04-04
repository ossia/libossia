#include <ossia/network/leapmotion/leapmotion_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

int main()
{
  auto proto = std::make_unique<ossia::net::multiplex_protocol>();
  auto& multiplex = *proto;
  ossia::net::generic_device leap{std::move(proto), "leapmotion"};

  multiplex.expose_to(std::make_unique<ossia::oscquery::oscquery_server_protocol>(15554, 1349));
  multiplex.expose_to(std::make_unique<ossia::leapmotion_protocol>());

  while(true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
