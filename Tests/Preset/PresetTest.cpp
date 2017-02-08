#include <QtTest>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/address.hpp>
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

    ossia::net::generic_device dev{std::make_unique<ossia::net::local_protocol>(), "mydevice"};

    auto& root = dev.getRootNode();
    ossia::net::set_app_creator(root, "test"s);
    ossia::net::set_app_version(root, "v1.0"s);

    auto& n1 = ossia::net::find_or_create_node(root, "/foo/bar/baz");
    auto& n2 = ossia::net::find_or_create_node(root, "/bim/bam");
    auto& n3 = ossia::net::find_or_create_node(root, "/bim/boum");
    auto& n4 = ossia::net::find_or_create_node(root, "/bim/boum.1");

    n1.createAddress(ossia::val_type::INT);
    n2.createAddress(ossia::val_type::FLOAT);
    n3.createAddress(ossia::val_type::STRING);
    n4.createAddress(ossia::val_type::STRING);

    ossia::net::set_default_value(n1, 1234);
    ossia::net::set_default_value(n2, 5678.);
    ossia::net::set_default_value(n3, "hello"s);
    ossia::net::set_default_value(n4, "bye"s);

    auto str = ossia::devices::write_json(dev);
    qDebug() << str.c_str();
  }
};


QTEST_APPLESS_MAIN(PresetTest)

#include "PresetTest.moc"

