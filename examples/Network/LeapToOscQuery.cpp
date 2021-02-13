#include <ossia/protocols/leapmotion/leapmotion_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

int main()
{
  auto ctx = ossia::net::create_network_context();
  auto proto = std::make_unique<ossia::net::multiplex_protocol>();
  auto& multiplex = *proto;
  ossia::net::generic_device leap{std::move(proto), "leapmotion"};

  multiplex.expose_to(std::make_unique<ossia::oscquery::oscquery_server_protocol>(15554, 1349));
  multiplex.expose_to(std::make_unique<ossia::leapmotion_protocol>(ctx));

  ossia::net::run_network_context(*ctx);
}
