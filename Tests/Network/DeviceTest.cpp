#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/network/http/http.hpp>
#include <iostream>

using namespace ossia;

class DeviceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
    {
      ossia::net::generic_device local_device{
        std::make_unique<ossia::net::local_protocol>(), "test" };
    }
    {
      ossia::net::generic_device osc_device{
        std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc" };
    }
    {
      ossia::net::generic_device minuit_device{
        std::make_unique<ossia::net::minuit_protocol>("i-score", "127.0.0.1", 13579, 13580), "test_minuit" };
    }
    }

    void test_http()
    {
      int argc{}; char** argv{};
      QCoreApplication app(argc, argv);

      ossia::context context;

      QFile f("testdata/http/http_example.qml");
      f.open(QFile::ReadOnly);

      ossia::net::http_device http_device{
        std::make_unique<ossia::net::http_protocol>(
              f.readAll()),
            "test" };

      // We have to wait a bit for the event loop to run.
      QTimer t;
      connect(&t, &QTimer::timeout, [&] () {
        auto node = ossia::net::find_node(http_device, "/tata/tutu");
        if(node)
        {
          node->getAddress()->pushValue(ossia::Impulse{});
        }
      });
      t.setInterval(1000);
      t.setSingleShot(true);
      t.start();

      QTimer::singleShot(3000, [&] () { app.exit(); });

      app.exec();
    }
};


QTEST_APPLESS_MAIN(DeviceTest)

#include "DeviceTest.moc"

