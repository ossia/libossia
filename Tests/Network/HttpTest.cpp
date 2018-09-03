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
#include <ossia/network/oscquery/detail/http_query_parser.hpp>
#include "TestUtils.hpp"

using namespace ossia;
using namespace ossia::net;
TEST_CASE ("test_http", "test_http")
{
  {
    auto res = ossia::oscquery::parse_http_methods_encoded("foo=bar");
    REQUIRE(res.size() == 1);
    REQUIRE(res.find("foo") != res.end());
    REQUIRE(res.find("foo").value() == "bar");
  }

  {
    auto res = ossia::oscquery::parse_http_methods_encoded("LISTEN=1");
    REQUIRE(res.size() == 1);
    REQUIRE(res.find("LISTEN") != res.end());
    REQUIRE(res.find("LISTEN").value() == "1");
  }

  {
    auto res = ossia::oscquery::parse_http_methods_encoded("VALUE");
    REQUIRE(res.size() == 1);
    REQUIRE(res.find("VALUE") != res.end());
    REQUIRE(res.find("VALUE").value() == "");
  }

  {
    auto res = ossia::oscquery::parse_http_methods_encoded("foo=bar&baz=blu");
    REQUIRE(res.size() == 2);
    REQUIRE(res.find("foo") != res.end());
    REQUIRE(res.find("foo").value() == "bar");
    REQUIRE(res.find("baz") != res.end());
    REQUIRE(res.find("baz").value() == "blu");
  }
}

TEST_CASE ("test_parse", "test_parse")
{
  generic_device dev{"A"};

  // Create a node
  {
    auto& n = find_or_create_node(dev, "/main");
    auto a = n.create_parameter(ossia::val_type::FLOAT);

    a->push_value(6);

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
  }

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
