// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia-c/preset/preset.hpp>
#include <QQmlEngine>
#include <QQmlComponent>

class PresetTest : public QObject
{
  Q_OBJECT

private slots:
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

    auto a1 = n1.create_address(ossia::val_type::INT);
    auto a2 = n2.create_address(ossia::val_type::FLOAT);
    auto a3 = n3.create_address(ossia::val_type::STRING);
    auto a4 = n4.create_address(ossia::val_type::STRING);

    ossia::net::set_default_value(n1, 1234);
    ossia::net::set_default_value(n2, 5678.);
    ossia::net::set_default_value(n3, "hello"s);
    ossia::net::set_default_value(n4, "bye"s);

    a1->push_value(13579);
    a2->push_value(3.1415);
    a3->push_value("foo"s);
    a4->push_value("bar"s);

    auto preset = ossia::devices::make_preset(dev);
    qDebug() << preset.begin()->first.c_str();
    auto presetJSON = ossia::presets::write_json("mydevice", preset);
    qDebug() << presetJSON.c_str();

    auto str = ossia::devices::write_json(dev);
    qDebug() << str.c_str();
  }
};


QTEST_APPLESS_MAIN(PresetTest)

#include "PresetTest.moc"

