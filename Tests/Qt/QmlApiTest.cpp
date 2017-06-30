#include <QtTest>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/context.hpp>
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/qml_plugin.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/domain/domain.hpp>
#include <fmt/format.h>
#include <QQmlEngine>
#include <QQmlComponent>

class QmlApiTest : public QObject
{
    Q_OBJECT

    void cleanup(QObject* item)
    {
      QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
      delete item;
      QCoreApplication::instance()->processEvents();
      QCoreApplication::instance()->processEvents();
      QCoreApplication::instance()->processEvents();
      QCoreApplication::instance()->processEvents();
    }

  private slots:
    void test_import()
    {
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
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);
        cleanup(item);
      }
    }

    void test_parameter()
    {
      int argc{}; char** argv{};
      QCoreApplication app(argc, argv);
      ossia::context context;
      QQmlEngine engine;

      engine.addImportPath(QDir().absolutePath() + "/testdata/qml");
      engine.addPluginPath(QDir().absolutePath() + "/testdata/qml");

      auto& dev = ossia::qt::qml_singleton_device::instance();

      {
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import Ossia 1.0 as Ossia
                          import QtQuick 2.5

                          Item {
                          Ossia.Parameter {
                            valueType: Ossia.Type.Float
                            node: "/foo/bar/float"
                            min: -5
                            max: 26
                            value: 12
                            access: Ossia.Access.Get
                            bounding: Ossia.Bounding.Wrap
                            filterRepetitions: Ossia.Repetitions.Filtered
                            unit: "distance.cm"
                          }
                          Component.onCompleted: Ossia.SingleDevice.recreate(this)
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);

        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/foo/bar/float");
          QVERIFY(node);
          auto addr = node->get_address();
          QVERIFY(addr);
          QVERIFY(addr->get_value_type() == ossia::val_type::FLOAT);
          QVERIFY(addr->value() == 12.);
          QVERIFY(addr->get_domain() == ossia::make_domain(-5., 26.));
          QVERIFY(addr->get_access() == ossia::access_mode::GET);
          QVERIFY(addr->get_bounding() == ossia::bounding_mode::WRAP);
          QVERIFY(addr->get_repetition_filter() == ossia::repetition_filter::ON);
          QVERIFY(addr->get_unit() == ossia::centimeter_u{});
        }
        cleanup(item);

      }
    }

    void test_property()
    {
      int argc{}; char** argv{};
      QCoreApplication app(argc, argv);
      ossia::context context;
      QQmlEngine engine;

      engine.addImportPath(QDir().absolutePath() + "/testdata/qml");
      engine.addPluginPath(QDir().absolutePath() + "/testdata/qml");

      auto& dev = ossia::qt::qml_singleton_device::instance();

      {
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import Ossia 1.0 as Ossia
                          import QtQuick 2.5

                          Item {
                          Ossia.Property on x { }
                          Ossia.Property on y { }
                          property int theInt: 123; Ossia.Property on theInt { }
                          property real theFloat: 456.; Ossia.Property on theFloat { }
                          property double theDouble: 789.; Ossia.Property on theDouble { }
                          property string theString: "banana"; Ossia.Property on theString { }
                          property bool theBool: true; Ossia.Property on theBool { }
                          property color theColor: Qt.rgba(1, 1, 1, 1); Ossia.Property on theColor { }
                          property point thePoint: Qt.point(2, 2);  Ossia.Property on thePoint { }
                          property vector2d theVector2D: Qt.vector2d(2, 2);  Ossia.Property on theVector2D { }
                          property vector3d theVector3D: Qt.vector3d(2, 2, 2); Ossia.Property on theVector3D { }
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);

        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/x");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::FLOAT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/y");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::FLOAT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theInt");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::INT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theFloat");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::FLOAT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theDouble");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::FLOAT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theString");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::STRING);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theBool");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::BOOL);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theColor");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::VEC4F);
          QVERIFY(node->get_address()->get_unit() == ossia::argb_u{});
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/thePoint");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::VEC2F);
          QVERIFY(node->get_address()->get_unit() == ossia::cartesian_2d_u{});
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theVector2D");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::VEC2F);
          QVERIFY(node->get_address()->get_unit() == ossia::cartesian_2d_u{});
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theVector3D");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::VEC3F);
          QVERIFY(node->get_address()->get_unit() == ossia::cartesian_3d_u{});
        }
        cleanup(item);
      }
    }

    void test_signal()
    {
      int argc{}; char** argv{};
      QCoreApplication app(argc, argv);
      ossia::context context;
      QQmlEngine engine;

      engine.addImportPath(QDir().absolutePath() + "/testdata/qml");
      engine.addPluginPath(QDir().absolutePath() + "/testdata/qml");

      auto& dev = ossia::qt::qml_singleton_device::instance();

      {
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import QtQuick 2.5
                          import Ossia 1.0 as Ossia

                          Item {
                          Ossia.Signal {
                            node: "buzz"
                            onTriggered: console.log('bang')
                          }

                          Component.onCompleted: Ossia.SingleDevice.recreate(this)
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);

        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/buzz");
          QVERIFY(node);
          QVERIFY(node->get_address());
          QVERIFY(node->get_address()->get_value_type() == ossia::val_type::IMPULSE);
        }
        cleanup(item);
      }
    }

    void test_hierarchy()
    {
      int argc{}; char** argv{};
      QCoreApplication app(argc, argv);
      ossia::context context;
      QQmlEngine engine;

      engine.addImportPath(QDir().absolutePath() + "/testdata/qml");
      engine.addPluginPath(QDir().absolutePath() + "/testdata/qml");

      auto& dev = ossia::qt::qml_singleton_device::instance();

      {
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import QtQuick 2.5
                          import Ossia 1.0 as Ossia

                          Item {
                          Ossia.Node { node: "/foo" }
                          Ossia.Property on x { }
                          Ossia.Property on y { }

                          Component.onCompleted: Ossia.SingleDevice.recreate(this)
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);

        fmt::MemoryWriter c; ossia::net::debug_recursively(c, dev.device().get_root_node());
        std::cerr << c.str();
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/x");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/y");
          QVERIFY(node);
        }
        cleanup(item);
      }


      {
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import QtQuick 2.5
                          import Ossia 1.0 as Ossia

                          Item {
                          Ossia.Property on x { }
                          Ossia.Property on y { }
                          Ossia.Node { node: "/foo" }

                          Component.onCompleted: Ossia.SingleDevice.recreate(this)
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);

        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/x");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/y");
          QVERIFY(node);
        }
        cleanup(item);
      }


      {
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import QtQuick 2.5
                          import Ossia 1.0 as Ossia

                          Item {
                          Ossia.Node { node: "/foo" }
                          Item {
                          Ossia.Property on x { }
                          Ossia.Property on y { }
                          }

                          Component.onCompleted: Ossia.SingleDevice.recreate(this)
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);

        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/x");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/y");
          QVERIFY(node);
        }
        cleanup(item);
      }
      {
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import QtQuick 2.5
                          import Ossia 1.0 as Ossia

                          Item {
                          Ossia.Node { node: "/foo" }
                          Item {
                          Ossia.Node { node: "/bar" }
                          Ossia.Property on x { }
                          Ossia.Property on y { }
                          }

                          Component.onCompleted: Ossia.SingleDevice.recreate(this)
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();

        QVERIFY(item);

        fmt::MemoryWriter c; ossia::net::debug_recursively(c, dev.device().get_root_node());
        std::cerr << c.str();
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/bar/x");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/bar/y");
          QVERIFY(node);
        }
        cleanup(item);
      }
      {
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import QtQuick 2.5
                          import Ossia 1.0 as Ossia

                          Item {
                          Ossia.Node { node: "/foo" }
                          Item {
                          Ossia.Node { node: "bar" }
                          Ossia.Property on x { }
                          Ossia.Property on y { }
                          }

                          Component.onCompleted: Ossia.SingleDevice.recreate(this)
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();

        QVERIFY(item);

        fmt::MemoryWriter c; ossia::net::debug_recursively(c, dev.device().get_root_node());
        std::cerr << c.str();
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/bar/x");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/bar/y");
          QVERIFY(node);
        }
        cleanup(item);
      }
      {
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import QtQuick 2.5
                          import Ossia 1.0 as Ossia

                          Item {
                          Ossia.Node { node: "/foo" }
                          Item {
                          Ossia.Node { node: "bar" }
                          Ossia.Property on x { }
                          Ossia.Property on y { }
                          }
                          Item {
                          Ossia.Node { node: "bar" }
                          Ossia.Property on x { }
                          Ossia.Property on y { }
                          }

                          Component.onCompleted: Ossia.SingleDevice.recreate(this)
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);

        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/bar/x");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/bar/y");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/bar.1/x");
          QVERIFY(node);
        }
        {
          auto node = ossia::net::find_node(dev.device().get_root_node(), "/foo/bar.1/y");
          QVERIFY(node);
        }
        cleanup(item);
      }

    }
};


QTEST_APPLESS_MAIN(QmlApiTest)

#include "QmlApiTest.moc"

