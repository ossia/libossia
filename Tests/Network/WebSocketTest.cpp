#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/context.hpp>
#include <ossia-qt/websocket-generic-client/ws_generic_client.hpp>
#include <iostream>

using namespace ossia;
class WebSocketTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:


    void test_websockets()
    {
        int argc{}; char** argv{};
        QCoreApplication app(argc, argv);

        ossia::context context;
        QFile f("testdata/websocket/ws_example.qml");
        f.open(QFile::ReadOnly);

        ossia::net::ws_generic_client_device ws_device{
          std::make_unique<ossia::net::ws_generic_client_protocol>(
                "ws://echo.websocket.org",
                f.readAll()),
              "test" };

        // We have to wait a bit for the event loop to run.
        QTimer t;
        connect(&t, &QTimer::timeout, [&] () {
          auto node = ossia::net::find_node(ws_device, "/tata/tutu");
          if(node)
          {
            node->get_address()->push_value(32.325);
          }
        });
        t.setInterval(1000);
        t.setSingleShot(true);
        t.start();

        QTimer::singleShot(3000, [&] () { app.exit(); });

        app.exec();
    }
};


QTEST_APPLESS_MAIN(WebSocketTest)

#include "WebSocketTest.moc"

