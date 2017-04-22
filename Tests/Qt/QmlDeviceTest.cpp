#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/context.hpp>
#include <ossia-qt/device/qml_device.hpp>
#include <QQmlEngine>
#include <QQmlComponent>

class QmlDeviceTest : public QObject
{
  Q_OBJECT

private slots:
  void test_device()
  {
    int argc{}; char** argv{};
    QCoreApplication app(argc, argv);
    QQmlEngine engine;
    ossia::context context;

    QQmlComponent component(&engine, "testdata/qml/TestDevice.qml");
    qDebug() << component.errorString();

    auto item = component.create();
    QVERIFY(item);

    auto& dev = ossia::qt::qml_singleton_device::instance();
    auto node = ossia::net::find_node(dev.device().get_root_node(), "/bar/x");
    QVERIFY(node);

  }
};


QTEST_APPLESS_MAIN(QmlDeviceTest)

#include "QmlDeviceTest.moc"

