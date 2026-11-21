// What the OSCQuery server answers, as a raw HTTP or WebSocket client sees it
#include "OSCQueryHttpTestUtils.hpp"
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

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
