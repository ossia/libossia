#pragma once
#include <array>
#include <cinttypes>
#include <utility>
namespace ossia
{
namespace net
{
namespace midi
{
using midi_size_t = uint8_t;
class midi_address;
struct midi_channel
{
  // [ note, vel ]
  std::pair<midi_size_t, midi_size_t> mNoteOn;
  std::pair<midi_size_t, midi_size_t> mNoteOff;

  // [ CC, value ]
  std::pair<midi_size_t, midi_size_t> mCC;

  // PC
  midi_size_t mPC;

  // velocity or value
  std::array<midi_size_t, 127> mNoteOn_N = {{64}};
  std::array<midi_size_t, 127> mNoteOff_N = {{64}};
  std::array<midi_size_t, 127> mCC_N = {{64}};
  // No need to store PC since they are only impulses

  // Callbacks
  midi_address* mCallbackNoteOn{};
  midi_address* mCallbackNoteOff{};
  midi_address* mCallbackCC{};
  midi_address* mCallbackPC{};
  std::array<midi_address*, 127> mCallbackNoteOn_N = {{}};
  std::array<midi_address*, 127> mCallbackNoteOff_N = {{}};
  std::array<midi_address*, 127> mCallbackCC_N = {{}};
  std::array<midi_address*, 127> mCallbackPC_N = {{}};
};
}
}
}
