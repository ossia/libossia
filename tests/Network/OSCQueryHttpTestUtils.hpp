#pragma once
// A raw HTTP client and a raw WebSocket client for the OSCQuery server tests:
// what goes on the wire, without the mirror in between.
#include <ossia/network/sockets/websocket_client.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <chrono>
#include <cstdio>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace ossia_test
{
struct http_reply
{
  int status{};
  std::string body;
};

//! GET <target> on 127.0.0.1:port; status 0 if nothing answered
inline http_reply http_get(int port, const std::string& target)
{
  http_reply r;
  try
  {
    boost::asio::io_context ctx;
    boost::asio::ip::tcp::socket sock{ctx};
    sock.connect({boost::asio::ip::make_address("127.0.0.1"), (unsigned short)port});
    const std::string req
        = "GET " + target + " HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n";
    boost::asio::write(sock, boost::asio::buffer(req));

    std::string reply;
    boost::system::error_code ec;
    char buf[4096];
    for(std::size_t n; (n = sock.read_some(boost::asio::buffer(buf), ec)) > 0 && !ec;)
      reply.append(buf, n);

    int code = 0;
    if(std::sscanf(reply.c_str(), "HTTP/1.1 %d", &code) == 1)
      r.status = code;
    if(auto body = reply.find("\r\n\r\n"); body != std::string::npos)
      r.body = reply.substr(body + 4);
  }
  catch(...)
  {
  }
  return r;
}

//! A WebSocket client that sends text as given and keeps what it receives
struct raw_ws_client
{
  explicit raw_ws_client(int port)
      : client{[this](auto&&, auto, const std::string& msg) {
        std::lock_guard l{mutex};
        received.push_back(msg);
      }}
  {
    thread = std::thread{[this, port] {
      client.connect_and_run("ws://127.0.0.1:" + std::to_string(port));
    }};
    for(int i = 0; i < 200 && !client.connected(); i++)
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  ~raw_ws_client()
  {
    client.stop();
    if(thread.joinable())
      thread.join();
  }

  bool connected() const { return client.connected(); }
  void send(const std::string& text) { client.send_message(text); }

  std::vector<std::string> messages()
  {
    std::lock_guard l{mutex};
    return received;
  }

  //! How many messages received hold that text
  int count(const std::string& what)
  {
    std::lock_guard l{mutex};
    int n = 0;
    for(auto& m : received)
      n += m.find(what) != std::string::npos;
    return n;
  }

  std::mutex mutex;
  std::vector<std::string> received;
  ossia::net::websocket_client client;
  std::thread thread;
};
}
