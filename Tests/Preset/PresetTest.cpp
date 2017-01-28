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
    ossia::net::node_base& n1 = ossia::net::find_or_create_node(root, "/foo/bar/baz");
    (n1.createAddress(ossia::val_type::INT))->setDefaultValue(1234);
    auto& n2 = ossia::net::find_or_create_node(root, "/bim/bam");
    (n2.createAddress(ossia::val_type::FLOAT))->setDefaultValue(5678.);
    auto& n3 = ossia::net::find_or_create_node(root, "/bim/boum");
    (n3.createAddress(ossia::val_type::STRING))->setDefaultValue("hello"s);
    auto& n4 = ossia::net::find_or_create_node(root, "/bim/boum.1");
    (n4.createAddress(ossia::val_type::STRING)->setDefaultValue("bye"s));

    auto str = ossia::devices::write_json(dev);
    qDebug() << str.c_str();
  }
};


QTEST_APPLESS_MAIN(PresetTest)

#include "PresetTest.moc"

