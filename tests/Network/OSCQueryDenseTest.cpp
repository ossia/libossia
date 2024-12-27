// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/detail/osc_writer.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/sockets/null_socket.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/protocols/dense/dense_protocol.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

#include <boost/endian/conversion.hpp>

#include <iostream>

using namespace ossia;
using namespace ossia::net;
struct test_device_dense
{
  ossia::net::node_base& device;
  ossia::net::parameter_base& af1 = *ossia::create_parameter(device, "/a/f1", "float");
  ossia::net::parameter_base& af2 = *ossia::create_parameter(device, "/a/f2", "float");
  ossia::net::parameter_base& af3 = *ossia::create_parameter(device, "/a/f3", "float");
  ossia::net::parameter_base& af4 = *ossia::create_parameter(device, "/a/f4", "float");
  ossia::net::parameter_base& bf1 = *ossia::create_parameter(device, "/b/f1", "float");
  ossia::net::parameter_base& bf2 = *ossia::create_parameter(device, "/b/f2", "float");
  ossia::net::parameter_base& bf3 = *ossia::create_parameter(device, "/b/f3", "float");
  ossia::net::parameter_base& bf4 = *ossia::create_parameter(device, "/b/f4", "float");
  ossia::net::parameter_base& ai1 = *ossia::create_parameter(device, "/a/i1", "int");
  ossia::net::parameter_base& ai2 = *ossia::create_parameter(device, "/a/i2", "int");
  ossia::net::parameter_base& ai3 = *ossia::create_parameter(device, "/a/i3", "int");
  ossia::net::parameter_base& ai4 = *ossia::create_parameter(device, "/a/i4", "int");
  ossia::net::parameter_base& bi1 = *ossia::create_parameter(device, "/b/i1", "int");
  ossia::net::parameter_base& bi2 = *ossia::create_parameter(device, "/b/i2", "int");
  ossia::net::parameter_base& bi3 = *ossia::create_parameter(device, "/b/i3", "int");
  ossia::net::parameter_base& bi4 = *ossia::create_parameter(device, "/b/i4", "int");
};

TEST_CASE("test_oscq_send_dense", "test_oscq_send_dense")
{
  using namespace std::literals;
  auto ctx = std::make_shared<ossia::net::network_context>();

  {
    auto proto1 = std::make_unique<ossia::net::multiplex_protocol>();

    ossia::net::inbound_socket_configuration in_udp{.bind = "", .port = 5478};
    auto proto2 = std::make_unique<ossia::oscquery_asio::oscquery_server_protocol_base>(
        ctx, std::vector<ossia::net::osc_server_configuration>{}, 5678, false);
    proto1->expose_to(std::move(proto2));

    ossia::net::dense_configuration out_conf{.interval = std::chrono::milliseconds(100)};
    ossia::net::outbound_socket_configuration out_udp{
        .host = "127.0.0.1", .port = 5478, .broadcast = true};
    auto proto3 = std::make_unique<ossia::net::dense_generic_bidir_protocol<
        ossia::net::udp_send_socket, ossia::net::null_socket>>(ctx, out_conf, out_udp);
    proto1->expose_to(std::move(proto3));

    ossia::net::generic_device serv{std::move(proto1), "foo"};
    test_device_dense x{serv.get_root_node()};

    for(int i = 0; i < 100; i++)
    {
      auto t0 = std::chrono::steady_clock::now();
      ossia::net::iterate_all_children(
          &serv.get_root_node(),
          [](ossia::net::parameter_base& p) { p.push_value(rand()); });

      ctx->context.run_one();
      if(i < 99)
        ctx->context.restart();
      else
        ctx->context.stop();
    }
  }
}
