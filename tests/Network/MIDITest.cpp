// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProtocolTestUtils.hpp"
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <libremidi/libremidi.hpp>
#if defined(OSSIA_PROTOCOL_MIDI)
#include <ossia/protocols/midi/midi.hpp>

TEST_CASE("test_midi", "test_midi")
{
  using namespace ossia::net::midi;
  try
  {
    auto ctx = ossia::net::create_network_context();
    for(auto midi_api : libremidi::available_apis())
    {
      libremidi::observer_configuration conf;
      libremidi::observer obs{conf, libremidi::observer_configuration_for(midi_api)};

      for(auto port : obs.get_input_ports())
      {
        libremidi::input_configuration conf;
        auto api_conf = libremidi::midi_in_configuration_for(midi_api);

        auto proto
            = std::make_unique<midi_protocol>(ctx, port.display_name, conf, api_conf);
        proto->set_info(midi_info{midi_info::Type::Input, port, false});

        midi_device dev(std::move(proto));
        dev.set_name("test");
        dev.create_full_tree();
      }
      for(auto port : obs.get_output_ports())
      {
        libremidi::input_configuration conf;
        auto api_conf = libremidi::midi_in_configuration_for(midi_api);

        auto proto
            = std::make_unique<midi_protocol>(ctx, port.display_name, conf, api_conf);
        proto->set_info(midi_info{midi_info::Type::Input, port, false});

        midi_device dev(std::move(proto));
        dev.set_name("test");
        dev.create_full_tree();
      }
    }
  }
  catch(...)
  {
  }
}
#endif
