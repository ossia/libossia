#pragma once

#include <memory>

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

}
