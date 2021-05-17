// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <catch.hpp>

#include <iostream>

#include <ossia/context.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>

#include "TestUtils.hpp"
#include "ProtocolTestUtils.hpp"

using namespace ossia;
using namespace ossia::net;

#if defined(OSSIA_PROTOCOL_OSC)
#include <ossia/network/osc/osc.hpp>
TEST_CASE ("test_oscquery_osc_out", "test_oscquery_osc_out")
{
  uint16_t shared = 9996;
  auto ctx = std::make_shared<ossia::net::network_context>();
  using conf = ossia::net::osc_protocol_configuration;

  auto osc = ossia::net::make_osc_protocol(
      ctx,
      {
        conf::HOST,
        conf::OSC1_1,
        conf::SIZE_PREFIX,
        ossia::net::udp_configuration{{{"0.0.0.0", 9998}, {"127.0.0.1", shared}}}
      }
      );
  auto oscquery = std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(ctx);

  generic_device device {
    std::make_unique<ossia::net::multiplex_protocol>(osc, oscquery),
      "my_device"
  };

  device.set_echo(true);

  ossia::net::generic_device remote{
    ossia::net::make_osc_protocol(
        ctx,
        {
          conf::MIRROR,
          conf::OSC1_1,
          conf::SIZE_PREFIX,
          ossia::net::udp_configuration{{{"0.0.0.0", shared}, {"127.0.0.1", 9997}}}
        }
        ),
      "test_osc"};


  std::string address; 
  ossia::value recv; 
  auto on_server_message = [&] (const std::string& s, const ossia::value& v) {
    address = s;
    recv = v;
  };
  remote.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  //test BI
  auto& bi = find_or_create_node(device, "/foo/bi");
  auto bip = bi.create_parameter(ossia::val_type::INT);
  bi.set(access_mode_attribute{}, access_mode::BI);
  bip->push_value(2084);

  ctx->context.run_one();
  REQUIRE(address == "/foo/bi");
  REQUIRE(recv == ossia::value { 2084 });

  //test GET
  address = std::string();
  auto& get = find_or_create_node(device, "/foo/get");
  auto getp = get.create_parameter(ossia::val_type::INT);
  get.set(access_mode_attribute{}, access_mode::GET);
  getp->push_value(42);

  ctx->context.run_one();
  REQUIRE(address == "/foo/get");
  REQUIRE(recv == ossia::value { 42 });

  //test SET
  address = std::string();
  auto& set = find_or_create_node(device, "/foo/set");
  auto setp = set.create_parameter(ossia::val_type::INT);
  set.set(access_mode_attribute{}, access_mode::SET);
  setp->push_value(1000);

  //shouldn't get anything in the remote
  ctx->context.run_for(std::chrono::milliseconds(100));
  REQUIRE(address.size() == 0);
  REQUIRE(recv == ossia::value { 42 }); //stays the same
}
#endif
