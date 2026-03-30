#pragma once
#include <ossia/network/sockets/var_size_prefix_framing.hpp>

namespace ossia::net
{

using size_prefix_framing = var_size_prefix_framing<4, byte_order::big_endian>;

// Backward-compatible aliases for direct use in QML / variant decoders
template <typename Socket>
using size_prefix_decoder = size_prefix_framing::decoder<Socket>;

template <typename Socket>
using size_prefix_encoder = size_prefix_framing::encoder<Socket>;

}
