#pragma once
#include <libremidi/cmidi2.hpp>
#include <libremidi/detail/conversion.hpp>
#include <libremidi/ump.hpp>

#include <array>
#include <bit>
#include <cstdint>

namespace ossia
{
/**
 * The notes a MIDI stream has started and not yet stopped.
 *
 * Whoever cuts a stream short (a cable taken off, a node removed, an output
 * rebound to another device) asks for what is left sounding and stops it at
 * the far end, which otherwise never gets the note off.
 *
 * One bit per (channel, pitch): a note on sets it, a note off clears it, so
 * tracking a message is a mask and a store, and nothing is ever allocated.
 * Releasing is conservative on purpose: a note off for a note that does not
 * sound is ignored by receivers, a missing one is a note that never ends.
 * Hence
 *  - a bit, not a count: two note ons for one pitch are stopped by one note
 *    off, the way MIDI 1 receivers take them;
 *  - the UMP groups are not part of the key: the notes are stopped on every
 *    group that carried a note on while some were held, almost always one;
 *  - "all notes off" and "reset all controllers" do not clear anything, since
 *    not every receiver honours them.
 *
 * Tracking keeps no summary of the table up to date: that would be paid on
 * every message, for the sake of a release that happens once.
 *
 * The damper pedal is tracked along: notes released under a pedal that stays
 * down ring forever just as well.
 */
struct midi_note_tracker
{
  static constexpr int channels = 16;
  static constexpr int pitches = 128;

  //! Bit (channel * 128 + pitch)
  std::array<uint64_t, channels * pitches / 64> notes{};
  //! Channels whose damper pedal (CC 64) is down
  uint16_t damper{};
  //! UMP groups that carried a note on or a pedal. Only ever grows: it says
  //! where to send the note offs, not whether there is something to stop.
  uint16_t groups{};
  //! The last note came as a MIDI 1 channel voice message: stop it in kind
  bool midi1{};

  //! Looks at the whole table: for when a stream is cut, not for every tick.
  [[nodiscard]] bool empty() const noexcept
  {
    uint64_t any = damper;
    for(uint64_t w : notes)
      any |= w;
    return any == 0;
  }

  void clear() noexcept { *this = midi_note_tracker{}; }

  void track(const libremidi::ump& m) noexcept { track(&m, &m + 1); }

  template <typename Messages>
  void track(const Messages& messages) noexcept
  {
    track(messages.data(), messages.data() + messages.size());
  }

  //! A tick's worth at once. The state other than the table is kept out of
  //! memory for the length of the loop: most messages of a stream are not
  //! notes, and those then cost a test and nothing else.
  void track(const libremidi::ump* it, const libremidi::ump* end) noexcept
  {
    uint32_t cur_groups = groups;
    uint32_t cur_damper = damper;
    bool cur_midi1 = midi1;

    for(; it != end; ++it)
    {
      const uint32_t w = it->data[0];
      const uint32_t type = w >> 28;
      // MIDI 1 and MIDI 2 channel voice messages share the layout of their
      // first word up to the pitch / controller index.
      constexpr uint32_t channel_voice = (1u << CMIDI2_MESSAGE_TYPE_MIDI_1_CHANNEL)
                                         | (1u << CMIDI2_MESSAGE_TYPE_MIDI_2_CHANNEL);
      if(!((channel_voice >> type) & 1))
        continue;

      const bool is_midi1 = type == CMIDI2_MESSAGE_TYPE_MIDI_1_CHANNEL;
      const uint32_t status = (w >> 20) & 0xF;
      if((status & 0xE) == 0x8)
      {
        // In MIDI 1 a note on of velocity zero is a note off; in MIDI 2 it
        // is a note.
        const bool on = (status & 1) && !(is_midi1 && (w & 0x7F) == 0);

        // channel * 128 + pitch
        const uint32_t key = ((w >> 9) & 0x780) | ((w >> 8) & 0x7F);
        uint64_t& word = notes[key >> 6];
        const uint64_t bit = uint64_t(1) << (key & 63);
        word = on ? (word | bit) : (word & ~bit);
        if(on)
        {
          cur_groups |= 1u << ((w >> 24) & 0xF);
          cur_midi1 = is_midi1;
        }
      }
      else if(status == 0xB && ((w >> 8) & 0x7F) == 64)
      {
        const uint32_t channel = (w >> 16) & 0xF;
        const bool down = is_midi1 ? (w & 0x7F) >= 64 : it->data[1] >= 0x80000000u;
        cur_damper = (cur_damper & ~(1u << channel)) | (uint32_t(down) << channel);
        if(down)
        {
          cur_groups |= 1u << ((w >> 24) & 0xF);
          cur_midi1 = is_midi1;
        }
      }
    }

    groups = uint16_t(cur_groups);
    damper = uint16_t(cur_damper);
    midi1 = cur_midi1;
  }

  //! Appends what stops everything held to \p out, stamped at \p timestamp:
  //! the pedals up, then a note off per note. The tracker is left untouched:
  //! the stream may go on elsewhere.
  template <typename Messages>
  void release(Messages& out, int64_t timestamp = 0) const
  {
    if(empty())
      return;

    for(uint32_t gs = groups; gs != 0; gs &= gs - 1)
    {
      const auto group = uint8_t(std::countr_zero(gs));

      for(uint32_t cs = damper; cs != 0; cs &= cs - 1)
      {
        const auto channel = uint8_t(std::countr_zero(cs));
        push(
            out, timestamp, midi1,
            midi1 ? int64_t(cmidi2_ump_midi1_cc(group, channel, 64, 0))
                  : cmidi2_ump_midi2_cc(group, channel, 64, 0));
      }

      for(uint8_t channel = 0; channel < channels; channel++)
      {
        for(int half = 0; half < 2; half++)
        {
          for(uint64_t bits = notes[channel * 2 + half]; bits != 0; bits &= bits - 1)
          {
            const auto pitch = uint8_t(half * 64 + std::countr_zero(bits));
            push(
                out, timestamp, midi1,
                midi1 ? int64_t(cmidi2_ump_midi1_note_off(group, channel, pitch, 0))
                      : cmidi2_ump_midi2_note_off(group, channel, pitch, 0, 0, 0));
          }
        }
      }
    }
  }

  //! Number of notes held
  [[nodiscard]] int count() const noexcept
  {
    int n = 0;
    for(uint64_t w : notes)
      n += std::popcount(w);
    return n;
  }

private:
  template <typename Messages>
  static void push(Messages& out, int64_t timestamp, bool is_midi1, int64_t msg)
  {
    libremidi::ump u;
    if(is_midi1)
      u.data[0] = uint32_t(msg);
    else
      cmidi2_reverse(msg, u.data);
    u.timestamp = timestamp;
    out.push_back(u);
  }
};
}
