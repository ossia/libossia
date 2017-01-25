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
  }
};


QTEST_APPLESS_MAIN(QmlDeviceTest)

#include "QmlDeviceTest.moc"

