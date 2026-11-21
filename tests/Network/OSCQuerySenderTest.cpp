// How the OSCQuery server knows which client a datagram comes from
#include "OSCQueryHttpTestUtils.hpp"
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/udp.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>

#include <atomic>
#include <thread>

#if !defined(_WIN32)
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#endif

using namespace ossia;
using namespace ossia::net;

namespace
{
namespace ip = boost::asio::ip;

// An IPv4 address of this machine other than 127.0.0.1 that a socket can be
// bound to: 127.0.0.2 where the loopback takes it, else one of the host's
// addresses, else one of its interfaces' (macOS); empty if there is none
std::string other_ipv4()
{
  boost::asio::io_context ctx;
  auto bindable = [&](const ip::address_v4& a) {
    boost::system::error_code ec;
    ip::udp::socket s{ctx};
    s.open(ip::udp::v4(), ec);
    s.bind({a, 0}, ec);
    return !ec;
  };
  if(bindable(ip::make_address_v4("127.0.0.2")))
    return "127.0.0.2";
  boost::system::error_code ec;
  ip::udp::resolver r{ctx};
  for(auto& e : r.resolve(ip::udp::v4(), ip::host_name(), "", ec))
  {
    auto a = e.endpoint().address().to_v4();
    if(!a.is_loopback() && bindable(a))
      return a.to_string();
  }
#if !defined(_WIN32)
  ifaddrs* list{};
  if(getifaddrs(&list) == 0)
  {
    std::string found;
    for(auto* i = list; i && found.empty(); i = i->ifa_next)
    {
      if(!i->ifa_addr || i->ifa_addr->sa_family != AF_INET)
        continue;
      auto* in = reinterpret_cast<const sockaddr_in*>(i->ifa_addr);
      const auto a = ip::address_v4{ntohl(in->sin_addr.s_addr)};
      if(!a.is_loopback() && bindable(a))
        found = a.to_string();
    }
    freeifaddrs(list);
    if(!found.empty())
      return found;
  }
#endif
  return {};
}

// The datagrams that reach a port of this machine
struct udp_sink
{
  boost::asio::io_context ctx;
  ip::udp::socket socket{ctx, {ip::make_address_v4("127.0.0.1"), 0}};
  std::atomic_int count{0};
  std::atomic_bool done{false};
  std::thread thread{[this] {
    socket.non_blocking(true);
    char buf[2048];
    while(!done)
    {
      boost::system::error_code ec;
      if(socket.receive(boost::asio::buffer(buf), 0, ec) > 0 && !ec)
        count++;
      else
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
  }};
  ~udp_sink()
  {
    done = true;
    thread.join();
  }
  int port() const { return socket.local_endpoint().port(); }
};

// An OSC float sent to 127.0.0.1:port from from_ip:from_port
void send_osc_float(
    int port, const char* address, float f, const std::string& from_ip, int from_port)
{
  char buf[256];
  oscpack::OutboundPacketStream p(buf, sizeof(buf));
  p << oscpack::BeginMessage(address) << f << oscpack::EndMessage();
  boost::asio::io_context ctx;
  ip::udp::socket s{ctx};
  s.open(ip::udp::v4());
  s.set_option(ip::udp::socket::reuse_address(true));
  s.bind({ip::make_address_v4(from_ip), (unsigned short)from_port});
  s.send_to(
      boost::asio::buffer(p.Data(), p.Size()),
      {ip::make_address_v4("127.0.0.1"), (unsigned short)port});
}
}

TEST_CASE("test_oscquery_sender_address", "test_oscquery_sender_address")
{
  // A datagram from another address than a client's, from the source port
  // that client declared, is not that client's: it is forwarded to it like
  // any other value
  const auto other = other_ipv4();
  if(other.empty())
    SKIP("no IPv4 address other than 127.0.0.1 to send from");

  auto proto = new ossia::oscquery::oscquery_server_protocol{1730, 5730};
  generic_device dev{std::unique_ptr<protocol_base>(proto), "A"};
  dev.set_echo(true);
  auto level = find_or_create_node(dev, "/level").create_parameter(val_type::FLOAT);

  udp_sink sink;
  ossia_test::raw_ws_client client{5730};
  REQUIRE(client.connected());
  const int sender = 41730;
  client.send(ossia::oscquery::json_writer::start_osc_streaming(sink.port(), sender)
                  .GetString());
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  send_osc_float(1730, "/level", 0.25f, other, sender);
  for(int i = 0; i < 200 && level->value() != ossia::value{0.25f}; i++)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  REQUIRE(level->value() == ossia::value{0.25f});
  for(int i = 0; i < 200 && sink.count == 0; i++)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  REQUIRE(sink.count > 0);
}
