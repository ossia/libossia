// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/sockets/framing.hpp>
#include <ossia/network/sockets/line_framing.hpp>
#include <ossia/network/sockets/no_framing.hpp>
#include <ossia/network/sockets/serial_socket.hpp>
#include <ossia/network/sockets/tcp_socket.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/unix_socket.hpp>

#include <boost/asio/io_context.hpp>

#include <memory>

/**
 * Destroying a socket that still has a read outstanding.
 *
 * Closing a descriptor does not make asio forget the operations pending on it:
 * they complete with operation_aborted on the next poll(), usually after the
 * owner is gone, and the handlers capture `this`.
 *
 * Each test arms a read, destroys the socket and pumps the context. Run them
 * under ASan - otherwise a use-after-free on a just-freed allocation goes
 * unnoticed and they pass either way.
 */

#if !defined(_WIN32)
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#endif

namespace
{
//! Nano::Signal wants a callable object, not a lambda.
struct flag
{
  bool value{};
  void operator()() { value = true; }
};

//! Run the queued completions, including the aborted ones.
void drain(boost::asio::io_context& ctx)
{
  for(int i = 0; i < 8; i++)
  {
    ctx.poll();
    ctx.restart();
  }
}

#if !defined(_WIN32)
//! A pseudo-terminal is close enough to a serial port for asio to drive it,
//! which gives the test a device to open without any hardware.
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

//! Open a serial_socket on a pty, arm a read, destroy it, then pump.
template <typename Framing>
void serial_read_outlives_socket()
{
  pty_pair pty;
  if(!pty.valid())
    SKIP("could not allocate a pseudo-terminal");

  boost::asio::io_context ctx;

  ossia::net::serial_configuration conf;
  conf.port = pty.slave;

  // Heap-allocated on purpose: this is how the protocols own their socket, and
  // it is what makes the failure a plain heap-use-after-free for ASan.
  auto sock = std::make_unique<ossia::net::serial_socket<Framing>>(conf, ctx);
  sock->connect();

  // connect() posts on_open() with a bare `this`; run it now so that the test
  // is about the pending *read* and nothing else.
  drain(ctx);

  bool got_message = false;
  sock->receive([&](const unsigned char*, std::size_t) { got_message = true; });

  // Nothing is ever written to the master side, so the read is still pending.
  sock.reset();

  // Before the fix: the aborted completion runs here, calls
  // stream_processor::validate_stream, and that calls on_fail() on the freed
  // serial_socket.
  drain(ctx);

  REQUIRE(!got_message);
}
#endif
}

#if !defined(_WIN32)
TEST_CASE("test_serial_socket_destroyed_with_pending_read", "test_socket_lifetime")
{
  serial_read_outlives_socket<ossia::net::no_framing>();
}

TEST_CASE("test_serial_slip_destroyed_with_pending_read", "test_socket_lifetime")
{
  serial_read_outlives_socket<ossia::net::slip_framing>();
}

TEST_CASE("test_serial_cobs_destroyed_with_pending_read", "test_socket_lifetime")
{
  serial_read_outlives_socket<ossia::net::cobs_framing>();
}

TEST_CASE("test_serial_size_prefix_destroyed_with_pending_read", "test_socket_lifetime")
{
  serial_read_outlives_socket<ossia::net::size_prefix_framing>();
}

TEST_CASE("test_serial_stx_etx_destroyed_with_pending_read", "test_socket_lifetime")
{
  serial_read_outlives_socket<ossia::net::stx_etx_framing>();
}

TEST_CASE("test_serial_fixed_length_destroyed_with_pending_read", "test_socket_lifetime")
{
  serial_read_outlives_socket<ossia::net::fixed_length_framing>();
}

TEST_CASE("test_serial_line_destroyed_with_pending_read", "test_socket_lifetime")
{
  serial_read_outlives_socket<ossia::net::line_framing>();
}
#endif

TEST_CASE("test_udp_receive_socket_destroyed_with_pending_read", "test_socket_lifetime")
{
  boost::asio::io_context ctx;

  ossia::net::inbound_socket_configuration conf;
  conf.bind = "127.0.0.1";
  conf.port = 0; // ephemeral: nothing else needs to find us

  auto sock = std::make_unique<ossia::net::udp_receive_socket>(conf, ctx);
  sock->open();

  bool got_message = false;
  sock->receive([&](const char*, std::size_t) { got_message = true; });

  sock.reset();
  drain(ctx);

  REQUIRE(!got_message);
}

TEST_CASE("test_tcp_client_destroyed_with_pending_read", "test_socket_lifetime")
{
  boost::asio::io_context ctx;

  boost::asio::ip::tcp::acceptor acceptor{
      ctx, boost::asio::ip::tcp::endpoint{
               boost::asio::ip::make_address("127.0.0.1"), 0}};
  const auto port = acceptor.local_endpoint().port();

  // The accepted socket has to stay alive, otherwise the client's read would
  // complete with eof instead of staying pending.
  boost::asio::ip::tcp::socket peer{ctx};
  acceptor.async_accept(peer, [](boost::system::error_code) {});

  ossia::net::outbound_socket_configuration conf;
  conf.host = "127.0.0.1";
  conf.port = port;

  using client = ossia::net::framed_client<ossia::net::tcp_client, ossia::net::slip_framing>;
  auto sock = std::make_unique<client>(conf, ctx);

  flag connected;
  sock->on_open.connect(&connected);
  sock->connect();

  drain(ctx);
  REQUIRE(connected.value);

  bool got_message = false;
  sock->receive([&](const unsigned char*, std::size_t) { got_message = true; });

  sock.reset();
  drain(ctx);

  REQUIRE(!got_message);
}

TEST_CASE("test_tcp_client_destroyed_with_pending_connect", "test_socket_lifetime")
{
  // Same defect, on the *connect* handler rather than the read one: async_connect
  // also captures `this`, and a socket destroyed before the connection settles
  // leaves that handler queued.
  boost::asio::io_context ctx;

  boost::asio::ip::tcp::acceptor acceptor{
      ctx, boost::asio::ip::tcp::endpoint{
               boost::asio::ip::make_address("127.0.0.1"), 0}};
  const auto port = acceptor.local_endpoint().port();

  ossia::net::outbound_socket_configuration conf;
  conf.host = "127.0.0.1";
  conf.port = port;

  auto sock = std::make_unique<ossia::net::tcp_client>(conf, ctx);
  sock->connect();

  // No drain(): the connect is still in flight.
  sock.reset();
  drain(ctx);
}

TEST_CASE("test_udp_receive_socket_destroyed_after_close", "test_socket_lifetime")
{
  // close() does not close anything itself, it *posts* a lambda that does. The
  // usual shutdown sequence is close() immediately followed by dropping the
  // owner, which leaves that lambda queued on a destroyed socket.
  boost::asio::io_context ctx;

  ossia::net::inbound_socket_configuration conf;
  conf.bind = "127.0.0.1";
  conf.port = 0;

  auto sock = std::make_unique<ossia::net::udp_receive_socket>(conf, ctx);
  sock->open();
  sock->close();
  sock.reset();

  drain(ctx);
}

TEST_CASE("test_tcp_client_destroyed_after_close", "test_socket_lifetime")
{
  boost::asio::io_context ctx;

  boost::asio::ip::tcp::acceptor acceptor{
      ctx, boost::asio::ip::tcp::endpoint{
               boost::asio::ip::make_address("127.0.0.1"), 0}};

  ossia::net::outbound_socket_configuration conf;
  conf.host = "127.0.0.1";
  conf.port = acceptor.local_endpoint().port();

  auto sock = std::make_unique<ossia::net::tcp_client>(conf, ctx);
  sock->connect();
  drain(ctx);

  sock->close();
  sock.reset();

  drain(ctx);
}

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
TEST_CASE("test_unix_stream_client_destroyed_with_pending_read", "test_socket_lifetime")
{
  boost::asio::io_context ctx;

  const std::string path = "/tmp/ossia-socket-lifetime-test.sock";
  ::unlink(path.c_str());

  boost::asio::local::stream_protocol::acceptor acceptor{
      ctx, boost::asio::local::stream_protocol::endpoint{path}};

  boost::asio::local::stream_protocol::socket peer{ctx};
  acceptor.async_accept(peer, [](boost::system::error_code) {});

  ossia::net::fd_configuration conf;
  conf.fd = path;

  using client = ossia::net::framed_socket<
      ossia::net::unix_stream_client, ossia::net::size_prefix_framing>;
  auto sock = std::make_unique<client>(conf, ctx);
  sock->connect();

  drain(ctx);

  bool got_message = false;
  sock->receive([&](const unsigned char*, std::size_t) { got_message = true; });

  sock.reset();
  drain(ctx);
  ::unlink(path.c_str());

  REQUIRE(!got_message);
}

TEST_CASE("test_unix_datagram_socket_destroyed_with_pending_read", "test_socket_lifetime")
{
  // This one is expected to have been safe all along: unix_datagram_socket
  // already cancels and drains in its destructor -- see the comment there, it
  // was added for a crash of exactly this family on macOS. The test is here so
  // that the property does not quietly regress.
  boost::asio::io_context ctx;

  const std::string path = "/tmp/ossia-socket-lifetime-test.dgram";
  ::unlink(path.c_str());

  ossia::net::fd_configuration conf;
  conf.fd = path;

  auto sock = std::make_unique<ossia::net::unix_datagram_socket>(conf, ctx);
  sock->open();

  bool got_message = false;
  sock->receive([&](const char*, std::size_t) { got_message = true; });

  sock.reset();
  drain(ctx);
  ::unlink(path.c_str());

  REQUIRE(!got_message);
}
#endif
