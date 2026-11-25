// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>

#include "include_catch.hpp"

#include <chrono>
#include <string>
#include <thread>

#if __has_include(<ifaddrs.h>)
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#endif

namespace
{
//! An IPv4 address of this machine other than the loopback, or empty: from
//! the host name, which Windows resolves to its addresses, else from the
//! interfaces
std::string other_ipv4()
{
  boost::asio::io_context ctx;
  boost::system::error_code ec;
  boost::asio::ip::tcp::resolver r{ctx};
  for(auto& e : r.resolve(boost::asio::ip::tcp::v4(), boost::asio::ip::host_name(), "", ec))
  {
    const auto a = e.endpoint().address();
    if(a.is_v4() && !a.is_loopback())
      return a.to_string();
  }
  std::string res;
#if __has_include(<ifaddrs.h>)
  ifaddrs* ifs{};
  if(getifaddrs(&ifs) != 0)
    return res;
  for(auto i = ifs; i && res.empty(); i = i->ifa_next)
  {
    if(!i->ifa_addr || i->ifa_addr->sa_family != AF_INET)
      continue;
    auto a = reinterpret_cast<sockaddr_in*>(i->ifa_addr)->sin_addr;
    if((ntohl(a.s_addr) >> 24) == 127)
      continue;
    char buf[INET_ADDRSTRLEN]{};
    inet_ntop(AF_INET, &a, buf, sizeof(buf));
    res = buf;
  }
  freeifaddrs(ifs);
#endif
  return res;
}

bool reaches(const std::string& ip, int port)
{
  try
  {
    boost::asio::io_context ctx;
    boost::asio::ip::tcp::socket sock{ctx};
    sock.connect({boost::asio::ip::make_address(ip), (unsigned short)port});
    return true;
  }
  catch(...)
  {
    return false;
  }
}

std::string host_info(int port)
{
  boost::asio::io_context ctx;
  boost::asio::ip::tcp::socket sock{ctx};
  sock.connect({boost::asio::ip::make_address("127.0.0.1"), (unsigned short)port});
  const std::string req
      = "GET /?HOST_INFO HTTP/1.1\r\nHost: 127.0.0.1\r\n"
        "Connection: close\r\n\r\n";
  boost::asio::write(sock, boost::asio::buffer(req));
  std::string reply;
  boost::system::error_code ec;
  char buf[4096];
  for(std::size_t n; (n = sock.read_some(boost::asio::buffer(buf), ec)) > 0 && !ec;)
    reply.append(buf, n);
  return reply;
}

//! An asio server and the thread that runs it, stopped before the device goes
struct served
{
  using proto_t = ossia::oscquery_asio::oscquery_server_protocol;
  std::shared_ptr<ossia::net::network_context> ctx
      = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device dev;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work{
      ctx->context.get_executor()};
  std::thread thread{[c = ctx] { c->context.run(); }};

  served(uint16_t osc_port, uint16_t ws_port, std::string bind)
      : dev{bind.empty()
                ? std::make_unique<proto_t>(ctx, osc_port, ws_port)
                : std::make_unique<proto_t>(ctx, osc_port, ws_port, false, bind),
            "listen_address"}
  {
  }
  ~served()
  {
    ctx->context.stop();
    thread.join();
  }
};
}

TEST_CASE("test_oscquery_asio_every_interface_by_default", "[oscquery]")
{
  served s{1824, 5824, ""};
  auto& proto = static_cast<served::proto_t&>(s.dev.get_protocol());
  CHECK(proto.bound_to_every_interface());
  CHECK(reaches("127.0.0.1", 5824));
}

TEST_CASE("test_oscquery_asio_every_interface_reached", "[oscquery]")
{
  const auto other = other_ipv4();
  if(other.empty())
    SKIP("no IPv4 address other than the loopback");
  served s{1826, 5826, ""};
  CHECK(reaches(other, 5826));
}

TEST_CASE("test_oscquery_asio_listens_on_one_address", "[oscquery]")
{
  served s{1825, 5825, "127.0.0.1"};
  auto& proto = static_cast<served::proto_t&>(s.dev.get_protocol());
  CHECK(!proto.bound_to_every_interface());
  CHECK(proto.get_bind() == "127.0.0.1");
  CHECK(reaches("127.0.0.1", 5825));
  CHECK(host_info(5825).find(R"("WS_IP":"127.0.0.1")") != std::string::npos);
}

TEST_CASE("test_oscquery_asio_one_address_only", "[oscquery]")
{
  const auto other = other_ipv4();
  if(other.empty())
    SKIP("no IPv4 address other than the loopback");
  served s{1827, 5827, "127.0.0.1"};
  CHECK(!reaches(other, 5827));
}
