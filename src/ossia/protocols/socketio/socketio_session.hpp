#pragma once

#include <ossia/detail/config.hpp>
#include <ossia/detail/logger.hpp>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/json.hpp>

#include <charconv>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>

namespace ossia::net
{

/// Engine.IO v4 packet types
enum engine_io_packet_type : char
{
  EIO_OPEN = '0',
  EIO_CLOSE = '1',
  EIO_PING = '2',
  EIO_PONG = '3',
  EIO_MESSAGE = '4',
  EIO_UPGRADE = '5',
  EIO_NOOP = '6',
};

/// Socket.IO v5 packet types (carried inside EIO_MESSAGE)
enum socket_io_packet_type : char
{
  SIO_CONNECT = '0',
  SIO_DISCONNECT = '1',
  SIO_EVENT = '2',
  SIO_ACK = '3',
  SIO_CONNECT_ERROR = '4',
  SIO_BINARY_EVENT = '5',
  SIO_BINARY_ACK = '6',
};

/// Engine.IO configuration received from the server handshake
struct engineio_config
{
  std::string sid;
  std::string socketio_sid;
  std::chrono::milliseconds ping_interval{25000};
  std::chrono::milliseconds ping_timeout{20000};
  int64_t max_payload{1000000};
  bool can_websocket{false};
};

/// Parses Engine.IO OPEN packet JSON
inline bool parse_engineio_open(std::string_view str, engineio_config& config)
{
  if(str.size() < 2 || !str.starts_with("0{"))
    return false;

  str = str.substr(1);
  auto json = boost::json::parse(str);
  if(auto obj = json.try_as_object())
  {
    auto& o = *obj;
    if(auto k = o.find("sid"))
      if(auto sid = k->value().try_as_string())
        config.sid = *sid;

    if(auto k = o.find("upgrades"))
      if(auto v = k->value().try_as_array())
        if(v->size() > 0 && v->front() == "websocket")
          config.can_websocket = true;

    if(auto k = o.find("pingInterval"))
      if(auto v = k->value().try_as_int64())
        config.ping_interval = std::chrono::milliseconds(*v);

    if(auto k = o.find("pingTimeout"))
      if(auto v = k->value().try_as_int64())
        config.ping_timeout = std::chrono::milliseconds(*v);

    if(auto k = o.find("maxPayload"))
      if(auto v = k->value().try_as_int64())
        config.max_payload = *v;
  }
  return !config.sid.empty();
}

/// Parses Socket.IO CONNECT response (e.g. "40{"sid":"..."}")
inline bool parse_socketio_connect(std::string_view str, engineio_config& config)
{
  if(str.size() < 4 || !str.starts_with("40{"))
    return false;
  if(auto end = str.find('\x1e'); end != std::string_view::npos)
    str = str.substr(0, end);

  str = str.substr(2);
  auto json = boost::json::parse(str);
  if(auto obj = json.try_as_object())
  {
    if(auto k = obj->find("sid"))
      if(auto sid = k->value().try_as_string())
      {
        config.socketio_sid = *sid;
        return true;
      }
  }
  return false;
}

/// Generates Engine.IO OPEN packet JSON
inline std::string make_engineio_open(const engineio_config& config)
{
  boost::json::object obj;
  obj["sid"] = config.sid;
  obj["upgrades"] = boost::json::array{"websocket"};
  obj["pingInterval"] = config.ping_interval.count();
  obj["pingTimeout"] = config.ping_timeout.count();
  obj["maxPayload"] = config.max_payload;
  return std::string(1, EIO_OPEN) + boost::json::serialize(obj);
}

/// Generates Socket.IO CONNECT response
inline std::string make_socketio_connect(const std::string& sid)
{
  boost::json::object obj;
  obj["sid"] = sid;
  return std::string("40") + boost::json::serialize(obj);
}

/// Helper to consume a leading integer from a string_view
inline std::optional<int> consume_int(std::string_view& input)
{
  int out;
  const std::from_chars_result result
      = std::from_chars(input.data(), input.data() + input.size(), out);
  if(result.ec == std::errc::invalid_argument
     || result.ec == std::errc::result_out_of_range)
    return std::nullopt;

  int n = result.ptr - input.data();
  input = input.substr(n);
  return out;
}

}
