// What the OSCQuery server answers, as a raw HTTP or WebSocket client sees it
#include "OSCQueryHttpTestUtils.hpp"
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace ossia;
using namespace ossia::net;
using ossia_test::http_get;

namespace
{
struct served_device
{
  explicit served_device(int ws_port)
      : ws{ws_port}
      , device{
            std::make_unique<ossia::oscquery::oscquery_server_protocol>(
                ws_port - 4000, ws_port),
            "A"}
  {
  }
  parameter_base& float_parameter(std::string_view address, float v)
  {
    auto p = find_or_create_node(device, address).create_parameter(val_type::FLOAT);
    p->push_value(v);
    return *p;
  }
  int ws;
  generic_device device;
};

// One member of the JSON at that target, as text; "absent" if it is not there
std::string member_of(int port, const std::string& target, const char* key)
{
  rapidjson::Document doc;
  doc.Parse(http_get(port, target).body.c_str());
  if(doc.HasParseError() || !doc.IsObject() || !doc.HasMember(key))
    return "absent";
  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> wr(buf);
  doc[key].Accept(wr);
  return buf.GetString();
}
}

TEST_CASE("test_oscquery_listen_data_not_string", "test_oscquery_listen_data_not_string")
{
  // The DATA of LISTEN and IGNORE is a string in the proposal: anything else is
  // ignored, it does not take the server down
  served_device s{5700};
  s.float_parameter("/level", 0.5f);
  ossia_test::raw_ws_client c{s.ws};
  REQUIRE(c.connected());

  for(const char* command : {"LISTEN", "IGNORE"})
    for(const char* data : {"12", "null", "true", "[1]", "{\"a\":1}"})
      c.send(std::string{"{\"COMMAND\":\""} + command + "\",\"DATA\":" + data + "}");
  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  REQUIRE(http_get(s.ws, "/level?VALUE").body == "{\"VALUE\":0.5}");
}

TEST_CASE("test_oscquery_url_fragment", "test_oscquery_url_fragment")
{
  // The fragment of the URL is stripped and disregarded
  served_device s{5701};
  s.float_parameter("/level", 0.5f);

  REQUIRE(http_get(s.ws, "/level#frag").status == 200);
  REQUIRE(http_get(s.ws, "/level#frag").body == http_get(s.ws, "/level").body);
  REQUIRE(http_get(s.ws, "/level?VALUE#frag").body == "{\"VALUE\":0.5}");
  REQUIRE(http_get(s.ws, "/nothing#frag").status == 404);
}

TEST_CASE("test_oscquery_http_error_status", "test_oscquery_http_error_status")
{
  // A node that is not there is 404, a bad request 400: not 500
  served_device s{5702};
  s.float_parameter("/level", 0.5f);

  REQUIRE(http_get(s.ws, "/level?VALUE").status == 200);
  REQUIRE(http_get(s.ws, "/nothing").status == 404);
  REQUIRE(http_get(s.ws, "/nothing?TYPE").status == 404);
  REQUIRE(http_get(s.ws, "/level?LISTEN=true").status == 400);
}

TEST_CASE("test_oscquery_attribute_queries", "test_oscquery_attribute_queries")
{
  // What a query says agrees with the tree; what a node does not have is an
  // empty object, as for the TYPE of a container in the proposal
  served_device s{5703};
  s.float_parameter("/ctn/sub", 0.f);
  s.float_parameter("/f", 0.f);
  find_or_create_node(s.device, "/i").create_parameter(val_type::INT);
  auto v3 = find_or_create_node(s.device, "/v3").create_parameter(val_type::VEC3F);
  v3->set_unit(ossia::cartesian_3d_u{});

  for(const char* query :
      {"/ctn?TYPE", "/ctn?VALUE", "/ctn?RANGE", "/?VALUE", "/f?UNIT", "/f?EXTENDED_TYPE",
       "/i?RANGE", "/v3?UNIT"})
  {
    INFO(query);
    auto r = http_get(s.ws, query);
    REQUIRE(r.status == 200);
    REQUIRE(r.body == "{}");
  }

  for(const char* key :
      {"FULL_PATH", "TYPE", "VALUE", "ACCESS", "CLIPMODE", "EXTENDED_TYPE"})
  {
    INFO(key);
    REQUIRE(
        member_of(s.ws, std::string{"/v3?"} + key, key) == member_of(s.ws, "/v3", key));
  }
  REQUIRE(
      member_of(s.ws, "/v3?EXTENDED_TYPE", "EXTENDED_TYPE").find("position.cartesian.x")
      != std::string::npos);
  REQUIRE(
      member_of(s.ws, "/ctn?CONTENTS", "CONTENTS")
      == member_of(s.ws, "/ctn", "CONTENTS"));
  REQUIRE(http_get(s.ws, "/f?TYPE&ACCESS").body == "{\"TYPE\":\"f\",\"ACCESS\":3}");
}

TEST_CASE("test_oscquery_attribute_unknown", "test_oscquery_attribute_unknown")
{
  // An attribute outside the proposal and outside what the server serves is a
  // bad request, even beside a served one; one of the proposal the server
  // does not serve is 204, without a body
  served_device s{5704};
  s.float_parameter("/ctn/sub", 0.f);

  for(const char* query :
      {"/ctn/sub?BOGUS", "/ctn?BOGUS", "/?BOGUS", "/ctn/sub?BOGUS=1",
       "/ctn/sub?VALUE&BOGUS"})
  {
    INFO(query);
    REQUIRE(http_get(s.ws, query).status == 400);
  }
  for(const char* query : {"/ctn/sub?OVERLOADS", "/ctn?OVERLOADS"})
  {
    INFO(query);
    auto r = http_get(s.ws, query);
    REQUIRE(r.status == 204);
    REQUIRE(r.body.empty());
  }
  REQUIRE(http_get(s.ws, "/nothing?BOGUS").status == 404);
  // A known attribute with a value is still that attribute
  REQUIRE(http_get(s.ws, "/ctn/sub?VALUE=1").status == 200);
  REQUIRE(
      member_of(s.ws, "/ctn/sub?VALUE=1", "VALUE")
      == member_of(s.ws, "/ctn/sub", "VALUE"));
}
