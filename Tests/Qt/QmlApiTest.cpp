// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/context.hpp>
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_model_property.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia-qt/qml_plugin.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/domain/domain.hpp>
#include <fmt/format.h>
#include <QQmlEngine>
#include <QQmlComponent>
#include <ossia-qt/device/qml_logger.hpp>

static int n = 0;
void dumpTree(QQuickItem* root);
void dumpTree(QObject* root)
{
  QString sep(n, ' ');
  qDebug() << qPrintable(sep) << root;
  n++;
  for(auto cld : root->children())
  {
    dumpTree(cld);
    if(auto item = qobject_cast<QQuickItem*>(cld))
    {
      dumpTree(item);
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
class QmlApiTest : public QObject
{
    Q_OBJECT

    void print_device()
    {
      auto& dev = ossia::qt::qml_singleton_device::instance();
      fmt::MemoryWriter c; ossia::net::debug_recursively(c, dev.device().get_root_node());
      qDebug() << c.str().c_str();
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

  private Q_SLOTS:
    void test_logger()
    {
      int argc{}; char** argv{};
      QCoreApplication app(argc, argv);
      ossia::context context;
      ossia::qt::qml_logger log;
      log.setLoggerHost("ws://127.0.0.1:5678");
      for(int i = 0; i < 1000; i++)
          log.setAppName(QString::number(i));
      for(int i = 0; i < 1000; i++)
          QCoreApplication::processEvents();
    }

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


    void test_logFilter()
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

                          Item{

                          property var msgFiltered: []
                          property var logTypeFiltered: []

                            Connections{
                            target: Ossia.Logger
                            onFilteredLog : {
                               msgFiltered.push(msg);
                               logTypeFiltered.push(type);

                               console.log('filtered: ' + msg + ' '+ type+ ' '+fileName + ' '+line );
                             }

                            }
                          Component.onCompleted:{
                          console.log("test")

                          Ossia.Logger.logFilter= ["cameraBin","touchpoint"];
                          Ossia.Logger.loggerHost = 'ws://127.0.0.1:1337';
                          Ossia.Logger.logQtMessages = true;

                          Ossia.Logger.startHeartbeat({});


                          console.log("avant")
                          console.log('touchpoint')

                          console.log("touchy")
                          console.log("cameraBin");
                          console.log("autre")
                          }
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);
        QList<QVariant> listMsgFiltered = QQmlProperty::read(item, "msgFiltered").toList();
        QVERIFY(listMsgFiltered.length() ==2);
        QVERIFY(listMsgFiltered[1].toString() == "cameraBin");
        QVERIFY(listMsgFiltered[0].toString() =="touchpoint");

        QList<QVariant> listLogTypeFiltered = QQmlProperty::read(item, "logTypeFiltered").toList();
        QVERIFY(listLogTypeFiltered.length() ==2);
        QVERIFY(listLogTypeFiltered[1].toInt() == 0);

        QTimer::singleShot(2000, [&] {
          app.exit();
        });
        app.exec();
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
                            // Address attributes:
                            valueType: Ossia.Type.Float
                            node: "/foo/bar/float"
                            min: -5
                            max: 26
                            value: 12
                            access: Ossia.Access.Get
                            bounding: Ossia.Bounding.Wrap
                            filterRepetitions: Ossia.Repetitions.Filtered
                            unit: "distance.cm"

                            // Node attributes:
                            description: "une bien jolie node!"
                            tags: ["foo", "bar"]
                            extendedType: "filepath"
                            priority: 36
                            refreshRate: 50
                            stepSize: 20
                            defaultValue: 40.34
                            critical: true
                            hidden: true
                            muted: true
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
          auto addr = node->get_parameter();
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

    void test_model()
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
                          Repeater {
                            model: Ossia.Instances { id: rp; node: "foo"; count: 10 }
                            Item {
                              Ossia.Node { node: "foo." + index; id: n }
                              Ossia.Property on x { parentNode: n }
                              Ossia.Property on y { parentNode: n }
                            }
                          }
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = (QQuickItem*) component.create();
        QVERIFY(item);

        print_device();

        for(int i = 0; i < 10;i ++)
        {
          ossia::net::node_base* node = ossia::net::find_node(
                                          dev.device().get_root_node(),
                                          "/foo." + std::to_string(i) + "/x");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::FLOAT);
        }

        item->setParent(&app);
        auto model = item->findChildren<ossia::qt::qml_model_property*>();
        qDebug() << "Model: " << model.size();
        for(auto m : model)
          qDebug() << m << m->QObject::parent();
        auto props = item->findChildren<ossia::qt::qml_property*>();


        qDebug() << "Props: " << props.size();
        for(auto p : props)
          qDebug() << p << p->parent() << p->parentItem();
        qDebug() << "Child items: " << item->childItems();

        cleanup(item);
      }
    }

    void test_model_sub_child()
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
                          Ossia.Node { id: tata; node: "tata"}
                          Repeater {
                            model: Ossia.Instances { id: rp; node: "foo"; parentNode: tata; count: 10 }
                            Item {
                              Ossia.Node { node: "foo." + index; id: n }
                              Ossia.Property on x { parentNode: n }
                              Ossia.Property on y { parentNode: n }
                            }
                          }
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = (QQuickItem*) component.create();
        QVERIFY(item);

        for(int k = 0; k < 10; k++)
        {
          dev.recreate(item);

          ossia::net::node_base* tata = ossia::net::find_node(
                                          dev.device().get_root_node(),
                                          "/tata");
          QVERIFY(tata->children().size() == 10);
          for(int i = 0; i < 10;i ++)
          {
            ossia::net::node_base* node = ossia::net::find_node(
                                            dev.device().get_root_node(),
                                            "/tata/foo." + std::to_string(i) + "/x");
            QVERIFY(node);
            QVERIFY(node->get_parameter());
            QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::FLOAT);
          }
        }

        cleanup(item);
      }
    }


    void test_model_sub_child_noparent()
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
                          Repeater {
                            model: Ossia.Instances { id: rp; node: "foo";  count: 10 }
                            Item {
                              Ossia.Node { node: "foo." + index; id: n }
                              Ossia.Property on x { parentNode: n }
                              Ossia.Property on y { parentNode: n }
                            }
                          }
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = (QQuickItem*) component.create();
        QVERIFY(item);

        for(int k = 0; k < 10; k++)
        {
          dev.recreate(item);
          QVERIFY(dev.device().get_root_node().children().size() == 10);
          for(int i = 0; i < 10;i ++)
          {
            ossia::net::node_base* node = ossia::net::find_node(
                                            dev.device().get_root_node(),
                                            "/foo." + std::to_string(i) + "/x");
            QVERIFY(node);
            QVERIFY(node->get_parameter());
            QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::FLOAT);
          }
        }

        cleanup(item);
      }
    }

    void test_model_sub_child_device_parent()
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
                          Repeater {
                            model: Ossia.Instances { id: rp; node: "foo"; parentNode: Ossia.SingleDevice;  count: 10 }
                            Item {
                              Ossia.Node { node: "foo." + index; id: n }
                              Ossia.Property on x { parentNode: n }
                              Ossia.Property on y { parentNode: n }
                            }
                          }
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = (QQuickItem*) component.create();
        QVERIFY(item);

        for(int k = 0; k < 10; k++)
        {
          dev.recreate(item);
          QVERIFY(dev.device().get_root_node().children().size() == 10);
          for(int i = 0; i < 10;i ++)
          {
            ossia::net::node_base* node = ossia::net::find_node(
                                            dev.device().get_root_node(),
                                            "/foo." + std::to_string(i) + "/x");
            QVERIFY(node);
            QVERIFY(node->get_parameter());
            QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::FLOAT);
          }
        }

        cleanup(item);
      }
    }

    void test_sub_item()
    {
        int argc{}; char** argv{};
        QCoreApplication app(argc, argv);
        ossia::context context;
        QQmlEngine engine;

        engine.addImportPath(QDir().absolutePath() + "/testdata/qml");
        engine.addPluginPath(QDir().absolutePath() + "/testdata/qml");
        auto& dev = ossia::qt::qml_singleton_device::instance();

        print_device();
        QQmlComponent component(&engine);
        component.setData(R"_(
                          import Ossia 1.0 as Ossia
                          import QtQuick 2.5

                          Item {
                            id: tutu
                            Ossia.Node { node: "foo." + 0 ; id: n }
                            Ossia.Property on x { parentNode: n }
                            Ossia.Property on y { parentNode: n }
                            Item {
                              id: tata
                              parent: tutu

                              Ossia.Node {
                                  id: sub
                                  node: "tata"
                                  parentNode: n
                                  Component.onCompleted: console.log("fuuu")
                              }
                              Ossia.Property on scale { parentNode: sub }
                            }
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = (QQuickItem*) component.create();
        QVERIFY(item);

        dev.recreate(item);
        app.processEvents();

        print_device();
        auto child_scale = ossia::net::find_node(dev.device().get_root_node(), "/foo.0/tata/scale");
        qDebug() << child_scale;

        dumpTree(item);

        QVERIFY(child_scale);
        cleanup(item);
    }


    /* What we want but not possible due to
     * https://bugreports.qt.io/browse/QTBUG-60121
    void test_model_simpler()
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
                          Repeater {
                            model: Ossia.Instances { node: "foo"; count: 10 }
                            Item {
                              Ossia.Property on x { }
                              Ossia.Property on y { }
                            }
                          }
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);

        fmt::MemoryWriter c; ossia::net::debug_recursively(c, dev.device().get_root_node());
        std::cerr << c.str();

        for(int i = 0; i < 10;i ++)
        {
          ossia::net::node_base* node = ossia::net::find_node(
                                          dev.device().get_root_node(),
                                          "/foo." + std::to_string(i) + "/x");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::FLOAT);
        }

        cleanup(item);
      }
    }
    */




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

                          Component.onCompleted: Ossia.SingleDevice.recreate(this)
                          }
                          )_", QUrl{});

        qDebug() << component.errorString();
        QVERIFY(component.errors().empty());
        auto item = component.create();
        QVERIFY(item);
        dev.recreate(item);

        print_device();

        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/x");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::FLOAT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/y");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::FLOAT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theInt");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::INT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theFloat");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::FLOAT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theDouble");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::FLOAT);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theString");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::STRING);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theBool");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::BOOL);
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theColor");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::VEC4F);
          QVERIFY(node->get_parameter()->get_unit() == ossia::argb_u{});
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/thePoint");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::VEC2F);
          QVERIFY(node->get_parameter()->get_unit() == ossia::cartesian_2d_u{});
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theVector2D");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::VEC2F);
          QVERIFY(node->get_parameter()->get_unit() == ossia::cartesian_2d_u{});
        }
        {
          ossia::net::node_base* node = ossia::net::find_node(dev.device().get_root_node(), "/theVector3D");
          QVERIFY(node);
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::VEC3F);
          QVERIFY(node->get_parameter()->get_unit() == ossia::cartesian_3d_u{});
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
          QVERIFY(node->get_parameter());
          QVERIFY(node->get_parameter()->get_value_type() == ossia::val_type::IMPULSE);
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

        print_device();

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

        dev.recreate(item);
        QVERIFY(item);

        print_device();

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

        dev.recreate(item);
        QVERIFY(item);

        print_device();

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

        dev.recreate(item);
        dumpTree(item);
        print_device();
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

