// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/context.hpp>
#include <ossia/network/sockets/can_socket.hpp>

// Same guard as CanSocketTest: CMake only builds this on Linux, but the file
// says so itself so that it cannot be broken by a change to the test lists.
#if defined(__linux__)
#include <ossia-qt/qml_protocols.hpp>

#include <QCoreApplication>
#include <QJSEngine>

#include <net/if.h>
#include <sys/ioctl.h>

#include <unistd.h>

#include <cstring>

namespace
{
constexpr const char* can_iface = "vcan0";

//! The tests need a real CAN netdev; skip when the machine has no vcan0.
bool has_vcan()
{
  int fd = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if(fd < 0)
    return false;
  ifreq ifr{};
  std::strncpy(ifr.ifr_name, can_iface, sizeof(ifr.ifr_name) - 1);
  const bool ok = ::ioctl(fd, SIOCGIFINDEX, &ifr) == 0;
  ::close(fd);
  return ok;
}

//! A plain SocketCAN socket, to play the other node on the bus.
struct raw_can
{
  int fd{-1};

  raw_can()
  {
    fd = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(fd < 0)
      return;
    ifreq ifr{};
    std::strncpy(ifr.ifr_name, can_iface, sizeof(ifr.ifr_name) - 1);
    if(::ioctl(fd, SIOCGIFINDEX, &ifr) != 0)
    {
      ::close(fd);
      fd = -1;
      return;
    }
    sockaddr_can addr{};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if(::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
    {
      ::close(fd);
      fd = -1;
    }
  }

  ~raw_can()
  {
    if(fd >= 0)
      ::close(fd);
  }

  bool valid() const { return fd >= 0; }

  void send(uint32_t id, std::initializer_list<uint8_t> bytes) const
  {
    can_frame f{};
    f.can_id = id;
    f.can_dlc = uint8_t(bytes.size());
    int i = 0;
    for(auto b : bytes)
      f.data[i++] = b;
    [[maybe_unused]] auto res = ::write(fd, &f, sizeof(f));
  }

  //! Non-blocking read of one frame, if any is pending.
  bool recv(can_frame& out) const
  {
    return ::recv(fd, &out, sizeof(out), MSG_DONTWAIT) == CAN_MTU;
  }
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

TEST_CASE("test_qml_can_write", "test_qml_can")
{
  if(!has_vcan())
    SKIP("no " << can_iface << " interface available");

  raw_can peer;
  REQUIRE(peer.valid());

  qml_fixture f;

  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      return Protocols.can({ Transport: { Interface: "%1" } });
    })()
  )_")
                                   .arg(QString::fromUtf8(can_iface)));

  REQUIRE(!res.isError());
  REQUIRE(!res.isNull());
  REQUIRE(res.isQObject());

  auto write = f.engine.evaluate(
      QStringLiteral("(function(s) { s.write({ id: 0x123, bytes: [1, 2, 3, 4] }); })"));
  REQUIRE(!write.isError());
  REQUIRE(!write.call({res}).isError());

  can_frame frame{};
  bool got = false;
  f.spin([&] { return (got = peer.recv(frame)); });

  REQUIRE(got);
  REQUIRE(frame.can_id == 0x123);
  REQUIRE(frame.can_dlc == 4);
  REQUIRE(frame.data[0] == 1);
  REQUIRE(frame.data[3] == 4);
}

TEST_CASE("test_qml_can_write_extended", "test_qml_can")
{
  if(!has_vcan())
    SKIP("no " << can_iface << " interface available");

  raw_can peer;
  REQUIRE(peer.valid());

  qml_fixture f;

  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var s = Protocols.can({ Transport: { Interface: "%1" } });
      s.write({ id: 0x18DAF110, extended: true, bytes: [2, 16, 1] });
      return s;
    })()
  )_")
                                   .arg(QString::fromUtf8(can_iface)));
  REQUIRE(!res.isError());
  REQUIRE(!res.isNull());

  can_frame frame{};
  bool got = false;
  f.spin([&] { return (got = peer.recv(frame)); });

  REQUIRE(got);
  REQUIRE((frame.can_id & CAN_EFF_FLAG) != 0);
  REQUIRE((frame.can_id & CAN_EFF_MASK) == 0x18DAF110);
  REQUIRE(frame.can_dlc == 3);
}

TEST_CASE("test_qml_can_write_rtr", "test_qml_can")
{
  if(!has_vcan())
    SKIP("no " << can_iface << " interface available");

  raw_can peer;
  REQUIRE(peer.valid());

  qml_fixture f;

  // An RTR frame carries no data, only the length it is asking for
  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var s = Protocols.can({ Transport: { Interface: "%1" } });
      s.write({ id: 0x321, rtr: true, length: 8 });
      return s;
    })()
  )_")
                                   .arg(QString::fromUtf8(can_iface)));
  REQUIRE(!res.isError());
  REQUIRE(!res.isNull());

  can_frame frame{};
  bool got = false;
  f.spin([&] { return (got = peer.recv(frame)); });

  REQUIRE(got);
  REQUIRE((frame.can_id & CAN_RTR_FLAG) != 0);
  REQUIRE((frame.can_id & CAN_SFF_MASK) == 0x321);
  REQUIRE(frame.can_dlc == 8);
}

TEST_CASE("test_qml_can_read", "test_qml_can")
{
  if(!has_vcan())
    SKIP("no " << can_iface << " interface available");

  raw_can peer;
  REQUIRE(peer.valid());

  qml_fixture f;

  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var received = [];
      var s = Protocols.can({
        Transport: { Interface: "%1" },
        onMessage: function(frame) { received.push(frame); }
      });
      return { socket: s, received: received };
    })()
  )_")
                                   .arg(QString::fromUtf8(can_iface)));

  REQUIRE(!res.isError());
  REQUIRE(!res.property("socket").isNull());

  peer.send(0x2A1, {0xDE, 0xAD, 0xBE, 0xEF});

  auto received = res.property("received");
  f.spin([&] { return received.property("length").toInt() > 0; });

  REQUIRE(received.property("length").toInt() == 1);
  auto frame = received.property(0);
  REQUIRE(frame.property("id").toUInt() == 0x2A1);
  REQUIRE(frame.property("extended").toBool() == false);
  REQUIRE(frame.property("rtr").toBool() == false);
  REQUIRE(frame.property("fd").toBool() == false);
  REQUIRE(frame.property("bytes").property("length").toInt() == 4);
  REQUIRE(frame.property("bytes").property(0).toUInt() == 0xDE);
  REQUIRE(frame.property("bytes").property(3).toUInt() == 0xEF);
}

TEST_CASE("test_qml_can_filters", "test_qml_can")
{
  if(!has_vcan())
    SKIP("no " << can_iface << " interface available");

  raw_can peer;
  REQUIRE(peer.valid());

  qml_fixture f;

  // A filter without an explicit mask means "exactly this id"
  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var received = [];
      var s = Protocols.can({
        Transport: { Interface: "%1" },
        Filters: [ { id: 0x321 } ],
        onMessage: function(frame) { received.push(frame.id); }
      });
      return { socket: s, received: received };
    })()
  )_")
                                   .arg(QString::fromUtf8(can_iface)));

  REQUIRE(!res.isError());
  REQUIRE(!res.property("socket").isNull());

  peer.send(0x320, {1});
  peer.send(0x321, {2});
  peer.send(0x322, {3});

  auto received = res.property("received");
  f.spin([&] { return received.property("length").toInt() > 0; }, 500);

  REQUIRE(received.property("length").toInt() == 1);
  REQUIRE(received.property(0).toUInt() == 0x321);
}

TEST_CASE("test_qml_can_write_error", "test_qml_can")
{
  if(!has_vcan())
    SKIP("no " << can_iface << " interface available");

  qml_fixture f;

  // Writing an FD frame on a socket that did not ask for CAN FD is rejected by
  // the kernel (EINVAL): the write path must surface it through onError instead
  // of dropping the frame silently. This is the same route ENOBUFS takes when a
  // real interface's transmit queue overflows.
  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var errors = [];
      var s = Protocols.can({
        Transport: { Interface: "%1", FD: false },
        onError: function(e) { errors.push(e); }
      });
      s.write({ id: 0x111, fd: true, bytes: [1,2,3,4,5,6,7,8,9,10,11,12] });
      return { socket: s, errors: errors };
    })()
  )_")
                                   .arg(QString::fromUtf8(can_iface)));

  REQUIRE(!res.isError());
  REQUIRE(!res.property("socket").isNull());

  auto errors = res.property("errors");
  f.spin([&] { return errors.property("length").toInt() > 0; });

  REQUIRE(errors.property("length").toInt() > 0);
  REQUIRE(errors.property(0).toString().contains("write:"));
}

TEST_CASE("test_qml_can_interfaces", "test_qml_can")
{
  qml_fixture f;

  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var l = Protocols.canInterfaces();
      var names = [];
      for(var i = 0; i < l.length; i++) names.push(l[i].name);
      return names;
    })()
  )_"));

  REQUIRE(!res.isError());

  bool found = false;
  for(int i = 0; i < res.property("length").toInt(); i++)
    if(res.property(i).toString() == QString::fromUtf8(can_iface))
      found = true;

  if(!has_vcan())
    SKIP("no " << can_iface << " interface available");
  REQUIRE(found);
}

TEST_CASE("test_qml_can_bad_interface", "test_qml_can")
{
  qml_fixture f;

  // An interface that does not exist has to be reported, not silently swallowed
  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var err = null;
      var s = Protocols.can({
        Transport: { Interface: "nosuchcan0" },
        onError: function(e) { err = e; }
      });
      return { socket: s, error: err };
    })()
  )_"));

  REQUIRE(!res.isError());
  REQUIRE(res.property("socket").isNull());
  REQUIRE(!res.property("error").isNull());
  // The message must name the interface, so that the user knows what to fix
  REQUIRE(res.property("error").toString().contains("nosuchcan0"));
}

TEST_CASE("test_qml_can_missing_interface", "test_qml_can")
{
  qml_fixture f;

  auto res = f.engine.evaluate(QStringLiteral(R"_(
    (function() {
      var err = null;
      var s = Protocols.can({ onError: function(e) { err = e; } });
      return { socket: s, error: err };
    })()
  )_"));

  REQUIRE(!res.isError());
  REQUIRE(res.property("socket").isNull());
  REQUIRE(!res.property("error").isNull());
  REQUIRE(res.property("error").toString().contains("Interface"));
}
#endif
