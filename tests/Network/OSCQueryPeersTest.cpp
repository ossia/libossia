// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/detail/json.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

#include <boost/asio/ip/udp.hpp>

#include "OSCQueryHttpTestUtils.hpp"
#include "include_catch.hpp"

#include <chrono>
#include <functional>
#include <thread>

using namespace std::chrono_literals;

namespace
{
rapidjson::Document parse(const std::string& s)
{
  rapidjson::Document doc;
  doc.Parse(s.c_str());
  return doc;
}

//! Whether the EXTENSIONS of HOST_INFO declare key
bool declares(int ws_port, const char* key)
{
  auto doc = parse(ossia_test::http_get(ws_port, "/?HOST_INFO").body);
  REQUIRE(doc.IsObject());
  auto ext = doc.FindMember("EXTENSIONS");
  REQUIRE(ext != doc.MemberEnd());
  auto it = ext->value.FindMember(key);
  return it != ext->value.MemberEnd() && it->value.IsBool() && it->value.GetBool();
}

//! /?PEERS once cond holds on it, or the last answer after 2 s
rapidjson::Document
peers_when(int ws_port, const std::function<bool(const rapidjson::Document&)>& cond)
{
  rapidjson::Document doc;
  for(int i = 0; i < 200; i++)
  {
    doc = parse(ossia_test::http_get(ws_port, "/?PEERS").body);
    if(doc.IsObject() && cond(doc))
      break;
    std::this_thread::sleep_for(10ms);
  }
  return doc;
}

const rapidjson::Value* client_named(const rapidjson::Document& doc, const char* name)
{
  auto clients = doc.FindMember("CLIENTS");
  if(clients == doc.MemberEnd() || !clients->value.IsArray())
    return nullptr;
  for(auto& c : clients->value.GetArray())
  {
    auto n = c.FindMember("NAME");
    if(n != c.MemberEnd() && n->value.IsString()
       && std::string_view{n->value.GetString()} == name)
      return &c;
  }
  return nullptr;
}

//! A UDP OSC message /a 1 from 127.0.0.1; gives the port it left from
int send_osc_from_udp(int osc_port)
{
  boost::asio::io_context ctx;
  boost::asio::ip::udp::socket sock{ctx};
  sock.open(boost::asio::ip::udp::v4());
  sock.bind({boost::asio::ip::make_address("127.0.0.1"), 0});
  const char msg[] = {'/', 'a', 0, 0, ',', 'i', 0, 0, 0, 0, 0, 1};
  sock.send_to(
      boost::asio::buffer(msg, sizeof(msg)),
      {boost::asio::ip::make_address("127.0.0.1"), (unsigned short)osc_port});
  return sock.local_endpoint().port();
}

const rapidjson::Value* sender_on(const rapidjson::Document& doc, int port)
{
  auto senders = doc.FindMember("SENDERS");
  if(senders == doc.MemberEnd() || !senders->value.IsArray())
    return nullptr;
  for(auto& s : senders->value.GetArray())
  {
    auto p = s.FindMember("PORT");
    if(p != s.MemberEnd() && p->value.IsInt() && p->value.GetInt() == port)
      return &s;
  }
  return nullptr;
}
}

TEST_CASE("test_oscquery_peers_off_by_default", "[oscquery]")
{
  using proto_t = ossia::oscquery::oscquery_server_protocol;
  ossia::net::generic_device dev{std::make_unique<proto_t>(1820, 5820), "peers_off"};
  dev.create_child("a")->create_parameter(ossia::val_type::INT);

  CHECK(!declares(5820, "PEERS"));
  CHECK(!declares(5820, "CLIENT_NAME"));

  ossia_test::raw_ws_client ws{5820};
  REQUIRE(ws.connected());
  ws.send(R"({"COMMAND":"CLIENT_NAME","DATA":"probe"})");
  std::this_thread::sleep_for(200ms);

  // Answered as on a server without the extension: no list of the clients
  auto body = ossia_test::http_get(5820, "/?PEERS").body;
  CHECK(body.find("CLIENTS") == std::string::npos);
  CHECK(body.find("probe") == std::string::npos);
}

TEST_CASE("test_oscquery_peers_on", "[oscquery]")
{
  using proto_t = ossia::oscquery::oscquery_server_protocol;
  ossia::net::generic_device dev{std::make_unique<proto_t>(1821, 5821), "peers_on"};
  dev.create_child("a")->create_parameter(ossia::val_type::INT);
  static_cast<proto_t&>(dev.get_protocol()).set_peers_extension(true);

  CHECK(declares(5821, "PEERS"));
  CHECK(declares(5821, "CLIENT_NAME"));

  ossia_test::raw_ws_client ws{5821};
  REQUIRE(ws.connected());
  ws.send(R"({"COMMAND":"CLIENT_NAME","DATA":"probe"})");
  ws.send(R"({"COMMAND":"LISTEN","DATA":"/a"})");

  SECTION("a client named itself and listens")
  {
    auto doc = peers_when(5821, [](const rapidjson::Document& d) {
      auto c = client_named(d, "probe");
      return c && c->FindMember("LISTEN")->value.Size() == 1;
    });
    auto c = client_named(doc, "probe");
    REQUIRE(c);
    CHECK(std::string_view{(*c)["IP"].GetString()} == "127.0.0.1");
    CHECK((*c)["PORT"].GetInt() > 0);
    REQUIRE((*c)["LISTEN"].Size() == 1);
    CHECK(std::string_view{(*c)["LISTEN"][0].GetString()} == "/a");
  }

  SECTION("a UDP source that is none of the clients")
  {
    const int port = send_osc_from_udp(1821);
    auto doc = peers_when(
        5821, [port](const rapidjson::Document& d) { return sender_on(d, port); });
    auto s = sender_on(doc, port);
    REQUIRE(s);
    CHECK(std::string_view{(*s)["IP"].GetString()} == "127.0.0.1");
    CHECK((*s)["LAST_MS"].GetInt64() >= 0);
  }

  SECTION("at the root only")
  {
    auto body = ossia_test::http_get(5821, "/a?PEERS").body;
    CHECK(body.find("CLIENTS") == std::string::npos);
  }
}

TEST_CASE("test_oscquery_asio_peers", "[oscquery]")
{
  using proto_t = ossia::oscquery_asio::oscquery_server_protocol;
  auto ctx = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device dev{
      std::make_unique<proto_t>(ctx, 1822, 5822), "asio_peers"};
  dev.create_child("a")->create_parameter(ossia::val_type::INT);
  auto& proto = static_cast<proto_t&>(dev.get_protocol());

  // Runs the context, and stops it before the device goes
  struct runner
  {
    std::shared_ptr<ossia::net::network_context> ctx;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work{
        ctx->context.get_executor()};
    std::thread thread{[c = ctx] { c->context.run(); }};
    ~runner()
    {
      ctx->context.stop();
      thread.join();
    }
  } run{ctx};

  // Off by default
  CHECK(!declares(5822, "PEERS"));
  CHECK(!declares(5822, "CLIENT_NAME"));

  proto.set_peers_extension(true);
  CHECK(declares(5822, "PEERS"));
  CHECK(declares(5822, "CLIENT_NAME"));

  ossia_test::raw_ws_client ws{5822};
  REQUIRE(ws.connected());
  ws.send(R"({"COMMAND":"CLIENT_NAME","DATA":"probe"})");
  auto doc = peers_when(
      5822, [](const rapidjson::Document& d) { return client_named(d, "probe"); });
  CHECK(client_named(doc, "probe"));
  // This server does not know where its UDP messages come from
  CHECK(doc.FindMember("SENDERS") == doc.MemberEnd());
}

TEST_CASE("test_oscquery_mirror_client_name", "[oscquery]")
{
  using proto_t = ossia::oscquery::oscquery_server_protocol;
  using mirror_t = ossia::oscquery::oscquery_mirror_protocol;
  ossia::net::generic_device dev{std::make_unique<proto_t>(1823, 5823), "named"};
  dev.create_child("a")->create_parameter(ossia::val_type::INT);
  auto& server = static_cast<proto_t&>(dev.get_protocol());

  auto clients = [](const rapidjson::Document& d) {
    auto c = d.FindMember("CLIENTS");
    return c != d.MemberEnd() && c->value.IsArray() ? (int)c->value.Size() : 0;
  };

  SECTION("no name by default")
  {
    server.set_peers_extension(true);
    auto mirror = std::make_unique<mirror_t>("ws://127.0.0.1:5823", 10823);
    CHECK(mirror->get_client_name().empty());
    ossia::net::generic_device client{std::move(mirror), "client"};
    auto doc = peers_when(
        5823, [&](const rapidjson::Document& d) { return clients(d) == 1; });
    REQUIRE(clients(doc) == 1);
    std::this_thread::sleep_for(200ms);
    doc = parse(ossia_test::http_get(5823, "/?PEERS").body);
    CHECK(doc["CLIENTS"][0].FindMember("NAME") == doc["CLIENTS"][0].MemberEnd());
  }

  SECTION("a name, to a server that declares CLIENT_NAME")
  {
    server.set_peers_extension(true);
    auto mirror = std::make_unique<mirror_t>("ws://127.0.0.1:5823", 10823);
    auto& m = *mirror;
    ossia::net::generic_device client{std::move(mirror), "client"};
    m.set_client_name("probe");
    auto doc = peers_when(
        5823, [](const rapidjson::Document& d) { return client_named(d, "probe"); });
    CHECK(client_named(doc, "probe"));
  }

  SECTION("no name to a server that does not declare it")
  {
    auto mirror = std::make_unique<mirror_t>("ws://127.0.0.1:5823", 10823);
    auto& m = *mirror;
    ossia::net::generic_device client{std::move(mirror), "client"};
    m.set_client_name("probe");
    std::this_thread::sleep_for(300ms);

    // The extension turned on afterwards: the mirror had not sent its name
    server.set_peers_extension(true);
    auto doc = peers_when(
        5823, [&](const rapidjson::Document& d) { return clients(d) == 1; });
    REQUIRE(clients(doc) == 1);
    CHECK(!client_named(doc, "probe"));
  }
}
