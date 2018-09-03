// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/device.hpp>

#include <ossia/context.hpp>
#include <ossia-qt/device/qml_device.hpp>
#include <QQmlEngine>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlComponent>


TEST_CASE ("test_client", "test_client")
{
  int argc{}; char** argv{};
  QCoreApplication app(argc, argv);
  QQmlEngine engine;
  ossia::context context;

  QQmlComponent component(&engine, "testdata/qml/TestClient.qml");
  qDebug() << component.errorString();

  auto item = component.create();
  REQUIRE(item);
}


TEST_CASE ("test_device", "test_device")
{
  int argc{}; char** argv{};
  QCoreApplication app(argc, argv);
  QQmlEngine engine;
  ossia::context context;

  QQmlComponent component(&engine, "testdata/qml/TestDevice.qml");
  qDebug() << component.errorString();

  auto item = component.create();
  REQUIRE(item);

  auto& dev = ossia::qt::qml_singleton_device::instance();
  auto node = ossia::net::find_node(dev.device().get_root_node(), "/bar/x");
  REQUIRE(node);
}

