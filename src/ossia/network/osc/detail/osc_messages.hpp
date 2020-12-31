#pragma once
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/osc_fwd.hpp>

#include <ossia/detail/algorithms.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>

#include <memory>
#include <vector>

namespace ossia::net
{

constexpr int max_osc_message_size = 65536;

struct buffer_packed_osc_stream
{
    std::unique_ptr<char[]> buffer;
    oscpack::OutboundPacketStream stream;
};

std::optional<buffer_packed_osc_stream> make_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& addresses);

std::optional<buffer_packed_osc_stream> make_bundle(
    const std::vector<const parameter_base*>& addresses);

void osc_learn(ossia::net::node_base* n, const oscpack::ReceivedMessage& m);


}
