#pragma once
// What an OSCQuery client or server sends over UDP, read by a raw socket: the
// size of every datagram and the messages it holds, bundles opened.

#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/sockets/websocket_client.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

#include <oscpack/osc/OscReceivedElements.h>

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace
{
// A free port on the loopback, chosen by the system.
inline uint16_t free_port(bool tcp)
{
  boost::asio::io_context ctx;
  const auto loopback = boost::asio::ip::make_address("127.0.0.1");
  if(tcp)
  {
    boost::asio::ip::tcp::acceptor acceptor{ctx, {loopback, 0}};
    return acceptor.local_endpoint().port();
  }
  boost::asio::ip::udp::socket socket{ctx, {loopback, 0}};
  return socket.local_endpoint().port();
}

// Every datagram received on a UDP port, and the messages it holds.
struct udp_sink
{
  boost::asio::io_context ctx;
  boost::asio::ip::udp::socket socket{ctx};
  std::thread thread;
  std::mutex mutex;
  std::vector<std::size_t> datagrams;                  // size of each datagram
  std::vector<std::pair<std::string, float>> messages; // address, first float
  std::atomic_bool running{true};

  explicit udp_sink(uint16_t port)
  {
    socket.open(boost::asio::ip::udp::v4());
    socket.bind({boost::asio::ip::make_address("0.0.0.0"), port});
    socket.non_blocking(true);
    thread = std::thread([this] {
      std::vector<char> buf(65536);
      while(running)
      {
        boost::system::error_code ec;
        boost::asio::ip::udp::endpoint from;
        auto n = socket.receive_from(boost::asio::buffer(buf), from, 0, ec);
        if(ec)
        {
          std::this_thread::sleep_for(std::chrono::microseconds(200));
          continue;
        }
        std::lock_guard lock{mutex};
        datagrams.push_back(n);
        decode(buf.data(), n);
      }
    });
  }
  ~udp_sink()
  {
    running = false;
    thread.join();
  }
  void decode(const char* data, std::size_t size)
  {
    try
    {
      oscpack::ReceivedPacket p{data, (oscpack::osc_bundle_element_size_t)size};
      if(p.IsBundle())
      {
        oscpack::ReceivedBundle b{p};
        for(auto it = b.ElementsBegin(); it != b.ElementsEnd(); ++it)
          decode(it->Contents(), it->Size());
      }
      else
      {
        oscpack::ReceivedMessage m{p};
        float v = 0.f;
        if(m.ArgumentCount() > 0 && m.ArgumentsBegin()->IsFloat())
          v = m.ArgumentsBegin()->AsFloat();
        messages.emplace_back(m.AddressPattern(), v);
      }
    }
    catch(...)
    {
    }
  }
  void clear()
  {
    std::lock_guard lock{mutex};
    datagrams.clear();
    messages.clear();
  }
  // Waits until n messages arrived, or the time is up.
  bool wait_messages(std::size_t n, int ms)
  {
    for(int i = 0; i < ms / 5; i++)
    {
      {
        std::lock_guard lock{mutex};
        if(messages.size() >= n)
          return true;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::lock_guard lock{mutex};
    return messages.size() >= n;
  }
};

// An OSCQuery server exposed through a multiplex, as ossia-max exposes a
// device, with float leaves /load/p0 to /load/p<leaves - 1>. Its OSC port is
// read by a udp_sink: it receives what a client sends over UDP.
struct sink_server
{
  ossia::net::network_context_ptr ctx = ossia::net::create_network_context();
  ossia::net::multiplex_protocol* multiplex{};
  std::unique_ptr<ossia::net::generic_device> dev;
  uint16_t osc = free_port(false);
  uint16_t ws = free_port(true);
  udp_sink sink{osc};
  std::thread net;

  explicit sink_server(int leaves)
  {
    auto mp = std::make_unique<ossia::net::multiplex_protocol>();
    multiplex = mp.get();
    dev = std::make_unique<ossia::net::generic_device>(std::move(mp), "server");
    for(int i = 0; i < leaves; i++)
    {
      auto& n
          = ossia::net::create_node(dev->get_root_node(), "/load/p" + std::to_string(i));
      auto* p = ossia::try_setup_parameter("float", n);
      p->set_domain(ossia::make_domain(0.f, 1000000.f));
    }
    multiplex->expose_to(
        std::make_unique<ossia::oscquery_asio::oscquery_server_protocol_base>(
            ctx,
            std::vector<ossia::net::osc_server_configuration>{
                ossia::net::udp_server_configuration{{{"0.0.0.0"}, osc}}},
            ws, false));
    net = std::thread([this] { ctx->run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
  ~sink_server()
  {
    ossia::net::stop_network_context(*ctx);
    net.join();
  }
  std::string url() const { return "ws://127.0.0.1:" + std::to_string(ws); }
};

// A client of an OSCQuery server reduced to its WebSocket: it asks the server
// to send its values over UDP to a port of the test (START_OSC_STREAMING),
// and listens to the addresses it wants, as any client.
struct raw_client
{
  ossia::net::websocket_client ws{
      [](auto, websocketpp::frame::opcode::value, const std::string&) {}};
  std::thread thread;

  explicit raw_client(const std::string& url)
  {
    thread = std::thread([this, url] {
      try
      {
        ws.connect_and_run(url);
      }
      catch(...)
      {
      }
    });
    for(int i = 0; i < 400 && !ws.connected(); i++)
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  ~raw_client()
  {
    ws.stop();
    thread.join();
  }
  void stream_to(uint16_t udp_port)
  {
    auto msg = ossia::oscquery::json_writer::start_osc_streaming(udp_port, 0);
    ws.send_message(std::string(msg.GetString(), msg.GetSize()));
  }
  void listen_load(int n, const std::string& prefix = "/load/p")
  {
    for(int i = 0; i < n; i++)
      ws.send_message(ossia::oscquery::json_writer::listen(prefix + std::to_string(i)));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
};
}
