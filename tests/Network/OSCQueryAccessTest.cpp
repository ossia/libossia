// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/value/value_conversion.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

#include "include_catch.hpp"
#include "OSCQueryHttpTestUtils.hpp"

#include <boost/asio/ip/udp.hpp>
#include <boost/endian/conversion.hpp>

#include <chrono>
#include <cstring>
#include <functional>
#include <thread>

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono_literals;

namespace
{
//! "<address> ,f <f>" as one OSC message
std::string osc_float_message(const std::string& address, float f)
{
  std::string m = address;
  m.append(4 - m.size() % 4, '\0');
  m += ",f";
  m.append(2, '\0');
  uint32_t bits;
  std::memcpy(&bits, &f, 4);
  bits = boost::endian::native_to_big(bits);
  m.append(reinterpret_cast<const char*>(&bits), 4);
  return m;
}

void send_udp(int port, const std::string& data)
{
  boost::asio::io_context ctx;
  boost::asio::ip::udp::socket sock{ctx, boost::asio::ip::udp::v4()};
  sock.send_to(
      boost::asio::buffer(data),
      {boost::asio::ip::make_address("127.0.0.1"), (unsigned short)port});
}

bool wait_for(const std::function<bool()>& f, std::chrono::milliseconds d = 2000ms)
{
  for(auto end = std::chrono::steady_clock::now() + d;
      std::chrono::steady_clock::now() < end; std::this_thread::sleep_for(10ms))
    if(f())
      return true;
  return f();
}

float value_of(parameter_base& p)
{
  return ossia::convert<float>(p.value());
}

// /ro is read-only, /so write-only, /rw both. Each way in is checked by
// writing /ro then /rw the same way: once /rw has its value, /ro had its
// chance.
template <typename Server>
void access(generic_device& dev, Server& server, int osc_port, int ws_port, bool applied)
{
  server.set_apply_access(applied);
  auto ro = dev.create_child("ro")->create_parameter(val_type::FLOAT);
  auto rw = dev.create_child("rw")->create_parameter(val_type::FLOAT);
  auto so = dev.create_child("so")->create_parameter(val_type::FLOAT);
  ro->set_access(access_mode::GET);
  so->set_access(access_mode::SET);
  ro->push_value(0.5f);
  so->push_value(0.25f);

  ossia_test::raw_ws_client c{ws_port};
  REQUIRE(c.connected());

  int n = 1;
  auto written = [&](const std::function<void(const std::string&, float)>& write) {
    const float value = 10.f * n++;
    write("/ro", value + 1.f);
    write("/rw", value);
    REQUIRE(wait_for([&] { return value_of(*rw) == value; }));
    std::this_thread::sleep_for(50ms);
    if(applied)
      CHECK(value_of(*ro) == 0.5f);
    else
      CHECK(value_of(*ro) == value + 1.f);
  };
  // OSC over UDP, OSC over WebSocket, JSON over WebSocket
  written([&](auto& a, float f) { send_udp(osc_port, osc_float_message(a, f)); });
  written(
      [&](auto& a, float f) { c.client.send_binary_message(osc_float_message(a, f)); });
  written(
      [&](auto& a, float f) { c.send("{\"" + a + "\":" + std::to_string(f) + "}"); });

  // A write-only parameter can be written; its value is served only without
  // the option
  send_udp(osc_port, osc_float_message("/so", 3.f));
  REQUIRE(wait_for([&] { return value_of(*so) == 3.f; }));
  auto reply = ossia_test::http_get(ws_port, "/so?VALUE");
  if(applied)
    CHECK(reply.body.find("VALUE") == std::string::npos);
  else
    CHECK(reply.body.find("VALUE") != std::string::npos);
  CHECK(
      ossia_test::http_get(ws_port, "/ro?VALUE").body.find("VALUE")
      != std::string::npos);
}

void websocketpp_server(bool applied)
{
  auto proto = new ossia::oscquery::oscquery_server_protocol{1770, 5770};
  generic_device dev{std::unique_ptr<protocol_base>(proto), "access"};
  CHECK(!proto->get_apply_access());
  access(dev, *proto, 1770, 5770, applied);
}

void asio_server(bool applied)
{
  auto ctx = std::make_shared<network_context>();
  auto proto = new ossia::oscquery_asio::oscquery_server_protocol{ctx, 1771, 5771};
  auto dev = std::make_unique<generic_device>(
      std::unique_ptr<protocol_base>(proto), "access_asio");
  std::thread t{[ctx] { ctx->run(); }};
  CHECK(!proto->get_apply_access());
  access(*dev, *proto, 1771, 5771, applied);
  ctx->context.stop();
  t.join();
  dev.reset();
}
}

TEST_CASE("test_oscquery_access_off", "[oscquery]")
{
  // As before: every client writes and reads every parameter
  websocketpp_server(false);
  asio_server(false);
}

TEST_CASE("test_oscquery_access_applied", "[oscquery]")
{
  websocketpp_server(true);
  asio_server(true);
}
