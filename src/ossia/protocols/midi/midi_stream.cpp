#include <ossia/protocols/midi/midi_stream.hpp>

namespace ossia::net::midi
{
midi_stream::~midi_stream() = default;

std::optional<int> midi_stream::stream_channel(const node_base&) const noexcept
{
  return std::nullopt;
}
}
