// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/value/detail/value_parse_impl.hpp>
#include <QQmlEngine>
#include <QQmlComponent>

class PresetTest : public QObject
{
  Q_OBJECT

private Q_SLOTS:
  void test_device()
  {
    using namespace std::literals;

    ossia::net::generic_device dev{std::make_unique<ossia::net::multiplex_protocol>(), "mydevice"};

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
    qDebug() << preset.begin()->first.c_str();
    auto presetJSON = ossia::presets::write_json("mydevice", preset);
    qDebug() << presetJSON.c_str();

    auto str = ossia::presets::write_json(dev);
    qDebug() << str.c_str();
  }

  void test_nodes()
  {

    ossia::net::generic_device dev{std::make_unique<ossia::net::multiplex_protocol>(), "mydevice"};

    auto& root = dev.get_root_node();

    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.1"));
    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.2"));
    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.3"));
    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.4"));
    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.5"));


    auto preset = ossia::presets::make_preset(dev);
    auto presetJSON = ossia::presets::write_json("mydevice", preset);
    qDebug() << presetJSON.c_str();

  }

  void test_parse()
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
      QVERIFY(ok);
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
      QVERIFY(ok);
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
      qDebug() << str.c_str();
      QVERIFY(ok);
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
      qDebug() << str.c_str();
      QVERIFY(ok);
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
      qDebug() << str.c_str();
      QVERIFY(ok);
    }
  }

  void test_nodes_txt()
  {
    ossia::net::generic_device dev{std::make_unique<ossia::net::multiplex_protocol>(), "mydevice"};

    auto& root = dev.get_root_node();

    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.1"));
    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.2"));
    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.3"));
    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.4"));
    ossia::try_setup_parameter("vec4", ossia::net::create_node(root, "/matrix/color.5"));


    auto preset = ossia::presets::make_preset(dev);
    auto presetStr = ossia::presets::to_string(preset);
    qDebug() << presetStr.c_str();

    auto loadPreset = ossia::presets::from_string(presetStr);
    for(auto s : loadPreset) qDebug() << s.first.c_str();
    auto presetStr2 = ossia::presets::to_string(loadPreset);
    qDebug() << presetStr2.c_str();
    QVERIFY(loadPreset == preset);

  }
};


QTEST_APPLESS_MAIN(PresetTest)

#include "PresetTest.moc"

