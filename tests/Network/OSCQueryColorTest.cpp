// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <iostream>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
#include <boost/endian/conversion.hpp>
#include "TestUtils.hpp"

using namespace ossia;
using namespace ossia::net;
TEST_CASE ("test_osc_writer_send_rgba8", "test_osc_writer_send_rgba8")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.vec4f_addr->set_unit(ossia::rgba8_u{});

  network_logger l;
  auto res = ossia::oscquery::osc_writer::send_message(*dev.vec4f_addr, ossia::make_vec(0, 59, 111, 255), l);

  uint32_t exp = boost::endian::native_to_big((0 << 24) + (59 << 16) + (111 << 8) + 255);
  const char* data = reinterpret_cast<const char*>(&exp);
  auto expected = std::string("/vec4f\0\0,r\0\0", 12) + std::string(data, 4);
  REQUIRE(res == expected);
}

TEST_CASE ("test_osc_receive_rgba8", "test_osc_receive_rgba8")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};
  dev.vec4f_addr->set_unit(ossia::rgba8_u{});

  uint32_t exp = boost::endian::native_to_big((0 << 24) + (59 << 16) + (111 << 8) + 255);
  const char* data = reinterpret_cast<const char*>(&exp);
  auto expected = std::string("/vec4f\0\0,r\0\0", 12) + std::string(data, 4);

  oscpack::ReceivedMessage m(oscpack::ReceivedPacket{expected.data(), expected.size()});

  ossia::net::listened_parameters p;
  network_logger l;
  ossia::net::handle_osc_message<true>(m, p, dev.device, l);

  REQUIRE(dev.vec4f_addr->value() == ossia::value{ossia::make_vec(0, 59, 111, 255)});
}

TEST_CASE ("test_json_rgba8", "test_json_rgba8")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.vec4f_addr->set_unit(ossia::rgba8_u{});
  dev.vec4f_addr->push_value(ossia::make_vec(0, 59, 111, 255));
  auto json = oscquery::json_writer{}.query_namespace(dev.vec4f_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "r"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 1);
    REQUIRE(doc["VALUE"][0].IsString());
    REQUIRE(doc["VALUE"][0].GetString() == "#003B6FFF"s);
  }
}
TEST_CASE ("test_parse_json_rgba8", "test_parse_json_rgba8")
{
  using namespace std::literals;

  std::string json = R"_(
                     {
                     "FULL_PATH": "/",
                     "ACCESS": 0,
                     "CONTENTS": {
                     "vec4f": {
                     "FULL_PATH": "/vec4f",
                     "ACCESS": 1,
                     "TYPE": "r",
                     "VALUE": ["#003B6FFF"]
                     }
                     }
                     }
                     )_";

  generic_device serv{"foo"};

  ossia::oscquery::load_oscquery_device(serv, json);
  auto n = ossia::net::find_node(serv, "/vec4f");
  REQUIRE(n);
  auto p = n->get_parameter();
  REQUIRE(p->get_unit() == ossia::rgba8_u{});
  REQUIRE(p->value() == ossia::value{ossia::make_vec(0, 59, 111, 255)});
}
