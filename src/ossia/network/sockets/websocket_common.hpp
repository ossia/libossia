#pragma once

#include <memory>
#include <string>
#include <string_view>

namespace ossia::net
{

/// WebSocket frame opcode
enum class ws_opcode
{
  text,
  binary
};

/// Type-erased connection handle for WebSocket connections.
/// Both websocketpp and beast use weak_ptr<void> for this purpose.
using ws_connection_handle = std::weak_ptr<void>;

/// Parsed pieces of a ws://, wss://, http:// or https:// URI.
struct ws_parsed_uri
{
  std::string host;
  std::string port;
  std::string path;
  bool secure{false}; // true for wss:// or https://
};

/// Parses a URI of the form scheme://host[:port][/path]. The scheme can be
/// ws://, wss://, http:// or https://; if missing it is treated as ws://.
/// When no port is specified, the default depends on the scheme: 443 for
/// secure, 80 otherwise.
inline ws_parsed_uri parse_websocket_uri(std::string_view uri)
{
  ws_parsed_uri out;
  std::string_view sv = uri;

  if(sv.starts_with("wss://"))
  {
    sv.remove_prefix(6);
    out.secure = true;
  }
  else if(sv.starts_with("https://"))
  {
    sv.remove_prefix(8);
    out.secure = true;
  }
  else if(sv.starts_with("ws://"))
  {
    sv.remove_prefix(5);
  }
  else if(sv.starts_with("http://"))
  {
    sv.remove_prefix(7);
  }

  // Find path
  auto path_pos = sv.find('/');
  if(path_pos != std::string_view::npos)
  {
    out.path.assign(sv.substr(path_pos));
    sv = sv.substr(0, path_pos);
  }
  else
  {
    out.path = "/";
  }

  // Find port. IPv6 literals are surrounded by [], so be careful with the
  // last ':' search to skip the brackets.
  if(!sv.empty() && sv.front() == '[')
  {
    auto close = sv.find(']');
    if(close != std::string_view::npos)
    {
      out.host.assign(sv.substr(1, close - 1));
      sv = sv.substr(close + 1);
      if(!sv.empty() && sv.front() == ':')
        out.port.assign(sv.substr(1));
    }
    else
    {
      out.host.assign(sv);
    }
  }
  else
  {
    auto port_pos = sv.find(':');
    if(port_pos != std::string_view::npos)
    {
      out.host.assign(sv.substr(0, port_pos));
      out.port.assign(sv.substr(port_pos + 1));
    }
    else
    {
      out.host.assign(sv);
    }
  }

  if(out.port.empty())
    out.port = out.secure ? "443" : "80";

  return out;
}

}
