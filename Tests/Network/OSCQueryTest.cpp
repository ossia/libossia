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
#include "TestUtils.hpp"

using namespace ossia;
using namespace ossia::net;
TEST_CASE ("test_oscquery", "test_oscquery")
{
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
  TestDeviceRef d{serv};
  auto& dev = d.device;

  auto& n = find_or_create_node(serv, "/main");
  auto a = n.create_parameter(ossia::val_type::FLOAT);

  a->push_value(6.5);

  n.set(access_mode_attribute{}, access_mode::GET);
  n.set(bounding_mode_attribute{}, bounding_mode::FOLD);
  n.set(domain_attribute{}, make_domain(-10., 10.));
  n.set(default_value_attribute{}, ossia::value(0.));
  n.set(tags_attribute{}, tags{"fancy", "wow", "1234"});
  n.set(refresh_rate_attribute{}, 100);
  n.set(value_step_size_attribute{}, 0.5);
  n.set(repetition_filter_attribute{}, repetition_filter::ON);
  n.set(critical_attribute{}, true);
  n.set(unit_attribute{}, meter_per_second_u{});
  n.set(priority_attribute{}, 50);
  n.set(description_attribute{}, "Such a fancy node?! Incredible! すごい!!");
  n.set(app_name_attribute{}, "AppName");
  n.set(app_version_attribute{}, "1.0.0");
  n.set(app_creator_attribute{}, "Lelouch vi Brittania");



  // Node -> json
  auto str = ossia::oscquery::json_writer::query_namespace(dev);
  std::cerr << str.GetString() << std::endl;

  // Clear the device
  dev.clear_children();

  // Parse json
  rapidjson::Document doc;
  doc.Parse( str.GetString() );

  // Json -> node
  ossia::oscquery::json_parser::parse_namespace(dev, doc);

  {
    auto node = find_node(dev, "/main");
    REQUIRE(node);
    auto& n = *node;

    REQUIRE((bool)get_access_mode(n));
    REQUIRE(*get_access_mode(n) == access_mode::GET);

    REQUIRE((bool)get_bounding_mode(n));
    REQUIRE(*get_bounding_mode(n) == bounding_mode::FOLD);

    REQUIRE((bool)get_domain(n));
    REQUIRE(get_domain(n) == make_domain(-10., 10.));

    REQUIRE((bool)get_default_value(n));
    REQUIRE(*get_default_value(n) == ossia::value(0));

    tags the_tags{"fancy", "wow", "1234"};
    REQUIRE((bool)get_tags(n));
    REQUIRE(*get_tags(n) == the_tags);

    REQUIRE((bool)get_refresh_rate(n));
    REQUIRE(*get_refresh_rate(n) == 100);

    REQUIRE((bool)get_value_step_size(n));
    REQUIRE(*get_value_step_size(n) == 0.5);

    REQUIRE(get_repetition_filter(n));
    REQUIRE(get_repetition_filter(n) == repetition_filter::ON);

    REQUIRE(get_critical(n));

    REQUIRE((bool)get_unit(n));
    REQUIRE(get_unit(n) == ossia::unit_t(meter_per_second_u{}));

    REQUIRE((bool)get_priority(n));
    REQUIRE(*get_priority(n) == 50.f);

    REQUIRE((bool)get_description(n));
    REQUIRE(*get_description(n) == std::string("Such a fancy node?! Incredible! すごい!!"));

    REQUIRE((bool)get_app_name(n));
    REQUIRE(*get_app_name(n) == std::string("AppName"));

    REQUIRE((bool)get_app_version(n));
    REQUIRE(*get_app_version(n) == std::string("1.0.0"));

    REQUIRE((bool)get_app_creator(n));
    REQUIRE(*get_app_creator(n) == std::string("Lelouch vi Brittania"));
  }
}

TEST_CASE ("test_oscquery_unit_1", "test_oscquery_unit_1")
{
  generic_device serv{"A"};

  ossia::create_parameter(serv, "/main", "color.hsv");

  // Node -> json
  auto str = ossia::oscquery::json_writer::query_namespace(serv);
  std::cerr << str.GetString() << std::endl;

  // Clear the device
  serv.clear_children();

  // Parse json
  rapidjson::Document doc;
  doc.Parse( str.GetString() );

  // Json -> node
  ossia::oscquery::json_parser::parse_namespace(serv, doc);

  {
    auto node = find_node(serv, "/main");
    REQUIRE(node);
    auto& n = *node;

    REQUIRE((bool)get_unit(n));
    REQUIRE(get_unit(n) == ossia::unit_t(hsv_u{}));
  }
}

TEST_CASE ("test_oscquery_unit_2", "test_oscquery_unit_2")
{
  generic_device serv{"A"};

  ossia::create_parameter(serv, "/main", "centimeter");

  // Node -> json
  auto str = ossia::oscquery::json_writer::query_namespace(serv);
  std::cerr << str.GetString() << std::endl;

  // Clear the device
  serv.clear_children();

  // Parse json
  rapidjson::Document doc;
  doc.Parse( str.GetString() );

  // Json -> node
  ossia::oscquery::json_parser::parse_namespace(serv, doc);

  {
    auto node = find_node(serv, "/main");
    REQUIRE(node);
    auto& n = *node;

    REQUIRE((bool)get_unit(n));
    REQUIRE(get_unit(n) == ossia::unit_t(centimeter_u{}));
  }
}

TEST_CASE ("test_oscquery_unit_3", "test_oscquery_unit_3")
{
  generic_device serv{"A"};

  ossia::create_parameter(serv, "/main", "blob");

  // Node -> json
  auto str = ossia::oscquery::json_writer::query_namespace(serv);
  std::cerr << str.GetString() << std::endl;

  // Clear the device
  serv.clear_children();

  // Parse json
  rapidjson::Document doc;
  doc.Parse( str.GetString() );

  // Json -> node
  ossia::oscquery::json_parser::parse_namespace(serv, doc);

  {
    auto node = find_node(serv, "/main");
    REQUIRE(node);
    auto& n = *node;

    REQUIRE((bool)get_extended_type(n));
    REQUIRE(get_extended_type(n) == ossia::generic_buffer_type());
  }
}


TEST_CASE ("test_json_impulse", "test_json_impulse")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  auto json = oscquery::json_writer{}.query_namespace(dev.impulse_addr->get_node());
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "I"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 1);
    REQUIRE(doc["VALUE"][0].IsNull());
  }
}

TEST_CASE ("test_json_bool", "test_json_bool")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  {
    dev.bool_addr->push_value(false);
    auto json = oscquery::json_writer{}.query_namespace(dev.bool_addr->get_node());
    std::cerr << json.GetString();
    {
      rapidjson::Document doc;
      doc.Parse(json.GetString());
      REQUIRE(doc.IsObject());
      REQUIRE(doc.HasMember("TYPE"));
      REQUIRE(doc.HasMember("VALUE"));
      REQUIRE(doc["TYPE"].IsString());
      REQUIRE(doc["TYPE"].GetString() == "F"s);

      REQUIRE(doc["VALUE"].IsArray());
      REQUIRE(doc["VALUE"].GetArray().Size() == 1);
      REQUIRE(doc["VALUE"][0].IsNull());
    }
  }
  {
    dev.bool_addr->push_value(true);
    auto json = oscquery::json_writer{}.query_namespace(dev.bool_addr->get_node());
    std::cerr << json.GetString();
    {
      rapidjson::Document doc;
      doc.Parse(json.GetString());
      REQUIRE(doc.IsObject());
      REQUIRE(doc.HasMember("TYPE"));
      REQUIRE(doc.HasMember("VALUE"));
      REQUIRE(doc["TYPE"].IsString());
      REQUIRE(doc["TYPE"].GetString() == "T"s);

      REQUIRE(doc["VALUE"].IsArray());
      REQUIRE(doc["VALUE"].GetArray().Size() == 1);
      REQUIRE(doc["VALUE"][0].IsNull());
    }
  }
}

TEST_CASE ("test_json_int", "test_json_int")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.int_addr->push_value(1234);
  auto json = oscquery::json_writer{}.query_namespace(dev.int_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "i"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 1);
    REQUIRE(doc["VALUE"][0].IsInt());
    REQUIRE(doc["VALUE"][0].GetInt() == 1234);
  }
}

TEST_CASE ("test_json_float", "test_json_float")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.float_addr->push_value(1234.f);
  auto json = oscquery::json_writer{}.query_namespace(dev.float_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "f"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 1);
    REQUIRE(doc["VALUE"][0].IsFloat());
    REQUIRE(doc["VALUE"][0].GetFloat() == 1234.f);
  }
}

TEST_CASE ("test_json_string", "test_json_string")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.string_addr->push_value("hello world");
  auto json = oscquery::json_writer{}.query_namespace(dev.string_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "s"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 1);
    REQUIRE(doc["VALUE"][0].IsString());
    REQUIRE(doc["VALUE"][0].GetString() == "hello world"s);
  }
}


TEST_CASE ("test_json_char", "test_json_char")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.char_addr->push_value('x');
  auto json = oscquery::json_writer{}.query_namespace(dev.char_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "c"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 1);
    REQUIRE(doc["VALUE"][0].IsString());
    REQUIRE(doc["VALUE"][0].GetString() == "x"s);
  }
}


TEST_CASE ("test_json_vec2f", "test_json_vec2f")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.vec2f_addr->push_value(ossia::make_vec(123, 456));
  auto json = oscquery::json_writer{}.query_namespace(dev.vec2f_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "ff"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 2);
    REQUIRE(doc["VALUE"][0].IsFloat());
    REQUIRE(doc["VALUE"][0].GetFloat() == 123.f);
    REQUIRE(doc["VALUE"][1].IsFloat());
    REQUIRE(doc["VALUE"][1].GetFloat() == 456.f);
  }
}

TEST_CASE ("test_json_vec3f", "test_json_vec3f")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.vec3f_addr->push_value(ossia::make_vec(123, 456, 789));
  auto json = oscquery::json_writer{}.query_namespace(dev.vec3f_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "fff"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 3);
    REQUIRE(doc["VALUE"][0].IsFloat());
    REQUIRE(doc["VALUE"][0].GetFloat() == 123.f);
    REQUIRE(doc["VALUE"][1].IsFloat());
    REQUIRE(doc["VALUE"][1].GetFloat() == 456.f);
    REQUIRE(doc["VALUE"][2].IsFloat());
    REQUIRE(doc["VALUE"][2].GetFloat() == 789.f);
  }
}


TEST_CASE ("test_json_vec4f", "test_json_vec4f")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.vec4f_addr->push_value(ossia::make_vec(123, 456, 789, 10));
  auto json = oscquery::json_writer{}.query_namespace(dev.vec4f_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "ffff"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 4);
    REQUIRE(doc["VALUE"][0].IsFloat());
    REQUIRE(doc["VALUE"][0].GetFloat() == 123.f);
    REQUIRE(doc["VALUE"][1].IsFloat());
    REQUIRE(doc["VALUE"][1].GetFloat() == 456.f);
    REQUIRE(doc["VALUE"][2].IsFloat());
    REQUIRE(doc["VALUE"][2].GetFloat() == 789.f);
    REQUIRE(doc["VALUE"][3].IsFloat());
    REQUIRE(doc["VALUE"][3].GetFloat() == 10.f);
  }
}


TEST_CASE ("test_json_list", "test_json_list")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.tuple_addr->push_value(
        std::vector<ossia::value>{1.234,
                                  "foobar",
                                  std::vector<ossia::value>{ 45, 43},
                                  std::vector<ossia::value>{ }
        });
  auto json = oscquery::json_writer{}.query_namespace(dev.tuple_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "fs[ii][]"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 4);
    REQUIRE(doc["VALUE"][0].IsFloat());
    REQUIRE(doc["VALUE"][0].GetFloat() == 1.234f);
    REQUIRE(doc["VALUE"][1].IsString());
    REQUIRE(doc["VALUE"][1].GetString() == "foobar"s);
    REQUIRE(doc["VALUE"][2].IsArray());
    REQUIRE(doc["VALUE"][2].GetArray().Size() == (unsigned int)2);
    REQUIRE(doc["VALUE"][2][0].GetInt() == 45);
    REQUIRE(doc["VALUE"][2][1].GetInt() == 43);
    REQUIRE(doc["VALUE"][3].IsArray());
    REQUIRE(doc["VALUE"][3].GetArray().Size() == (unsigned int)0);
  }
}
TEST_CASE ("test_json_rgba8", "test_json_rgba8")
{
  /*
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.vec4f_addr->push_value(ossia::rgba8_u{});
      dev.vec4f_addr->push_value(ossia::make_vec(123, 456, 789, 10));
      auto json = oscquery::json_writer{}.query_namespace(dev.int_addr->get_node());
      std::cerr << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        REQUIRE(doc.IsObject());
        REQUIRE(doc.HasMember("TYPE"));
        REQUIRE(doc.HasMember("VALUE"));
        REQUIRE(doc["TYPE"].IsString());
        REQUIRE(doc["TYPE"].GetString() == "ffff"s);

        REQUIRE(doc["VALUE"].IsArray());
        REQUIRE(doc["VALUE"].GetArray().Size() == 4);
        REQUIRE(doc["VALUE"][0].IsFloat());
        REQUIRE(doc["VALUE"][0].GetFloat() == 123.f);
        REQUIRE(doc["VALUE"][1].IsFloat());
        REQUIRE(doc["VALUE"][1].GetFloat() == 456.f);
        REQUIRE(doc["VALUE"][2].IsFloat());
        REQUIRE(doc["VALUE"][2].GetFloat() == 789.f);
        REQUIRE(doc["VALUE"][3].IsFloat());
        REQUIRE(doc["VALUE"][3].GetFloat() == 10.f);
      }
      */
}

TEST_CASE ("test_json_unit_float", "test_json_unit_float")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.float_addr->set_unit(ossia::midigain_u{});
  dev.float_addr->push_value(100);
  auto json = oscquery::json_writer{}.query_namespace(dev.float_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "f"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 1);
    REQUIRE(doc["VALUE"][0].IsFloat());
    REQUIRE(doc["VALUE"][0].GetFloat() == 100.f);

    REQUIRE(doc.HasMember("UNIT"));
    REQUIRE(doc["UNIT"].IsArray());
    REQUIRE(doc["UNIT"].GetArray()[0].GetString() == "gain.midigain"s);
  }
}

TEST_CASE ("test_json_unit_vec3", "test_json_unit_vec3")
{
  using namespace std::literals;
  generic_device serv{"foo"};
  TestDeviceRef dev{serv};

  dev.vec3f_addr->set_unit(ossia::cartesian_3d_u{});
  dev.vec3f_addr->push_value(ossia::make_vec(1., 2., 3.));
  auto json = oscquery::json_writer{}.query_namespace(dev.vec3f_addr->get_node());
  std::cerr << json.GetString();
  {
    rapidjson::Document doc;
    doc.Parse(json.GetString());
    REQUIRE(doc.IsObject());
    REQUIRE(doc.HasMember("TYPE"));
    REQUIRE(doc.HasMember("VALUE"));
    REQUIRE(doc["TYPE"].IsString());
    REQUIRE(doc["TYPE"].GetString() == "fff"s);

    REQUIRE(doc["VALUE"].IsArray());
    REQUIRE(doc["VALUE"].GetArray().Size() == 3);
    REQUIRE(doc["VALUE"][0].IsFloat());
    REQUIRE(doc["VALUE"][0].GetFloat() == 1.f);
    REQUIRE(doc["VALUE"][1].IsFloat());
    REQUIRE(doc["VALUE"][1].GetFloat() == 2.f);
    REQUIRE(doc["VALUE"][2].IsFloat());
    REQUIRE(doc["VALUE"][2].GetFloat() == 3.f);

    REQUIRE(doc.HasMember("EXTENDED_TYPE"));
    REQUIRE(doc["EXTENDED_TYPE"].IsArray());
    REQUIRE(doc["EXTENDED_TYPE"].GetArray()[0] == "position.cartesian.x"s);
    REQUIRE(doc["EXTENDED_TYPE"].GetArray()[1] == "position.cartesian.y"s);
    REQUIRE(doc["EXTENDED_TYPE"].GetArray()[2] == "position.cartesian.z"s);
  }
}
TEST_CASE ("test_oscquery_http", "test_oscquery_http")
{
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
  TestDeviceRef dev{serv}; (void) dev;
  ossia::net::parameter_base* a{};
  {
    auto& n = find_or_create_node(serv, "/main");
    a = n.create_parameter(ossia::val_type::FLOAT);

    a->push_value(6);

    n.set(access_mode_attribute{}, access_mode::GET);
    n.set(bounding_mode_attribute{}, bounding_mode::FOLD);
    n.set(domain_attribute{}, make_domain(-10., 10.));
    n.set(default_value_attribute{}, ossia::value(0.));
    n.set(tags_attribute{}, tags{"fancy", "wow", "1234"});
    n.set(refresh_rate_attribute{}, 100);
    n.set(value_step_size_attribute{}, 0.5);
    n.set(repetition_filter_attribute{}, repetition_filter::ON);
    n.set(critical_attribute{}, false);
    n.set(unit_attribute{}, meter_per_second_u{});
    n.set(priority_attribute{}, 50);
    n.set(description_attribute{}, "Such a fancy node?! Incredible! すごい!!");
    n.set(extended_type_attribute{}, "custom");
    n.set(app_name_attribute{}, "AppName");
    n.set(app_version_attribute{}, "1.0.0");
    n.set(app_creator_attribute{}, "Lelouch vi Brittania");
  }

  // HTTP client
  auto http_proto = new ossia::oscquery::oscquery_mirror_protocol("http://127.0.0.1:5678", 10000);
  auto http_clt = new generic_device{std::unique_ptr<ossia::net::protocol_base>(http_proto), "B"};


  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  http_proto->update(http_clt->get_root_node());

  {
    net::full_parameter_data d; d.address = "/float"; d.set_value(123.f);
    http_proto->push_raw(d);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // WS client
  auto ws_proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678", 10001);
  auto ws_clt = new generic_device{std::unique_ptr<ossia::net::protocol_base>(ws_proto), "B"};

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ws_proto->update(ws_clt->get_root_node());

  double d{7.8};
  a->push_value(d);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  auto node = find_node(ws_clt->get_root_node(), "/main");
  REQUIRE(node);
  auto param = node->get_parameter();
  REQUIRE(param);
  auto b = param->value();
  std::cout << "new value : " << b << " expecting " << d << std::endl;
  ossia::value expected_value{d};

  net::full_parameter_data d2; d2.address = "/int"; d2.set_value(546);
  ws_proto->push_raw(d2);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  delete ws_clt;

  // should use QCOMPARE after device cleaning to avoid hang
  REQUIRE(b == expected_value);
}

TEST_CASE ("test_oscquery_critical_http", "test_oscquery_critical_http")
{
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
  TestDeviceRef dev{serv}; (void) dev;
  ossia::net::parameter_base* a{};
  {
    auto& n = find_or_create_node(serv, "/main");
    a = n.create_parameter(ossia::val_type::FLOAT);
    a->push_value(6);
    n.set(critical_attribute{}, true);
  }

  // HTTP client
  auto http_proto = new ossia::oscquery::oscquery_mirror_protocol("http://127.0.0.1:5678", 10000);
  std::unique_ptr<generic_device> http_clt{new generic_device{std::unique_ptr<ossia::net::protocol_base>(http_proto), "B"}};


  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  http_proto->update(http_clt->get_root_node());

  auto node = find_node(http_clt->get_root_node(), "/main");
  REQUIRE(node);
  auto param = node->get_parameter();
  REQUIRE(param);
  REQUIRE(param->value().get<float>() == 6.f);

  {
    net::full_parameter_data d; d.address = "/main"; d.set_value(4.5f);
    http_proto->push_raw(d); //  no way to send raw OSC through http requests: this will god through the OSC port
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  REQUIRE(a->value().get<float>() == 4.5f);
}


TEST_CASE ("test_oscquery_critical_ws", "test_oscquery_critical_ws")
{
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
  TestDeviceRef dev{serv}; (void) dev;
  ossia::net::parameter_base* a{};
  {
    auto& n = find_or_create_node(serv, "/main");
    a = n.create_parameter(ossia::val_type::FLOAT);
    a->push_value(6);
    n.set(critical_attribute{}, true);
  }

  // WS client
  auto ws_proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678", 10001);
  std::unique_ptr<generic_device> ws_clt{new generic_device{std::unique_ptr<ossia::net::protocol_base>(ws_proto), "B"}};

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ws_proto->update(ws_clt->get_root_node());
  auto node = find_node(ws_clt->get_root_node(), "/main");
  REQUIRE(node);
  auto param = node->get_parameter();
  REQUIRE(param);
  REQUIRE(param->value().get<float>() == 6.f);

  {
    param->push_value(4.5f); // This will god through the WS port
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  std::cout << "new value : " << a->value() << " expecting " << 4.5f << std::endl;
  // should use QCOMPARE after device cleaning to avoid hang
  REQUIRE(a->value().get<float>() == 4.5f);

}
