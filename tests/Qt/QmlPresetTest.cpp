// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/context.hpp>
#include <fmt/format.h>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QDir>
#include <regex>
#define private public
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_model_property.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/qml_plugin.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/domain/domain.hpp>

static int n = 0;
void dumpTree(QQuickItem* root);
void dumpTree(QObject* root)
{
  QString sep(n, ' ');
  qDebug() << qPrintable(sep) << root;
  n++;
  for(auto cld : root->children())
  {
    if(auto item = qobject_cast<QQuickItem*>(cld))
    {
      dumpTree(item);
    }
    else
    {
      dumpTree(cld);
    }
  }
  n--;
}
void dumpTree(QQuickItem* root)
{
  QString sep(n, ' ');
  qDebug() << qPrintable(sep) << root;
  if(auto obj = qobject_cast<ossia::qt::qml_node_base*>(root))
    qDebug() << qPrintable(sep) << obj->node();
  n++;
  std::set<QObject*> dumped;
  for(auto cld : root->children())
  {
    dumpTree(cld);
    dumped.insert(cld);
  }
  for(auto cld : root->childItems())
  {
    if(dumped.find((QObject*)cld) == dumped.end())
      dumpTree(cld);
  }
  n--;
}


void print_device()
{
  auto& dev = ossia::qt::qml_singleton_device::instance();
  std::string c; ossia::net::debug_recursively(c, dev.device().get_root_node());
  qDebug() << c.c_str();
}

void cleanup(QObject* item)
{
  QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
  for(auto cld : item->children()) {
    QQmlEngine::setObjectOwnership(cld, QQmlEngine::CppOwnership);
    delete cld;
  }
  delete item;
  for(int i = 0; i < 10; i++)
  {
    QCoreApplication::instance()->processEvents();
  }

  auto& dev = ossia::qt::qml_singleton_device::instance();
  dev.cleanup();
}


TEST_CASE ("test_model_recursive_static", "test_model_recursive_static")
{
  auto& dev = ossia::qt::qml_singleton_device::instance();
  int argc{}; char** argv{};
  QCoreApplication app(argc, argv);
  ossia::context context;
  QQmlEngine engine;

  engine.addImportPath(QDir().absolutePath() + "/testdata/qml");
  engine.addPluginPath(QDir().absolutePath() + "/testdata/qml");

  {
    QQmlComponent component(&engine);
    component.setData(R"_(
                      import Ossia 1.0 as Ossia
                      import QtQuick 2.5

                      Item {
                      Repeater {
                      model: 2
                      Item {
                      id: tutu
                      Ossia.Node { node: "foo." + index; id: n }
                      Ossia.Property on x { parentNode: n }
                      Ossia.Property on y { parentNode: n }

                      Item {
                      id: tata
                      parent: tutu

                      Ossia.Node {
                      id: sub
                      node: "tata"
                      parentNode: n
                      }
                      Repeater {
                      model: 2
                      Item {
                      Ossia.Node {
                      node: "buzz." + index; id: subn; parentNode: sub;
                      Component.onCompleted: console.log("Build  buzz", index)
                      }
                      Ossia.Property on x { parentNode: subn }
                      Ossia.Property on y { parentNode: subn }
                      }
                      }
                      }
                      }
                      }
                      }
                      )_", QUrl{});

    qDebug() << component.errorString();
    REQUIRE(component.errors().empty());
    auto item = (QQuickItem*) component.create();
    REQUIRE(item);

    dev.recreate(item);

    //item->dumpObjectTree();
    //dumpTree(item);

    dev.savePreset(QUrl::fromLocalFile("/tmp/preset.json"));

    dev.saveDevice(QUrl::fromLocalFile("/tmp/device.json"));
    print_device();


    for(auto node : {
        "/foo.0/x", "/foo.0/y", "/foo.1/x", "/foo.1/y",
        "/foo.0/tata/buzz.0/x",
        "/foo.0/tata/buzz.0/y",
        "/foo.0/tata/buzz.1/x",
        "/foo.0/tata/buzz.1/y",
        "/foo.1/tata/buzz.0/x",
        "/foo.1/tata/buzz.0/y",
        "/foo.1/tata/buzz.1/x",
        "/foo.1/tata/buzz.1/y",
  })
    {
      qDebug() << node;
      auto found = ossia::net::find_node(dev.device().get_root_node(), node);
      REQUIRE(found);
    }

    cleanup(item);
  }
}
TEST_CASE ("test_qml_repeater", "test_qml_repeater")
{

  int argc{}; char** argv{};
  QCoreApplication app(argc, argv);
  ossia::context context;
  QQmlEngine engine;
  engine.addImportPath(QDir().absolutePath() + "/testdata/qml");
  engine.addPluginPath(QDir().absolutePath() + "/testdata/qml");

  QFile f(QDir().absolutePath() + "/testdata/qml/TestQmlRepeater.qml");
  f.open(QIODevice::ReadOnly);
  QQmlComponent component(&engine);
  component.setData(f.readAll(), QUrl());

  qDebug() << component.errorString();
  REQUIRE(component.errors().empty());
  auto item = (QQuickItem*) component.create();
  REQUIRE(item);

  for(int i = 0; i < 100; i++)
    qApp->processEvents();
  dumpTree(item);
}

// does not work on Qt on windows :
// https://bugreports.qt.io/browse/QTBUG-65261
#if !defined(_MSC_VER)
TEST_CASE ("test_model_recursive_preset", "test_model_recursive_preset")
{
  int argc{}; char** argv{};
  QCoreApplication app(argc, argv);
  ossia::context context;
  QQmlEngine engine;

  engine.addImportPath(QDir().absolutePath() + "/testdata/qml");
  engine.addPluginPath(QDir().absolutePath() + "/testdata/qml");
  auto& dev = ossia::qt::qml_singleton_device::instance();

  {
    QFile f(QDir().absolutePath() + "/testdata/qml/TestRecursivePreset.qml");
    f.open(QIODevice::ReadOnly);
    QQmlComponent component(&engine);
    component.setData(f.readAll(), QUrl());

    qDebug() << component.errorString();
    REQUIRE(component.errors().empty());
    auto item = (QQuickItem*) component.create();
    REQUIRE(item);

    print_device();

    dev.loadPreset(item, QDir().absolutePath() + "/testdata/qml/recursive_model_preset.json");
    //dev.recreate(item);

    print_device();

    auto check_preset = [&]
    {
      auto& root = dev.device().get_root_node();
      for(auto node : {
          "/foo.0/x", "/foo.0/y",
          "/foo.1/x", "/foo.1/y",
          "/foo.0/tata/bar.0/x",
          "/foo.0/tata/bar.0/y",
          "/foo.0/tata/bar.1/x",
          "/foo.0/tata/bar.1/y",
          "/foo.1/tata/bar.0/x",
          "/foo.1/tata/bar.0/y",
          "/foo.1/tata/bar.0/papa",
          "/foo.1/tata/bar.0/papa/baz.0",
          "/foo.1/tata/bar.0/papa/baz.0/x",
          "/foo.1/tata/bar.0/papa/baz.0/y",
          "/foo.1/tata/bar.0/papa/baz.1/x",
          "/foo.1/tata/bar.0/papa/baz.1/y",
          "/foo.1/tata/bar.1/x",
          "/foo.1/tata/bar.1/y",
    })
      {
        auto found = ossia::net::find_node(root, node);
        REQUIRE(found);
      }
      REQUIRE(root.children().size() == 2);
      auto& foo_0 = *root.children()[0];
      REQUIRE(foo_0.children().size() == 3);
      auto& foo_0_tata = *foo_0.find_child(ossia::string_view("tata"));
      REQUIRE(foo_0_tata.children().size() == 2);
      REQUIRE(foo_0_tata.children()[0]->children().size() == 3);
      REQUIRE(foo_0_tata.children()[1]->children().size() == 3);

      auto& foo_1 = *root.children()[1];
      REQUIRE(foo_1.children().size() == 3);
      auto& foo_1_tata = *foo_1.find_child(ossia::string_view("tata"));
      REQUIRE(foo_1_tata.children().size() == 2);
      REQUIRE(foo_1_tata.children()[0]->children().size() == 3);
      REQUIRE(foo_1_tata.children()[1]->children().size() == 3);

      /*
          dumpTree(item);
          dumpTree(&engine);
          dumpTree(engine.rootContext());

          auto obj00 = item->findChild<QQuickItem*>("root0RectChild0");
          auto obj01 = item->findChild<QQuickItem*>("root0RectChild1");
          auto obj10 = item->findChild<QQuickItem*>("root1RectChild0");
          auto obj11 = item->findChild<QQuickItem*>("root1RectChild1");
          REQUIRE(obj00);
          REQUIRE(obj01);
          REQUIRE(obj10);
          REQUIRE(obj11);

          REQUIRE(obj00->position() == (QPointF{0, 0}));
          REQUIRE(obj01->position() == (QPointF{5, 8}));
          REQUIRE(obj10->position() == (QPointF{1, 2}));
          REQUIRE(obj11->position() == (QPointF{3, 4}));
          */
    };
    for(int i = 0; i < 10; i++)
      qApp->processEvents();
    check_preset();

    dev.loadPreset(item, QDir().absolutePath() + "/testdata/qml/recursive_model_preset.json");

    for(int i = 0; i < 10; i++)
      qApp->processEvents();

    check_preset();
    dev.loadPreset(item, QDir().absolutePath() + "/testdata/qml/recursive_model_preset.json");

    for(int i = 0; i < 10; i++)
      qApp->processEvents();
    check_preset();
    dev.loadPreset(item, QDir().absolutePath() + "/testdata/qml/recursive_model_preset.json");

    for(int i = 0; i < 10; i++)
      qApp->processEvents();
    check_preset();
    cleanup(item);
  }
}
#endif

TEST_CASE ("test_model_preset_2", "test_model_preset_2")
{
  int argc{}; char** argv{};
  QCoreApplication app(argc, argv);
  ossia::context context;
  QQmlEngine engine;

  engine.addImportPath(QDir().absolutePath() + "/testdata/qml");
  engine.addPluginPath(QDir().absolutePath() + "/testdata/qml");
  auto& dev = ossia::qt::qml_singleton_device::instance();
  dev.cleanup();

  {
    QQmlComponent component(&engine);
    component.setData(R"_(
                      import QtQuick 2.2
                      import Ossia 1.0 as Ossia

                      Item{
                      id: chBoU
                      Ossia.Node { node: "chocolateBoxes"; id: chocoBoxesData }

                      Repeater {
                      id: boxData
                      model: Ossia.Instances {

                      node: "boxes";
                      count: 6;
                      parentNode: chocoBoxesData;
                      onCountChanged: { console.log("box count: " + count); gc(); }
                      }

                      delegate: Item
                      {
                      id: deleg
                      parent: chBoU
                      Component.onDestruction: { gc(); }
                      Ossia.Node {
                      node: "boxes." + index;
                      id: boxNode ;
                      parent: deleg;
                      parentNode: chocoBoxesData
                      //Component.onCompleted: console.log(node + " was created")
                      //Component.onDestruction: { console.log(node + " was destroyed"); gc(); }
                      }

                      property real weight;
                      Ossia.Property on weight { parentNode: boxNode }

                      property real boxWidth
                      Ossia.Property on boxWidth { parentNode: boxNode }

                      property real boxHeight
                      Ossia.Property on boxHeight { parentNode: boxNode }

                      property string imageFile;
                      Ossia.Property on imageFile { parentNode: boxNode }

                      property string defaultImagePath:"torino noir.png"
                      }
                      }
                      }
                      )_", QUrl{});

    qDebug() << component.errorString();
    REQUIRE(component.errors().empty());
    auto item = (QQuickItem*) component.create();
    REQUIRE(item);
    dev.recreate(item);
    app.processEvents();

    qDebug() << item->findChildren<ossia::qt::qml_node_base*>().size();
    qDebug() << dev.m_nodes.size();
    REQUIRE(dev.m_nodes.size() == 7);
    for(auto node :  dev.m_nodes)
    {
      REQUIRE(node.first);
      REQUIRE(node.first->ossiaNode());
    }

    {
      auto chocobox = ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes");
      REQUIRE(chocobox);
      qDebug() << chocobox->children().size();
      REQUIRE(chocobox->children().size() == 6);
      REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.0"));
      REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.1"));
      REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.2"));
      REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.3"));
      REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.4"));
      REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.5"));
    }

    for(int i = 0; i < 2; i++)
    {
      dev.loadPreset(item, QDir().absolutePath() + "/testdata/qml/preset_choco.json");

      for(int i = 0; i < 100; i++)
        app.processEvents();
      qDebug() << "HOW MANY NODES" << dev.m_nodes.size();

      /*
          REQUIRE(dev.m_nodes.size() == 7);
          for(auto node :  dev.m_nodes)
          {
            REQUIRE(node.first);
            REQUIRE(node.first->ossiaNode());
          }
          */
      {
        auto chocobox = ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes");
        REQUIRE(chocobox);
        REQUIRE(chocobox->children().size() == 6);
        REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.0"));
        REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.1"));
        REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.2"));
        REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.3"));
        REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.4"));
        REQUIRE(ossia::net::find_node(dev.device().get_root_node(), "/chocolateBoxes/boxes.5"));
      }

      qDebug(" ===== PRESET LOADED SUCCESSFULLY ");
    }
    cleanup(item);
  }
}
