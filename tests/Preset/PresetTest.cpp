// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/config.hpp>

#include <ossia/detail/json.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/value/detail/value_parse_impl.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/preset/preset.hpp>

#include "include_catch.hpp"

#include <iostream>

namespace ossia
{
std::ostream& operator<<(std::ostream& o, const ossia::presets::preset& p)
{
  o << "preset: [ ";
  for(auto& [k, v] : p)
  {
    o << k << " = " << ossia::value_to_pretty_string(v) << " ; ";
  }
  o << " ]";
  return o;
}
}

TEST_CASE("test_device", "test_device")
{
  using namespace std::literals;

  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();
  ossia::net::set_app_creator(root, "test"s);
  ossia::net::set_app_version(root, "v1.0"s);

  auto& n1 = ossia::net::find_or_create_node(root, "/foo/bar/baz");
  auto& n2 = ossia::net::find_or_create_node(root, "/bim/bam");
  auto& n3 = ossia::net::find_or_create_node(root, "/bim/boum");
  auto& n4 = ossia::net::find_or_create_node(root, "/bim/boum.1");

  auto a1 = n1.create_parameter(ossia::val_type::INT);
  auto a2 = n2.create_parameter(ossia::val_type::FLOAT);
  auto a3 = n3.create_parameter(ossia::val_type::STRING);
  auto a4 = n4.create_parameter(ossia::val_type::STRING);

  ossia::net::set_default_value(n1, 1234);
  ossia::net::set_default_value(n2, 5678.);
  ossia::net::set_default_value(n3, "hello"s);
  ossia::net::set_default_value(n4, "bye"s);

  a1->push_value(13579);
  a2->push_value(3.1415);
  a3->push_value("foo"s);
  a4->push_value("bar"s);

  auto preset = ossia::presets::make_preset(dev);
  auto presetJSON = ossia::presets::write_json("mydevice", preset);
  ossia::presets::write_json(dev);
}

TEST_CASE("test_nodes", "test_nodes")
{
  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();

  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.1"));
  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.2"));
  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.3"));
  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.4"));
  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.5"));

  auto preset = ossia::presets::make_preset(dev);
  auto presetJSON = ossia::presets::write_json("mydevice", preset);
}

TEST_CASE("test_parse", "test_parse")
{
  for(std::string str : {"/foo/bar/baz", "/foo/bar.1", "/", "/a", "/.0", "/.1"})
  {
    std::string res;
    bool ok = boost::spirit::x3::phrase_parse(
        str.begin(), str.end(), ossia::detail::parse::address_,
        boost::spirit::x3::ascii::space, res);
    REQUIRE(ok);
  }

  {
    std::string str = "vec4f: [0, 0, 0, 0]";
    auto beg = str.begin(), end = str.end();
    std::array<float, 4> v;
    bool ok = boost::spirit::x3::phrase_parse(
        beg, end, ossia::detail::parse::o_vec4_, boost::spirit::x3::ascii::space, v);
    REQUIRE(ok);
  }

  for(std::string str :
      {"vec4f: [0, 0, 0, 0]", "int: 1234", "string: \"hello\"", "char: 'x'",
       "float: 1.2345", "bool: true", "bool: false", "list: []", "list: [ ]",
       "list: [char: '0']", "list: [ char: '0' ]", "list: [ int: 1234 ]",
       "list: [ char: '0', int: 1234 ]", "list: [char: '0',int: 1234]"})
  {
    ossia::value v;
    auto beg = str.begin(), end = str.end();
    bool ok = boost::spirit::x3::phrase_parse(
        beg, end, ossia::detail::parse::value_, boost::spirit::x3::ascii::space, v);
    REQUIRE(ok);
  }

  for(std::string str : {"/foo/bar.1\t", "/foo/bar/baz\t"})
  {
    std::string v;
    using namespace ossia::detail::parse;
    auto beg = str.begin(), end = str.end();
    bool ok = boost::spirit::x3::phrase_parse(
        beg, end, x3::lexeme[address_ >> x3::lit("\t")], boost::spirit::x3::ascii::space,
        v);
    REQUIRE(ok);
  }

  for(std::string str :
      {"/foo/bar.1\tint: 1234", "/foo/bar/baz\tvec4f: [0, 0, 0, 0]",
       "/\tstring: \"hello\"", "/a\tchar: 'x'", "/.0\tfloat: 1.2345",
       "/.1\tlist: [ char: '0', int: 1234 ]"})
  {
    ossia::presets::preset_pair v;
    auto beg = str.begin(), end = str.end();
    bool ok = boost::spirit::x3::phrase_parse(
        beg, end, ossia::detail::parse::preset_pair_, boost::spirit::x3::ascii::space,
        v);
    REQUIRE(ok);
  }
}

TEST_CASE("test_nodes_txt", "test_nodes_txt")
{
  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();

  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.1"));
  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.2"));
  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.3"));
  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.4"));
  ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.5"));

  auto preset = ossia::presets::make_preset(dev);
  auto presetStr = ossia::presets::to_string(preset);

  auto loadPreset = ossia::presets::from_string(presetStr);
  auto presetStr2 = ossia::presets::to_string(loadPreset);
  REQUIRE(loadPreset == preset);
}

TEST_CASE("test_vec2f", "test_vec2f")
{
  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();

  auto p1 = ossia::try_setup_parameter("vec2", ossia::net::create_node(root, "/vec2"));
  p1->push_value(ossia::make_vec(1., -1.));

  const auto preset = ossia::presets::make_preset(dev);
  const auto presetStr = ossia::presets::to_string(preset);

  const auto loadPreset = ossia::presets::from_string(presetStr);
  auto presetStr2 = ossia::presets::to_string(loadPreset);
  REQUIRE(loadPreset == preset);

  const auto json = ossia::presets::write_json("whatever", loadPreset);
  const auto read_json = ossia::presets::read_json(json, false);
  ossia::presets::to_string(read_json);
}

TEST_CASE("test_vec3f", "test_vec3f")
{
  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();

  auto p2 = ossia::try_setup_parameter("vec3", ossia::net::create_node(root, "/vec3"));
  p2->push_value(ossia::make_vec(2., -2.5, -123.));

  const auto preset = ossia::presets::make_preset(dev);
  const auto presetStr = ossia::presets::to_string(preset);

  const auto loadPreset = ossia::presets::from_string(presetStr);
  auto presetStr2 = ossia::presets::to_string(loadPreset);
  REQUIRE(loadPreset == preset);

  const auto json = ossia::presets::write_json("whatever", loadPreset);
  const auto read_json = ossia::presets::read_json(json, false);
  ossia::presets::to_string(read_json);
}

TEST_CASE("test_vec4f", "test_vec4f")
{
  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();

  auto p3 = ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/vec4"));
  p3->push_value(ossia::make_vec(1e2, -0., 999, 1.));

  const auto preset = ossia::presets::make_preset(dev);
  const auto presetStr = ossia::presets::to_string(preset);

  const auto loadPreset = ossia::presets::from_string(presetStr);
  auto presetStr2 = ossia::presets::to_string(loadPreset);
  REQUIRE(loadPreset == preset);

  const auto json = ossia::presets::write_json("whatever", loadPreset);
  const auto read_json = ossia::presets::read_json(json, false);
  ossia::presets::to_string(read_json);
}

TEST_CASE("test_list", "test_list")
{
  static const ossia::value values[]
      = {std::vector<ossia::value>{},
         std::vector<ossia::value>{1, 12, 17},
         std::vector<ossia::value>{-4., 0.2},
         std::vector<ossia::value>{"toto", "foo"},
         std::vector<ossia::value>{'a', 'b', 'c'},
         std::vector<ossia::value>{
             std::vector<ossia::value>{}, std::vector<ossia::value>{},
             std::vector<ossia::value>{}},
         std::vector<ossia::value>{ossia::vec2f{}, ossia::vec3f{}, ossia::vec4f{}},
         std::vector<ossia::value>{
             1, 12, 17, -4, 5, .2, 15, "toto", 'a',
             std::vector<ossia::value>{"foo", "bar", 0.25}, std::vector<ossia::value>{}}

      };
  ossia::value val = GENERATE(
      values[0], values[1], values[2], values[3], values[4], values[5], values[6],
      values[7]);

  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();

  auto p4 = ossia::try_setup_parameter("list", ossia::net::create_node(root, "/list"));
  p4->push_value(val);

  const auto preset = ossia::presets::make_preset(dev);
  const auto presetStr = ossia::presets::to_string(preset);

  const auto loadPreset = ossia::presets::from_string(presetStr);
  auto presetStr2 = ossia::presets::to_string(loadPreset);
  INFO("preset: => ");
  INFO(preset);
  INFO("presetStr: => ");
  INFO(presetStr);
  REQUIRE(loadPreset == preset);

  const auto json = ossia::presets::write_json("whatever", loadPreset);
  const auto read_json = ossia::presets::read_json(json, false);
  ossia::presets::to_string(read_json);
}

TEST_CASE("test_values", "test_values")
{
  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();

  auto p1 = ossia::try_setup_parameter("int", ossia::net::create_node(root, "/my_int"));
  auto p2
      = ossia::try_setup_parameter("float", ossia::net::create_node(root, "/my_float"));
  auto p3
      = ossia::try_setup_parameter("char", ossia::net::create_node(root, "/my_char"));
  auto p4
      = ossia::try_setup_parameter("bool", ossia::net::create_node(root, "/my_bool"));

  p1->push_value(-5);
  p2->push_value(1e2);
  p3->push_value('e');
  p4->push_value(true);

  const auto preset = ossia::presets::make_preset(dev);
  const auto presetStr = ossia::presets::to_string(preset);

  const auto loadPreset = ossia::presets::from_string(presetStr);
  auto presetStr2 = ossia::presets::to_string(loadPreset);
  REQUIRE(loadPreset == preset);

  const auto json = ossia::presets::write_json("whatever", loadPreset);
  const auto read_json = ossia::presets::read_json(json, false);
  ossia::presets::to_string(read_json);
}

TEST_CASE("test_bool", "test_bool")
{
  using namespace std::literals;
  ossia::net::generic_device dev{"mydevice"};
  auto& root = dev.get_root_node();
  ossia::net::set_app_creator(root, "test"s);
  ossia::net::set_app_version(root, "v1.0"s);

  auto& n1 = ossia::net::find_or_create_node(root, "/t");
  auto a1 = n1.create_parameter(ossia::val_type::BOOL);
  a1->push_value(true);

  auto& n2 = ossia::net::find_or_create_node(root, "/f");
  auto a2 = n2.create_parameter(ossia::val_type::BOOL);
  a2->push_value(false);

  auto preset = ossia::presets::make_preset(dev);
  {
    auto presetJSON = ossia::presets::write_json("mydevice", preset);
    a1->push_value(false);
    a2->push_value(true);
    auto np = ossia::presets::read_json(presetJSON);

    REQUIRE(preset == np);
    REQUIRE_NOTHROW([&] {
      ossia::presets::apply_preset(
          root, np, ossia::presets::keep_arch_on, {}, false, false);
    }());
    REQUIRE(a1->value() == ossia::value{true});
    REQUIRE(a2->value() == ossia::value{false});
  }
  {
    auto presetTXT = ossia::presets::to_string(preset);
    a1->push_value(false);
    a2->push_value(true);
    auto np = ossia::presets::from_string(presetTXT);
    REQUIRE(preset == np);
    REQUIRE_NOTHROW([&] {
      ossia::presets::apply_preset(
          root, np, ossia::presets::keep_arch_on, {}, false, false);
    }());
    REQUIRE(a1->value() == ossia::value{true});
    REQUIRE(a2->value() == ossia::value{false});
  }
}

TEST_CASE("test_impulse_json", "test_impulse")
{
  GIVEN("A device with impulse members")
  {
    ossia::net::generic_device dev{"mydevice"};

    auto& root = dev.get_root_node();

    ossia::try_setup_parameter("float", ossia::net::create_node(root, "/foo"));
    ossia::try_setup_parameter("impulse", ossia::net::create_node(root, "/bar"));

    auto preset = ossia::presets::make_preset(dev);

    THEN("The base preset does not have them")
    {
      REQUIRE(preset.size() == 1);
      REQUIRE(preset[0].first == "/foo");
      REQUIRE(preset[0].second == ossia::value(0.f));
    }

    THEN("The preset -> json preset does not have them")
    {
      auto presetJSON = ossia::presets::write_json("mydevice", preset);
      rapidjson::Document doc;
      doc.Parse(presetJSON);
      REQUIRE(doc.IsObject());
      REQUIRE(doc["mydevice"].IsObject());
      REQUIRE(doc["mydevice"]["foo"].IsNumber());
      REQUIRE(doc["mydevice"].FindMember("bar") == doc["mydevice"].MemberEnd());
    }

    THEN("The json preset does not have them")
    {
      auto presetJSON = ossia::presets::make_json_preset(root);
      INFO(presetJSON);
      rapidjson::Document doc;
      doc.Parse(presetJSON);
      REQUIRE(doc.IsObject());
      REQUIRE(doc["foo"].IsNumber());
      REQUIRE(doc.FindMember("bar") == doc.MemberEnd());
    }
  }
}

TEST_CASE("test_get_set_bi_json", "test_get_set_bi")
{
  GIVEN("A device with bi/get/set members")
  {
    ossia::net::generic_device dev{"mydevice"};

    auto& root = dev.get_root_node();

    ossia::try_setup_parameter("float", ossia::net::create_node(root, "/get"))
        ->set_access(ossia::access_mode::GET);
    ossia::try_setup_parameter("float", ossia::net::create_node(root, "/set"))
        ->set_access(ossia::access_mode::SET);
    ossia::try_setup_parameter("float", ossia::net::create_node(root, "/bi"))
        ->set_access(ossia::access_mode::BI);

    auto preset = ossia::presets::make_preset(dev);

    THEN("The base preset does not have them")
    {
      REQUIRE(preset.size() == 1);
      REQUIRE(preset[0].first == "/bi");
      REQUIRE(preset[0].second == ossia::value(0.f));
    }

    THEN("The preset -> json preset does not have them")
    {
      auto presetJSON = ossia::presets::write_json("mydevice", preset);

      rapidjson::Document doc;
      doc.Parse(presetJSON);
      REQUIRE(doc.IsObject());
      REQUIRE(doc["mydevice"].IsObject());
      REQUIRE(doc["mydevice"].FindMember("get") == doc["mydevice"].MemberEnd());
      REQUIRE(doc["mydevice"].FindMember("set") == doc["mydevice"].MemberEnd());
      REQUIRE(doc["mydevice"]["bi"].IsNumber());
    }

    THEN("The json preset does not have them")
    {
      auto presetJSON = ossia::presets::make_json_preset(root);
      INFO(presetJSON);
      rapidjson::Document doc;
      doc.Parse(presetJSON);
      REQUIRE(doc.IsObject());
      REQUIRE(doc["bi"].IsNumber());
      REQUIRE(doc.FindMember("get") == doc.MemberEnd());
      REQUIRE(doc.FindMember("set") == doc.MemberEnd());
    }
  }
}
