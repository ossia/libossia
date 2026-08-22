// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/context.hpp>

#include <ossia-qt/qml_protocols.hpp>

#include <QCoreApplication>
#include <QJSEngine>
#include <QTimer>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <cstdlib>
#include <string>

namespace
{
// A pseudo-terminal is close enough to a serial port for boost::asio to drive
// it, which gives the test an actual device to open.
struct pty_pair
{
  int master{-1};
  std::string slave;

  pty_pair()
  {
    master = ::posix_openpt(O_RDWR | O_NOCTTY);
    if(master < 0)
      return;
    if(::grantpt(master) != 0 || ::unlockpt(master) != 0)
      return;
    if(const char* name = ::ptsname(master))
      slave = name;
  }

  ~pty_pair()
  {
    if(master >= 0)
      ::close(master);
  }

  bool valid() const { return master >= 0 && !slave.empty(); }
};

struct qml_fixture
{
  int argc{};
  char** argv{};
  QCoreApplication app{argc, argv};
  std::shared_ptr<ossia::net::network_context> ctx
      = std::make_shared<ossia::net::network_context>();
  QJSEngine engine;

  qml_fixture()
  {
    auto protocols = new ossia::qt::qml_protocols{ctx, nullptr};
    engine.globalObject().setProperty("Protocols", engine.newQObject(protocols));
  }

  //! Run both event loops until `pred` holds or the timeout expires.
  template <typename F>
  bool spin(F pred, int ms = 2000)
  {
    for(int i = 0; i < ms; i++)
    {
      ctx->context.poll();
      ctx->context.restart();
      QCoreApplication::processEvents();
      if(pred())
        return true;
      ::usleep(1000);
    }
    return pred();
  }
};
}

TEST_CASE("test_qml_serial_write", "test_qml_serial")
{
  pty_pair pty;
  REQUIRE(pty.valid());

  qml_fixture f;

  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      return Protocols.serial({ Transport: { Port: "%1", Baud: 115200 } });
    })()
  )_")
                                   .arg(QString::fromStdString(pty.slave)));

  REQUIRE(!res.isError());
  // Before the fix, Protocols.serial() was a stub returning nullptr, which
  // reaches JS as null.
  REQUIRE(!res.isNull());
  REQUIRE(res.isQObject());

  auto write
      = f.engine.evaluate(QStringLiteral("(function(s) { s.write(\"hello\"); })"));
  REQUIRE(!write.isError());
  auto call_res = write.call({res});
  REQUIRE(!call_res.isError());

  char buf[64] = {};
  ssize_t sz{};
  f.spin([&] {
    auto n = ::read(pty.master, buf, sizeof(buf) - 1);
    if(n > 0)
      sz = n;
    return sz > 0;
  });

  REQUIRE(sz == 5);
  REQUIRE(std::string(buf, sz) == "hello");
}

TEST_CASE("test_qml_serial_read", "test_qml_serial")
{
  pty_pair pty;
  REQUIRE(pty.valid());

  qml_fixture f;

  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var received = [];
      var s = Protocols.serial({
        Transport: { Port: "%1", Baud: 115200 },
        onBytes: function(b) { received.push(b.toString()); }
      });
      return { socket: s, received: received };
    })()
  )_")
                                   .arg(QString::fromStdString(pty.slave)));

  REQUIRE(!res.isError());
  REQUIRE(!res.property("socket").isNull());

  const char msg[] = "abcdef";
  REQUIRE(::write(pty.master, msg, sizeof(msg) - 1) == sizeof(msg) - 1);

  auto received = res.property("received");
  f.spin([&] { return received.property("length").toInt() > 0; });

  REQUIRE(received.property("length").toInt() > 0);
  REQUIRE(received.property(0).toString() == "abcdef");
}

TEST_CASE("test_qml_serial_bad_port", "test_qml_serial")
{
  qml_fixture f;

  // A port that cannot be opened has to be reported, not silently swallowed
  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var err = null;
      var s = Protocols.serial({
        Transport: { Port: "/dev/does-not-exist-ossia-test" },
        onError: function(e) { err = e; }
      });
      return { socket: s, error: err };
    })()
  )_"));

  REQUIRE(!res.isError());
  REQUIRE(res.property("socket").isNull());
  REQUIRE(!res.property("error").isNull());
}

TEST_CASE("test_qml_serial_missing_port", "test_qml_serial")
{
  qml_fixture f;

  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var err = null;
      var s = Protocols.serial({ onError: function(e) { err = e; } });
      return { socket: s, error: err };
    })()
  )_"));

  REQUIRE(!res.isError());
  REQUIRE(res.property("socket").isNull());
  REQUIRE(!res.property("error").isNull());
}
