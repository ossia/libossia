// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProtocolTestUtils.hpp"
#include "TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

#include "include_catch.hpp"

#include <iostream>
#include <thread>

using namespace ossia;
using namespace ossia::net;

#if defined(OSSIA_PROTOCOL_OSC)
#include <ossia/network/osc/osc.hpp>
TEST_CASE("test_oscquery_osc_out", "test_oscquery_osc_out")
{
  uint16_t shared = 9996;
  auto ctx = std::make_shared<ossia::net::network_context>();
  using conf = ossia::net::osc_protocol_configuration;

  auto osc = ossia::net::make_osc_protocol(
      ctx, {.mode = conf::HOST,
            .version = conf::OSC1_1,
            .transport = ossia::net::udp_configuration{
                {.local = std::nullopt,
                 .remote = ossia::net::outbound_socket_configuration{
                     .host = "127.0.0.1", .port = shared}}}});
  auto oscquery = std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(ctx);

  auto multiplex = std::make_unique<ossia::net::multiplex_protocol>();
  multiplex->expose_to(std::move(osc));
  multiplex->expose_to(std::move(oscquery));
  generic_device device{std::move(multiplex), "my_device"};

  device.set_echo(true);

  const auto udp_conf = ossia::net::udp_configuration{
      {.local
       = ossia::net::inbound_socket_configuration{.bind = "0.0.0.0", .port = shared},
       .remote = std::nullopt}};
  ossia::net::generic_device remote{
      ossia::net::make_osc_protocol(
          ctx,
          {.mode = conf::MIRROR,
           .version = conf::OSC1_1,
           .transport = udp_conf}),
      "test_osc"};

  std::string address;
  ossia::value recv;
  auto on_server_message = [&](const std::string& s, const ossia::value& v) {
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
  REQUIRE(recv == ossia::value{2084});

  //test GET
  address = std::string();
  auto& get = find_or_create_node(device, "/foo/get");
  auto getp = get.create_parameter(ossia::val_type::INT);
  get.set(access_mode_attribute{}, access_mode::GET);
  getp->push_value(42);

  ctx->context.run_one();
  REQUIRE(address == "/foo/get");
  REQUIRE(recv == ossia::value{42});

  //test SET
  address = std::string();
  auto& set = find_or_create_node(device, "/foo/set");
  auto setp = set.create_parameter(ossia::val_type::INT);
  set.set(access_mode_attribute{}, access_mode::SET);
  setp->push_value(1000);

  //shouldn't get anything in the remote
  ctx->context.run_for(std::chrono::milliseconds(100));
  REQUIRE(address.size() == 0);
  REQUIRE(recv == ossia::value{42}); //stays the same
}

TEST_CASE("test_oscquery_osc_large", "test_oscquery_osc_large")
{
  uint16_t shared = 9997;
  auto ctx = std::make_shared<ossia::net::network_context>();
  using conf = ossia::net::osc_protocol_configuration;

  auto osc = ossia::net::make_osc_protocol(
      ctx, {.mode = conf::HOST,
            .version = conf::OSC1_1,
            .transport = ossia::net::udp_configuration{
                {.local = std::nullopt,
                 .remote = ossia::net::outbound_socket_configuration{
                     .host = "127.0.0.1", .port = shared}}}});
  auto oscquery = std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(ctx);

  auto multiplex = std::make_unique<ossia::net::multiplex_protocol>();
  multiplex->expose_to(std::move(osc));
  multiplex->expose_to(std::move(oscquery));
  generic_device device{std::move(multiplex), "my_device"};

  device.set_echo(true);

  // very large should not crash. It will error, though, since
  // we likely hit UDP buffer limits
  auto& bi = find_or_create_node(device, "/foo/bi");
  auto bip = bi.create_parameter(ossia::val_type::STRING);
  bi.set(access_mode_attribute{}, access_mode::BI);
  std::string s(1048576, '*');
  bip->push_value(s);
  ctx->context.run_for(std::chrono::milliseconds(100));
}

TEST_CASE("test_multiplex_remove", "test_multiplex_remove")
{
  uint16_t shared = 9998;
  auto ctx = std::make_shared<ossia::net::network_context>();
  using conf = ossia::net::osc_protocol_configuration;

  auto osc = ossia::net::make_osc_protocol(
      ctx, {.mode = conf::HOST,
            .version = conf::OSC1_1,
            .transport = ossia::net::udp_configuration{
                {.local = std::nullopt,
                 .remote = ossia::net::outbound_socket_configuration{
                     .host = "127.0.0.1", .port = shared}}}});
  auto oscquery = std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(ctx);

  auto multiplex = std::make_unique<ossia::net::multiplex_protocol>();
  multiplex->expose_to(std::move(osc));
  multiplex->expose_to(std::move(oscquery));
  //get the pointer so we can use the protocol after using it with the device
  //is there a better way??
  auto proto = multiplex.get();
  generic_device device{std::move(multiplex), "my_device"};

  device.set_echo(true);

  const auto udp_conf = ossia::net::udp_configuration{
      {.local = ossia::net::inbound_socket_configuration{"0.0.0.0", shared},
       .remote = std::nullopt}};
  ossia::net::generic_device remote{
      ossia::net::make_osc_protocol(
          ctx,
          {.mode = conf::MIRROR,
           .version = conf::OSC1_1,
           .transport = udp_conf}),
      "test_osc"};

  auto& bi = find_or_create_node(device, "/foo/bi");
  auto bip = bi.create_parameter(ossia::val_type::INT);
  bi.set(access_mode_attribute{}, access_mode::BI);
  bool run = true;

  //updating from another thread should  not crash
  auto handle = std::thread([&run, bip]() mutable {
    int v = 0;
    while(run)
    {
      bip->push_value(v++);
    }
  });

  for(int i = 0; i < 10; i++)
  {
    ctx->context.run_for(std::chrono::milliseconds(1));
    for(auto& p : proto->get_protocols())
    {
      //i just want to remove the OSC protocol, but it is easier to match on not oscquery
      auto o = dynamic_cast<ossia::oscquery_asio::oscquery_server_protocol*>(p.get());
      if(!o)
      {
        proto->stop_expose_to(*p);
        break;
      }
    }

    //create a new OSC connection
    using conf = ossia::net::osc_protocol_configuration;
    auto o = ossia::net::make_osc_protocol(
        ctx, {.mode = conf::HOST,
              .version = conf::OSC1_1,
              .framing = conf::SLIP,
              .transport = ossia::net::udp_configuration{
                  {.local = std::nullopt,
                   .remote = ossia::net::outbound_socket_configuration{
                       .host = "127.0.0.1", .port = shared}}}});
    proto->expose_to(std::move(o));
  }

  run = false;
  handle.join();
}

#endif
