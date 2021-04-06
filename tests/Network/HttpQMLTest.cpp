// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include "TestUtils.hpp"
#include "ProtocolTestUtils.hpp"


#if defined(OSSIA_QT)
#include <QTimer>
#include <QCoreApplication>
#endif
#if defined(OSSIA_PROTOCOL_HTTP)
#include <ossia-qt/http/http_protocol.hpp>
#endif

#if defined(OSSIA_PROTOCOL_HTTP)
TEST_CASE ("test_http", "test_http")
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

    // For the sake of coverage...
    const auto& const_dev = http_device;
    REQUIRE(&http_device.get_root_node() == &const_dev.get_root_node());

    // We have to wait a bit for the event loop to run.
    QTimer t;
    QObject::connect(&t, &QTimer::timeout, [&] () {
      auto node = ossia::net::find_node(http_device, "/tata/tutu");
      if(node)
      {
        node->get_parameter()->push_value(ossia::impulse{});
      }
    });
    t.setInterval(1000);
    t.setSingleShot(true);
    t.start();

    QTimer::singleShot(3000, [&] () { app.exit(); });

    app.exec();
  }
#endif

