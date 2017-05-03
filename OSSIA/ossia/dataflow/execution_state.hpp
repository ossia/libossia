#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/network/base/device.hpp>
#include <unordered_map>
namespace ossia
{
struct OSSIA_EXPORT execution_state
{
  std::vector<ossia::net::device_base*> globalState;

  std::unordered_map<destination_t, value_port> valueState;
  std::unordered_map<destination_t, audio_port> audioState;
  std::unordered_map<destination_t, midi_port> midiState;

  void clear();

  void commit();

  void find_and_copy(ossia::net::address_base& addr, inlet& in);

  void copy_from_global(ossia::net::address_base& addr, inlet& in);

  // todo separate rvalue & cref
  void insert(const destination_t& dest, data_type v);
  void insert(const destination_t& dest, value_port v)
  {
    valueState[dest] = std::move(v);
  }
  void insert(const destination_t& dest, audio_port v)
  {
    audioState[dest] = std::move(v);
  }
  void insert(const destination_t& dest, midi_port v)
  {
    midiState[dest] = std::move(v);
  }

  bool in_local_scope(ossia::net::address_base& other) const;
};
}
