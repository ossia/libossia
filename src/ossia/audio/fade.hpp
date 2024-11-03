#pragma once
#include <ossia/dataflow/nodes/media.hpp>

namespace ossia::snd
{
OSSIA_EXPORT
void do_fade(
    bool start_discontinuous, bool end_discontinuous, audio_channel& ap,
    std::size_t start, std::size_t end);
}
