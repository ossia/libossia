#pragma once
#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/osc_fwd.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>

#include <memory>
#include <vector>

namespace ossia::net
{

constexpr int max_osc_message_size = 65507;

struct buffer_packed_osc_stream
{
  std::unique_ptr<char[]> buffer;
  oscpack::OutboundPacketStream stream;
};

// returns the node if it was already learned
OSSIA_EXPORT
ossia::net::node_base*
osc_learn(ossia::net::node_base* n, const oscpack::ReceivedMessage& m);

}
