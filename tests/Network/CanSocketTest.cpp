// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/sockets/can_socket.hpp>

#if defined(__linux__)
#include <boost/asio/io_context.hpp>

#include <net/if.h>
#include <sys/ioctl.h>

#include <chrono>
#include <cstring>
#include <thread>
#include <vector>

namespace
{
// The tests need an actual CAN netdev; "vcan0" is the conventional name of the
// virtual one (modprobe vcan; ip link add dev vcan0 type vcan; ip link set up
// vcan0). When it is not there we skip rather than fail: CI machines generally
// do not have the vcan module loaded, and neither does a plain user session.
constexpr const char* can_iface = "vcan0";

struct can_iface_info
{
  bool present{};
  int mtu{};
};

can_iface_info probe_iface()
{
  can_iface_info info;
  int fd = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if(fd < 0)
    return info;

  ifreq ifr{};
  std::strncpy(ifr.ifr_name, can_iface, sizeof(ifr.ifr_name) - 1);
  if(::ioctl(fd, SIOCGIFINDEX, &ifr) == 0)
  {
    info.present = true;
    ifreq mtu_req{};
    std::strncpy(mtu_req.ifr_name, can_iface, sizeof(mtu_req.ifr_name) - 1);
    if(::ioctl(fd, SIOCGIFMTU, &mtu_req) == 0)
      info.mtu = mtu_req.ifr_mtu;
  }
  ::close(fd);
  return info;
}

//! Run the io_context until `pred` holds or we give up.
template <typename F>
bool spin(boost::asio::io_context& ctx, F pred, int ms = 1000)
{
  for(int i = 0; i < ms; i++)
  {
    ctx.poll();
    ctx.restart();
    if(pred())
      return true;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return pred();
}

ossia::net::can_configuration base_conf()
{
  ossia::net::can_configuration c;
  c.interface_name = can_iface;
  return c;
}

struct collector
{
  std::vector<ossia::net::can_message> messages;
  void operator()(const ossia::net::can_message& m) { messages.push_back(m); }
};

struct flag
{
  bool value{};
  void operator()() { value = true; }
};
}

TEST_CASE("test_can_roundtrip", "test_can")
{
  const auto info = probe_iface();
  if(!info.present)
    SKIP("no " << can_iface << " interface available");

  boost::asio::io_context ctx;

  auto rx_conf = base_conf();
  ossia::net::can_socket rx{rx_conf, ctx};
  rx.open();

  ossia::net::can_socket tx{base_conf(), ctx};
  tx.open();

  collector c;
  rx.receive([&c](const ossia::net::can_message& m) { c(m); });

  ossia::net::can_message msg;
  msg.id = 0x123;
  msg.size = 8;
  for(int i = 0; i < 8; i++)
    msg.data[i] = 0xA0 + i;

  REQUIRE(!tx.write(msg));

  spin(ctx, [&] { return !c.messages.empty(); });

  REQUIRE(c.messages.size() == 1);
  const auto& r = c.messages[0];
  REQUIRE(r.id == 0x123);
  REQUIRE(r.size == 8);
  REQUIRE(!r.extended);
  REQUIRE(!r.remote);
  REQUIRE(!r.fd);
  REQUIRE(!r.error);
  for(int i = 0; i < 8; i++)
    REQUIRE(r.data[i] == 0xA0 + i);
}

TEST_CASE("test_can_extended_id", "test_can")
{
  if(!probe_iface().present)
    SKIP("no " << can_iface << " interface available");

  boost::asio::io_context ctx;
  ossia::net::can_socket rx{base_conf(), ctx};
  rx.open();
  ossia::net::can_socket tx{base_conf(), ctx};
  tx.open();

  collector c;
  rx.receive([&c](const ossia::net::can_message& m) { c(m); });

  ossia::net::can_message msg;
  msg.id = 0x18DAF110; // a 29-bit id, as used by e.g. ISO-TP diagnostics
  msg.extended = true;
  msg.size = 3;
  msg.data[0] = 1;
  msg.data[1] = 2;
  msg.data[2] = 3;
  REQUIRE(!tx.write(msg));

  spin(ctx, [&] { return !c.messages.empty(); });

  REQUIRE(c.messages.size() == 1);
  REQUIRE(c.messages[0].extended);
  REQUIRE(c.messages[0].id == 0x18DAF110);
  REQUIRE(c.messages[0].size == 3);
  REQUIRE(c.messages[0].data[2] == 3);
}

TEST_CASE("test_can_rtr", "test_can")
{
  if(!probe_iface().present)
    SKIP("no " << can_iface << " interface available");

  boost::asio::io_context ctx;
  ossia::net::can_socket rx{base_conf(), ctx};
  rx.open();
  ossia::net::can_socket tx{base_conf(), ctx};
  tx.open();

  collector c;
  rx.receive([&c](const ossia::net::can_message& m) { c(m); });

  ossia::net::can_message msg;
  msg.id = 0x321;
  msg.remote = true;
  msg.size = 4; // for an RTR frame this is the *requested* length
  REQUIRE(!tx.write(msg));

  spin(ctx, [&] { return !c.messages.empty(); });

  REQUIRE(c.messages.size() == 1);
  REQUIRE(c.messages[0].remote);
  REQUIRE(c.messages[0].id == 0x321);
  REQUIRE(c.messages[0].size == 4);
}

TEST_CASE("test_can_fd", "test_can")
{
  const auto info = probe_iface();
  if(!info.present)
    SKIP("no " << can_iface << " interface available");
  if(info.mtu < CANFD_MTU)
    SKIP("" << can_iface << " is not CAN FD capable (mtu " << info.mtu << ")");

  boost::asio::io_context ctx;

  auto conf = base_conf();
  conf.fd = true;
  ossia::net::can_socket rx{conf, ctx};
  rx.open();
  ossia::net::can_socket tx{conf, ctx};
  tx.open();

  collector c;
  rx.receive([&c](const ossia::net::can_message& m) { c(m); });

  ossia::net::can_message fd;
  fd.id = 0x456;
  fd.fd = true;
  fd.bitrate_switch = true;
  fd.size = 64;
  for(int i = 0; i < 64; i++)
    fd.data[i] = i;
  REQUIRE(!tx.write(fd));

  // Classic and FD frames share the socket once CAN_RAW_FD_FRAMES is on: check
  // that we still tell them apart on a single receive loop.
  ossia::net::can_message classic;
  classic.id = 0x457;
  classic.size = 2;
  classic.data[0] = 0xFF;
  classic.data[1] = 0xEE;
  REQUIRE(!tx.write(classic));

  spin(ctx, [&] { return c.messages.size() >= 2; });

  REQUIRE(c.messages.size() == 2);

  REQUIRE(c.messages[0].fd);
  REQUIRE(c.messages[0].bitrate_switch);
  REQUIRE(c.messages[0].id == 0x456);
  REQUIRE(c.messages[0].size == 64);
  for(int i = 0; i < 64; i++)
    REQUIRE(c.messages[0].data[i] == i);

  REQUIRE(!c.messages[1].fd);
  REQUIRE(c.messages[1].id == 0x457);
  REQUIRE(c.messages[1].size == 2);
  REQUIRE(c.messages[1].data[0] == 0xFF);
}

TEST_CASE("test_can_filters", "test_can")
{
  if(!probe_iface().present)
    SKIP("no " << can_iface << " interface available");

  boost::asio::io_context ctx;

  auto rx_conf = base_conf();
  rx_conf.filters.push_back({.id = 0x123, .mask = CAN_SFF_MASK});
  ossia::net::can_socket rx{rx_conf, ctx};
  rx.open();

  // A second socket on the same interface, with no filter: SocketCAN gives each
  // socket its own view of the bus, so this one must see everything.
  ossia::net::can_socket rx_all{base_conf(), ctx};
  rx_all.open();

  ossia::net::can_socket tx{base_conf(), ctx};
  tx.open();

  collector filtered, all;
  rx.receive([&](const ossia::net::can_message& m) { filtered(m); });
  rx_all.receive([&](const ossia::net::can_message& m) { all(m); });

  for(uint32_t id : {0x122u, 0x123u, 0x124u})
  {
    ossia::net::can_message msg;
    msg.id = id;
    msg.size = 1;
    msg.data[0] = uint8_t(id);
    REQUIRE(!tx.write(msg));
  }

  spin(ctx, [&] { return all.messages.size() >= 3; });

  REQUIRE(all.messages.size() == 3);
  REQUIRE(filtered.messages.size() == 1);
  REQUIRE(filtered.messages[0].id == 0x123);
}

TEST_CASE("test_can_receive_own_messages", "test_can")
{
  if(!probe_iface().present)
    SKIP("no " << can_iface << " interface available");

  boost::asio::io_context ctx;

  // Default: a socket does not see the frames it sent itself...
  {
    ossia::net::can_socket sock{base_conf(), ctx};
    sock.open();
    collector c;
    sock.receive([&](const ossia::net::can_message& m) { c(m); });

    ossia::net::can_message msg;
    msg.id = 0x001;
    msg.size = 1;
    REQUIRE(!sock.write(msg));

    spin(ctx, [&] { return !c.messages.empty(); }, 200);
    REQUIRE(c.messages.empty());
  }

  // ... unless it asks for it.
  {
    auto conf = base_conf();
    conf.receive_own_messages = true;
    ossia::net::can_socket sock{conf, ctx};
    sock.open();
    collector c;
    sock.receive([&](const ossia::net::can_message& m) { c(m); });

    ossia::net::can_message msg;
    msg.id = 0x002;
    msg.size = 1;
    REQUIRE(!sock.write(msg));

    spin(ctx, [&] { return !c.messages.empty(); });
    REQUIRE(c.messages.size() == 1);
    REQUIRE(c.messages[0].id == 0x002);
  }
}

TEST_CASE("test_can_no_loopback", "test_can")
{
  if(!probe_iface().present)
    SKIP("no " << can_iface << " interface available");

  boost::asio::io_context ctx;

  ossia::net::can_socket rx{base_conf(), ctx};
  rx.open();

  // Disabling loopback on the *sender* hides its frames from every other local
  // socket -- it is a property of the sending socket, not of the receiver.
  auto tx_conf = base_conf();
  tx_conf.loopback = false;
  ossia::net::can_socket tx{tx_conf, ctx};
  tx.open();

  collector c;
  rx.receive([&](const ossia::net::can_message& m) { c(m); });

  ossia::net::can_message msg;
  msg.id = 0x111;
  msg.size = 1;
  REQUIRE(!tx.write(msg));

  spin(ctx, [&] { return !c.messages.empty(); }, 200);
  REQUIRE(c.messages.empty());
}

TEST_CASE("test_can_bad_interface", "test_can")
{
  boost::asio::io_context ctx;
  ossia::net::can_configuration conf;
  conf.interface_name = "ossia-no-such-can-if";
  ossia::net::can_socket sock{conf, ctx};

  REQUIRE_THROWS_AS(sock.open(), std::system_error);
  REQUIRE(!sock.connected());
}

TEST_CASE("test_can_write_before_open", "test_can")
{
  boost::asio::io_context ctx;
  ossia::net::can_socket sock{base_conf(), ctx};

  ossia::net::can_message msg;
  msg.id = 0x1;
  // The write path reports rather than throws or drops silently
  REQUIRE(sock.write(msg) == boost::asio::error::not_connected);
}

TEST_CASE("test_can_write_error_is_reported", "test_can")
{
  if(!probe_iface().present)
    SKIP("no " << can_iface << " interface available");

  boost::asio::io_context ctx;
  ossia::net::can_socket sock{base_conf(), ctx}; // conf.fd == false
  sock.open();

  std::vector<boost::system::error_code> errors;
  auto record = [&](boost::system::error_code ec) { errors.push_back(ec); };
  sock.on_write_error.connect(&record);

  // The kernel only accepts a CANFD_MTU-sized write when CAN_RAW_FD_FRAMES was
  // enabled on the socket, so this is rejected with EINVAL. It is the same path
  // ENOBUFS takes when a real interface's txqueue is full: reported, not
  // dropped silently.
  ossia::net::can_message msg;
  msg.id = 0x100;
  msg.fd = true;
  msg.size = 12;
  auto ec = sock.write(msg);

  REQUIRE(ec);
  REQUIRE(errors.size() == 1);
  REQUIRE(errors[0] == ec);
}

TEST_CASE("test_can_error_frames", "test_can")
{
  if(!probe_iface().present)
    SKIP("no " << can_iface << " interface available");

  boost::asio::io_context ctx;

  auto conf = base_conf();
  conf.error_frames = true;
  ossia::net::can_socket rx{conf, ctx};
  rx.open();

  ossia::net::can_socket tx{base_conf(), ctx};
  tx.open();

  collector c;
  rx.receive([&](const ossia::net::can_message& m) { c(m); });

  // We cannot make a vcan controller go bus-off, so this only checks that
  // enabling the error filter does not disturb normal reception (the kernel
  // keeps CAN_RAW_ERR_FILTER and CAN_RAW_FILTER independent).
  ossia::net::can_message msg;
  msg.id = 0x7FF;
  msg.size = 1;
  msg.data[0] = 0x42;
  REQUIRE(!tx.write(msg));

  spin(ctx, [&] { return !c.messages.empty(); });
  REQUIRE(c.messages.size() == 1);
  REQUIRE(!c.messages[0].error);
  REQUIRE(c.messages[0].id == 0x7FF);
}

TEST_CASE("test_can_close", "test_can")
{
  if(!probe_iface().present)
    SKIP("no " << can_iface << " interface available");

  boost::asio::io_context ctx;
  ossia::net::can_socket sock{base_conf(), ctx};
  sock.open();
  REQUIRE(sock.connected());

  flag closed;
  sock.on_close.connect(&closed);

  collector c;
  sock.receive([&](const ossia::net::can_message& m) { c(m); });

  sock.close();
  spin(ctx, [&] { return closed.value; }, 200);

  REQUIRE(closed.value);
  REQUIRE(!sock.connected());
}
#endif
