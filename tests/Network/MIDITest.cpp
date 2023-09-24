// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProtocolTestUtils.hpp"
#include "TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include "include_catch.hpp"

#if defined(OSSIA_PROTOCOL_MIDI)
#include <ossia/protocols/midi/midi.hpp>
#endif

#ifdef OSSIA_PROTOCOL_MIDI
TEST_CASE("test_midi", "test_midi")
{
  using namespace ossia::net::midi;
  try
  {
    auto ctx = ossia::net::create_network_context();
    for(auto e : midi_protocol::scan())
    {
      ossia::net::midi::midi_device dev(std::make_unique<midi_protocol>(ctx, e));
      dev.set_name("dada");
      dev.create_full_tree();
    }
  }
  catch(...)
  {
  }
}
#endif
