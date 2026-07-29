#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/flat_multiset.hpp>

#include <libremidi/ump_events.hpp>

#include <algorithm>

namespace ossia::nodes
{

using midi_size_t = uint8_t;
struct note_data
{
  time_value start{};
  time_value duration{};

  midi_size_t pitch{};
  midi_size_t velocity{};
};

inline bool is_note_off(const libremidi::ump& m) noexcept
{
  return (m.get_status_code() & 0xF0) == 0x80;
}

struct note_comparator
{
  using is_transparent = std::true_type;
  bool operator()(const note_data& lhs, const note_data& rhs) const
  {
    return lhs.start < rhs.start;
  }
  bool operator()(const note_data& lhs, int64_t rhs) const
  {
    return lhs.start.impl < rhs;
  }
};

class midi final : public ossia::nonowning_graph_node
{
  ossia::midi_outlet midi_out;

public:
  using note_set = ossia::flat_multiset<note_data, note_comparator>;
  explicit midi(int64_t notes)
  {
    m_outlets.push_back(&midi_out);
    int64_t to_reserve = std::max(notes * 1.1, 128.);
    m_notes.reserve(to_reserve);
    m_orig_notes.reserve(to_reserve);
    m_playing_notes.reserve(to_reserve);
    m_to_stop.reserve(64);
    m_to_resume.reserve(64);
  }

  ~midi() override = default;

  std::string label() const noexcept override { return "midi"; }

  void set_channel(int c) { m_channel = c - 1; }

  void add_note(note_data nd)
  {
    m_orig_notes.insert(nd);
    // The playing scan takes start >= t.prev_date, and m_prev_date is where the
    // next tick starts: a note landing exactly there is still to be played.
    // With a strict comparison it was dropped, and every note at date 0 with it.
    if(nd.start >= m_prev_date)
    {
      m_notes.insert(nd);
    }
  }

  // note_comparator only orders on the start date: erasing by key would remove
  // every note of a chord, and find() could return a note of another pitch.
  static bool erase_exact(note_set& set, const note_data& nd)
  {
    auto [first, last] = set.equal_range(nd);
    for(auto it = first; it != last; ++it)
    {
      if(it->pitch == nd.pitch)
      {
        set.erase(it);
        return true;
      }
    }
    return false;
  }

  void remove_note(note_data nd)
  {
    erase_exact(m_orig_notes, nd);
    erase_exact(m_notes, nd);
    if(erase_exact(m_playing_notes, nd))
      m_to_stop.insert(nd);
  }

  void replace_notes(note_set&& notes)
  {
    for(auto& note : m_playing_notes)
      m_to_stop.insert(note);
    m_playing_notes.clear();

    m_to_resume.clear();

    using namespace std;
    swap(m_orig_notes, notes);
    m_notes.clear();

    auto start_it = m_orig_notes.lower_bound(m_prev_date.impl);
    if(start_it != m_orig_notes.end())
    {
      m_notes.tree().get_sequence_ref().assign(start_it, m_orig_notes.end());
    }
  }

  void transport(ossia::time_value date)
  {
    requestTransport = true;
    m_transport_date = date;
  }

  void transport_impl(ossia::time_value date)
  {
    // Whatever was playing has to be released: the playhead no longer follows
    // from it.
    m_to_stop.insert(m_playing_notes.begin(), m_playing_notes.end());
    m_playing_notes.clear();
    m_to_resume.clear();

    // Everything starting at or after the new position is to be played again,
    // and a note the position lands inside of is resumed from its middle -
    // the note-on scan can never pick those up, since it only matches notes
    // starting at or after the beginning of a tick.
    m_notes.clear();
    for(const note_data& n : m_orig_notes)
    {
      if(n.start >= date)
        m_notes.insert(n);
      else if((n.start + n.duration) > date)
        m_to_resume.insert(n);
    }

    m_prev_date = date;
  }

  void update_note(note_data oldNote, note_data newNote)
  {
    // OPTIMIZEME
    remove_note(oldNote);
    add_note(newNote);
  }

  void set_notes(note_set&& notes)
  {
    m_notes = std::move(notes);
    m_orig_notes = m_notes;

    auto max_it = m_notes.lower_bound({m_prev_date});
    if(max_it != m_notes.begin()) // TODO handle the begin case correctly
      m_notes.erase(m_notes.begin(), max_it);
  }

  bool mustStop{};
  bool requestTransport{};

private:
  //! Notes the playhead landed inside of after a transport: they are entered
  //! from the beginning of the tick rather than from their own start date.
  void resume_notes(ossia::midi_port& mp, int64_t tick_start)
  {
    for(const note_data& note : m_to_resume)
    {
      mp.messages.push_back(
          libremidi::from_midi1::note_on(m_channel, note.pitch, note.velocity));
      mp.messages.back().timestamp = tick_start;
      m_playing_notes.insert(note);
    }
    m_to_resume.clear();
  }

  //! Reverse playback: rewinding past the start of a note unwinds it, so it is
  //! released and becomes playable again. Notes are deliberately not entered
  //! from their tail - a note beginning at its own end is not a useful sound.
  void rewind(
      const ossia::token_request& t, ossia::midi_port& mp, double samplesratio,
      int64_t tick_start)
  {
    // physical_write_duration divides an absolute duration by a signed speed,
    // so it comes out negative when rewinding.
    const auto tick_end
        = tick_start + std::abs(t.physical_write_duration(samplesratio));
    const auto stamp = [&](time_value at) {
      return std::clamp(
          t.to_physical_time_in_tick(at, samplesratio), tick_start,
          std::max(tick_start, tick_end - 1));
    };

    for(auto it = m_playing_notes.begin(); it != m_playing_notes.end();)
    {
      if(it->start > t.date)
      {
        mp.messages.push_back(
            libremidi::from_midi1::note_off(m_channel, it->pitch, 0));
        mp.messages.back().timestamp = stamp(it->start);
        it = m_playing_notes.erase(it);
      }
      else
      {
        ++it;
      }
    }

    // Every note whose start the playhead went back over must be playable
    // again. Exactly once: m_notes is a multiset, and the ones that had not
    // been played yet are still in it.
    const auto first = m_orig_notes.upper_bound({t.date});
    const auto last = m_orig_notes.upper_bound({t.prev_date});
    for(auto it = first; it != last; ++it)
    {
      erase_exact(m_notes, *it);
      m_notes.insert(*it);
    }
  }

  //! Note-offs for every note whose end date has been reached.
  //! Overdue ends (the note both started and finished inside a single tick, or
  //! a tick was skipped) are stamped at the beginning of the tick instead of
  //! being dropped: t.in_range() can never become true for them again.
  void stop_finished_notes(
      const ossia::token_request& t, ossia::midi_port& mp, double samplesratio,
      int64_t tick_start)
  {
    for(auto it = m_playing_notes.begin(); it != m_playing_notes.end();)
    {
      const note_data& note = *it;
      const auto end_time = note.start + note.duration;

      if(end_time < t.date)
      {
        mp.messages.push_back(libremidi::from_midi1::note_off(m_channel, note.pitch, 0));
        mp.messages.back().timestamp
            = std::max(tick_start, t.to_physical_time_in_tick(end_time, samplesratio));

        it = m_playing_notes.erase(it);
      }
      else
      {
        ++it;
      }
    }
  }

  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    ossia::midi_port& mp = *midi_out;

    struct scope_guard
    {
      midi& self;
      const ossia::token_request& t;
      ossia::midi_port& mp;
      std::size_t first_message;
      ~scope_guard()
      {
        // Consumers (VST3 event lists, MIDI outputs...) expect events in
        // chronological order, and a note-off must win over a note-on sharing
        // its timestamp, else back-to-back notes on the same pitch cancel out.
        auto begin = mp.messages.begin() + first_message;
        std::stable_sort(begin, mp.messages.end(), [](const auto& lhs, const auto& rhs) {
          if(lhs.timestamp != rhs.timestamp)
            return lhs.timestamp < rhs.timestamp;
          return is_note_off(lhs) && !is_note_off(rhs);
        });

        self.m_prev_date = t.date;

        if(self.requestTransport)
        {
          self.transport_impl(self.m_transport_date);
          self.requestTransport = false;
        }
      }
    } guard{*this, t, mp, mp.messages.size()};

    const auto samplesratio = e.modelToSamples();
    const auto tick_start = t.physical_start(samplesratio);

    if(t.end_discontinuous)
    {
      auto& mess = mp.messages;
      for(auto note : m_playing_notes)
      {
        mess.push_back(libremidi::from_midi1::note_off(m_channel, note.pitch, 0));
        mess.back().timestamp = tick_start;
      }
      for(auto note : m_to_stop)
      {
        mess.push_back(libremidi::from_midi1::note_off(m_channel, note.pitch, 0));
        mess.back().timestamp = tick_start;
      }
      m_playing_notes.clear();
      m_to_stop.clear();
      return;
    }

    for(const note_data& note : m_to_stop)
    {
      mp.messages.push_back(libremidi::from_midi1::note_off(m_channel, note.pitch, 0));
      mp.messages.back().timestamp = tick_start;
    }
    m_to_stop.clear();

    if(mustStop)
    {
      for(auto& note : m_playing_notes)
      {
        mp.messages.push_back(libremidi::from_midi1::note_off(m_channel, note.pitch, 0));
        mp.messages.back().timestamp = tick_start;
      }

      m_notes = m_orig_notes;
      m_playing_notes.clear();
      m_to_resume.clear();

      mustStop = false;
    }
    else
    {
      if(m_notes.empty() && m_playing_notes.empty() && m_to_resume.empty())
        return;

      resume_notes(mp, tick_start);

      if(t.backward())
      {
        rewind(t, mp, samplesratio, tick_start);
      }
      else if(t.forward())
      {
        stop_finished_notes(t, mp, samplesratio, tick_start);

        // Look for all the messages
        auto max_it = m_notes.lower_bound({t.date});
        for(auto it = m_notes.begin(); it < max_it;)
        {
          note_data& note = const_cast<note_data&>(*it);
          auto start_time = note.start;
          if(start_time >= t.prev_date && start_time < t.date)
          {
            // Send note_on
            mp.messages.push_back(
                libremidi::from_midi1::note_on(m_channel, note.pitch, note.velocity));
            mp.messages.back().timestamp
                = t.to_physical_time_in_tick(start_time, samplesratio);

            m_playing_notes.insert(note);
            it = m_notes.erase(it);
            max_it = std::lower_bound(
                it, m_notes.end(), t.date.impl + 1, note_comparator{});
          }
          else
          {
            ++it;
          }
        }

        // Notes short enough to begin and end inside this tick were not in
        // m_playing_notes when the first pass ran: stop them here.
        stop_finished_notes(t, mp, samplesratio, tick_start);
      }
    }
  }

  note_set m_notes;
  note_set m_orig_notes;
  note_set m_playing_notes;
  note_set m_to_stop;
  note_set m_to_resume;
  time_value m_prev_date{};
  time_value m_transport_date{};

  int m_channel{};
};

class midi_node_process final : public ossia::node_process
{
public:
  using ossia::node_process::node_process;

  void transport_impl(ossia::time_value date) override
  {
    midi& n = *static_cast<midi*>(node.get());
    n.transport(date);
  }

  void stop() override
  {
    midi& n = *static_cast<midi*>(node.get());
    n.request(ossia::token_request{});
    n.mustStop = true;
  }
};
}
