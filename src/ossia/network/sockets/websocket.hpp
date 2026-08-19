#pragma once
// This header now just includes the beast-based simple wrappers
// for use by osc_factory.cpp (OSC-over-WebSocket).
// The old websocketpp-based websocket_simple_client/websocket_simple_server
// have been replaced by the beast variants.
#include <ossia/network/sockets/websocket_simple_beast.hpp>
