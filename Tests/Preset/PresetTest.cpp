// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/value/detail/value_parse_impl.hpp>

TEST_CASE ("test_device", "test_device")
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
  std::cerr << preset.begin()->first;
  auto presetJSON = ossia::presets::write_json("mydevice", preset);
  std::cerr << presetJSON.c_str();

  std::cerr << ossia::presets::write_json(dev);
}

TEST_CASE ("test_nodes", "test_nodes")
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
  std::cerr << presetJSON.c_str();

}

TEST_CASE ("test_parse", "test_parse")
{
  for(std::string str : {
      "/foo/bar/baz",
      "/foo/bar.1",
      "/",
      "/a",
      "/.0",
      "/.1"
}) {
    std::string res;
    bool ok = boost::spirit::x3::phrase_parse(str.begin(), str.end(), ossia::detail::parse::address_,
                                              boost::spirit::x3::ascii::space, res);
    REQUIRE(ok);
  }

  {
    std::string str = "vec4f: [0, 0, 0, 0]";
    auto beg = str.begin(), end = str.end();
    ossia::detail::parse::array_parser<4> v;
    bool ok = boost::spirit::x3::phrase_parse(
          beg, end,
          ossia::detail::parse::o_vec4_,
          boost::spirit::x3::ascii::space,
          v
          );
    REQUIRE(ok);
  }


  for(std::string str : {
      "vec4f: [0, 0, 0, 0]",
      "int: 1234",
      "string: \"hello\"",
      "char: 'x'",
      "float: 1.2345",
      "list: []",
      "list: [ ]",
      "list: [char: '0']",
      "list: [ char: '0' ]",
      "list: [ int: 1234 ]",
      "list: [ char: '0', int: 1234 ]",
      "list: [char: '0',int: 1234]"
}) {
    ossia::value v;
    auto beg = str.begin(), end = str.end();
    bool ok = boost::spirit::x3::phrase_parse(beg, end,
                                              ossia::detail::parse::value_,
                                              boost::spirit::x3::ascii::space,
                                              v);
    std::cerr << str.c_str();
    REQUIRE(ok);
  }

  for(std::string str : {
      "/foo/bar.1\t",
      "/foo/bar/baz\t"
}) {
    std::string v;
    using namespace ossia::detail::parse;
    auto beg = str.begin(), end = str.end();
    bool ok = boost::spirit::x3::phrase_parse(
          beg, end,
          x3::lexeme [ address_ >> x3::lit("\t") ],
        boost::spirit::x3::ascii::space,
        v);
    std::cerr << str.c_str();
    REQUIRE(ok);
  }

  for(std::string str : {
      "/foo/bar.1\tint: 1234",
      "/foo/bar/baz\tvec4f: [0, 0, 0, 0]",
      "/\tstring: \"hello\"",
      "/a\tchar: 'x'",
      "/.0\tfloat: 1.2345",
      "/.1\tlist: [ char: '0', int: 1234 ]"
}) {
    ossia::presets::preset_pair v;
    auto beg = str.begin(), end = str.end();
    bool ok = boost::spirit::x3::phrase_parse(beg, end,
                                              ossia::detail::parse::preset_pair_,
                                              boost::spirit::x3::ascii::space,
                                              v);
    std::cerr << str.c_str();
    REQUIRE(ok);
  }
}

TEST_CASE ("test_nodes_txt", "test_nodes_txt")
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
  std::cerr << presetStr.c_str();

  auto loadPreset = ossia::presets::from_string(presetStr);
  for(auto s : loadPreset) std::cerr << s.first.c_str();
  auto presetStr2 = ossia::presets::to_string(loadPreset);
  std::cerr << presetStr2.c_str();
  REQUIRE(loadPreset == preset);

}

TEST_CASE ("test_vecnf", "test_vecnf")
{
  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();

  auto p1 = ossia::try_setup_parameter("vec2", ossia::net::create_node(root, "/vec2"));
  auto p2 = ossia::try_setup_parameter("vec3", ossia::net::create_node(root, "/vec3"));
  auto p3 = ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/vec4"));
  auto p4 = ossia::try_setup_parameter("list", ossia::net::create_node(root, "/list"));

  p1->push_value(ossia::make_vec(1., -1.));
  p2->push_value(ossia::make_vec(2., -2.5, -123.));
  p3->push_value(ossia::make_vec(1e2, -0., 999, 4.56));
  p4->push_value(std::vector<ossia::value>{1, 12, 17, -4, 5, .2, 15, "toto", 'a', std::vector<ossia::value>{"foo","bar",0.25}, std::vector<ossia::value>{}});

  const auto preset = ossia::presets::make_preset(dev);
  const auto presetStr = ossia::presets::to_string(preset);
  std::cerr << presetStr.c_str();

  const auto loadPreset = ossia::presets::from_string(presetStr);
  for(auto s : loadPreset) std::cerr << s.first.c_str();
  auto presetStr2 = ossia::presets::to_string(loadPreset);
  std::cerr << presetStr2.c_str();
  REQUIRE(loadPreset == preset);

  std::cerr << "Write json";
  const auto json = ossia::presets::write_json("whatever", loadPreset, false);
  std::cerr << json.c_str();
  std::cerr << "Read json";
  const auto read_json = ossia::presets::read_json(json, false);
  std::cerr << ossia::presets::to_string(read_json).c_str();

}

TEST_CASE ("test_values", "test_values")
{
  ossia::net::generic_device dev{"mydevice"};

  auto& root = dev.get_root_node();

  auto p1 = ossia::try_setup_parameter("int", ossia::net::create_node(root, "/my_int"));
  auto p2 = ossia::try_setup_parameter("float", ossia::net::create_node(root, "/my_float"));
  auto p3 = ossia::try_setup_parameter("char", ossia::net::create_node(root, "/my_char"));
  auto p4 = ossia::try_setup_parameter("bool", ossia::net::create_node(root, "/my_bool"));

  p1->push_value(-5);
  p2->push_value(3.141);
  p3->push_value('e');
  p4->push_value(true);

  const auto preset = ossia::presets::make_preset(dev);
  const auto presetStr = ossia::presets::to_string(preset);
  std::cerr << presetStr.c_str();

  const auto loadPreset = ossia::presets::from_string(presetStr);
  for(auto s : loadPreset) std::cerr << s.first.c_str();
  auto presetStr2 = ossia::presets::to_string(loadPreset);
  std::cerr << presetStr2.c_str();
  REQUIRE(loadPreset == preset);

  std::cerr << "Write json";
  const auto json = ossia::presets::write_json("whatever", loadPreset, false);
  std::cerr << json.c_str();
  std::cerr << "Read json";
  const auto read_json = ossia::presets::read_json(json, false);
  std::cerr << ossia::presets::to_string(read_json).c_str();

}
