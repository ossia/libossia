#pragma once
#include <utility>
#include <cinttypes>
namespace OSSIA
{
namespace net
{

using midi_size_t = uint8_t;
class MIDIAddress;
struct Channel
{
    // [ note, vel ]
    std::pair<midi_size_t, midi_size_t> mNoteOn;
    std::pair<midi_size_t, midi_size_t> mNoteOff;

    // [ CC, value ]
    std::pair<midi_size_t, midi_size_t> mCC;

    // PC
    midi_size_t mPC;

    // velocity or value
    std::array<midi_size_t, 127> mNoteOn_N = { { 64 } };
    std::array<midi_size_t, 127> mNoteOff_N = { { 64 } };
    std::array<midi_size_t, 127> mCC_N = { { 64 } };
    // No need to store PC since they are only impulses

    // Callbacks
    MIDIAddress* mCallbackNoteOn{};
    MIDIAddress* mCallbackNoteOff{};
    MIDIAddress* mCallbackCC{};
    MIDIAddress* mCallbackPC{};
    std::array<MIDIAddress*, 127> mCallbackNoteOn_N = { {} };
    std::array<MIDIAddress*, 127> mCallbackNoteOff_N = { {} };
    std::array<MIDIAddress*, 127> mCallbackCC_N = { {} };
    std::array<MIDIAddress*, 127> mCallbackPC_N = { {} };
};

}
}
