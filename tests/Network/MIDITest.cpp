// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include "TestUtils.hpp"
#include "ProtocolTestUtils.hpp"

#if defined(OSSIA_PROTOCOL_MIDI)
#include <ossia/network/midi/midi.hpp>
#endif

#ifdef OSSIA_PROTOCOL_MIDI
TEST_CASE ("test_midi", "test_midi")
  {
    using namespace ossia::net::midi;
    try {
      auto proto = std::make_unique<midi_protocol>();
      auto res = proto->scan();
      for(auto e : res)
      {
        ossia::net::midi::midi_device dev(std::make_unique<midi_protocol>(e));
        dev.set_name("dada");
        dev.create_full_tree();
      }

    }
    catch(...)
    {
    }
  }
#endif
