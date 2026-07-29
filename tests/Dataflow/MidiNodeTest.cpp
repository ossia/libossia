#include <ossia/detail/config.hpp>

#include <ossia/dataflow/exec_state_facade.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/nodes/midi.hpp>

#include "include_catch.hpp"

#include <algorithm>
#include <vector>

namespace
{
constexpr int note_on = CMIDI2_STATUS_NOTE_ON;
constexpr int note_off = CMIDI2_STATUS_NOTE_OFF;

//! A MIDI 2.0 UMP channel-voice packet as produced by the node.
struct decoded
{
  int status{};
  int channel{};
  int note{};
  int velocity{}; // 7-bit: the 16-bit MIDI2 velocity downscaled
  int64_t timestamp{};
};

decoded decode(const libremidi::ump& u)
{
  return {
      cmidi2_ump_get_status_code(u.data), cmidi2_ump_get_channel(u.data),
      cmidi2_ump_get_midi2_note_note(u.data),
      cmidi2_ump_get_midi2_note_velocity(u.data) / 0x200, u.timestamp};
}

ossia::nodes::note_data
note(int64_t start, int64_t dur, uint8_t pitch, uint8_t vel = 100)
{
  return {ossia::time_value{start}, ossia::time_value{dur}, pitch, vel};
}

//! Holds a node with its execution state and decodes what its outlet produces.
struct fixture
{
  ossia::nodes::midi node{16};
  ossia::execution_state st; // modelToSamplesRatio == 1: model time == samples
  ossia::midi_port& port{*node.root_outputs()[0]->target<ossia::midi_port>()};

  //! Populates the node the way the executor does for the initial content.
  void set(std::initializer_list<ossia::nodes::note_data> notes)
  {
    ossia::nodes::midi::note_set s;
    for(const auto& n : notes)
      s.insert(n);
    node.set_notes(std::move(s));
  }

  std::vector<decoded> run(ossia::token_request tk)
  {
    port.messages.clear();
    ossia::exec_state_facade fac{&st};
    static_cast<ossia::graph_node&>(node).run(tk, fac);

    std::vector<decoded> out;
    out.reserve(port.messages.size());
    for(const auto& m : port.messages)
      out.push_back(decode(m));
    return out;
  }

  static ossia::token_request
  token(int64_t prev, int64_t date, int64_t offset = 0, double speed = 1.)
  {
    return {ossia::time_value{prev},   ossia::time_value{date},
            ossia::time_value{100000}, ossia::time_value{offset},
            speed,                     ossia::time_signature{4, 4},
            120.};
  }

  //! Runs one tick. Timestamps in the result are relative to the tick.
  std::vector<decoded>
  tick(int64_t prev, int64_t date, int64_t offset = 0, double speed = 1.)
  {
    return run(token(prev, date, offset, speed));
  }

  std::vector<decoded> tick_discontinuous(int64_t prev, int64_t date, int64_t offset)
  {
    auto tk = token(prev, date, offset);
    tk.end_discontinuous = true;
    return run(tk);
  }
};
}

TEST_CASE("midi node: a note shorter than the tick is still released", "[midi][node]")
{
  // The note-off scan used to run only at the top of the tick, over the notes
  // that were already playing: a note added to m_playing_notes by the note-on
  // scan below it could not be released in that same tick, and in_range() made
  // the miss permanent, its end date being in the past from then on.
  fixture f;
  f.set({note(10, 10, 60)}); // [10, 20), entirely inside the first tick

  auto msgs = f.tick(0, 100);

  REQUIRE(msgs.size() == 2);
  CHECK(msgs[0].status == note_on);
  CHECK(msgs[0].note == 60);
  CHECK(msgs[0].timestamp == 10);
  CHECK(msgs[1].status == note_off);
  CHECK(msgs[1].note == 60);
  CHECK(msgs[1].timestamp == 20);

  // and nothing is left playing
  CHECK(f.tick(100, 200).empty());
}

TEST_CASE("midi node: several notes inside one tick", "[midi][node]")
{
  fixture f;
  f.set({note(0, 40, 60), note(50, 40, 62)});

  auto msgs = f.tick(0, 100);

  REQUIRE(msgs.size() == 4);
  CHECK(msgs[0].status == note_on);
  CHECK(msgs[0].note == 60);
  CHECK(msgs[0].timestamp == 0);
  CHECK(msgs[1].status == note_off);
  CHECK(msgs[1].note == 60);
  CHECK(msgs[1].timestamp == 40);
  CHECK(msgs[2].status == note_on);
  CHECK(msgs[2].note == 62);
  CHECK(msgs[2].timestamp == 50);
  CHECK(msgs[3].status == note_off);
  CHECK(msgs[3].note == 62);
  CHECK(msgs[3].timestamp == 90);
}

TEST_CASE("midi node: a tick comes out in chronological order", "[midi][node]")
{
  // A note-off late in the tick must not be emitted before a note-on that
  // happens earlier in it: consumers such as VST3 event lists require ordered
  // events, and an out-of-order pair silently cancels a note.
  fixture f;
  // 60 ends at 90 and 62 starts at 60: both fall in the second tick
  f.set({note(0, 90, 60), note(60, 40, 62)});

  REQUIRE(f.tick(0, 50).size() == 1); // note 60 starts

  auto msgs = f.tick(50, 100);

  REQUIRE(msgs.size() == 2);
  CHECK(msgs[0].status == note_on);
  CHECK(msgs[0].note == 62);
  CHECK(msgs[0].timestamp == 10); // model 60 - tick start 50
  CHECK(msgs[1].status == note_off);
  CHECK(msgs[1].note == 60);
  CHECK(msgs[1].timestamp == 40); // model 90 - tick start 50
}

TEST_CASE("midi node: back-to-back notes on the same pitch", "[midi][node]")
{
  // The note-off of the first and the note-on of the second share a timestamp:
  // the note-off has to win the tie, else the second note is cut immediately.
  fixture f;
  f.set({note(0, 100, 60), note(100, 100, 60)});

  REQUIRE(f.tick(0, 100).size() == 1);

  auto msgs = f.tick(100, 200);

  REQUIRE(msgs.size() == 2);
  CHECK(msgs[0].status == note_off);
  CHECK(msgs[0].note == 60);
  CHECK(msgs[0].timestamp == 0);
  CHECK(msgs[1].status == note_on);
  CHECK(msgs[1].note == 60);
  CHECK(msgs[1].timestamp == 0);
}

TEST_CASE("midi node: an overdue note-off is flushed, not lost", "[midi][node]")
{
  // If a tick is skipped over the end of a held note, in_range() can never
  // become true again. The release is emitted at the start of the next tick
  // instead of being dropped.
  fixture f;
  f.set({note(0, 50, 60)});

  auto first = f.tick(0, 10); // starts here, would end at 50
  REQUIRE(first.size() == 1);
  CHECK(first[0].status == note_on);

  auto msgs = f.tick(500, 600); // the end date is long past

  REQUIRE(msgs.size() == 1);
  CHECK(msgs[0].status == note_off);
  CHECK(msgs[0].note == 60);
  CHECK(msgs[0].timestamp == 0); // clamped to the beginning of the tick
}

TEST_CASE("midi node: end_discontinuous releases inside the tick", "[midi][node]")
{
  // Stamping at 0 puts the message before the start of the tick as soon as the
  // interval does not begin on a buffer boundary; consumers that window on
  // [tick_start; tick_start + frames[ then drop it, and the notes are cleared
  // here, so they would never be released again.
  fixture f;
  f.set({note(0, 1000, 60), note(0, 1000, 64)});

  REQUIRE(f.tick(0, 100).size() == 2);

  auto msgs = f.tick_discontinuous(100, 200, /* offset */ 32);

  REQUIRE(msgs.size() == 2);
  for(const auto& m : msgs)
  {
    CHECK(m.status == note_off);
    CHECK(m.timestamp == 32);
  }

  // and they are not released a second time
  CHECK(f.tick(200, 300).empty());
}

TEST_CASE("midi node: removing one note of a chord", "[midi][node]")
{
  // Notes are ordered on their start date alone, so erasing by key used to
  // remove every note of the chord, and find() could return a note of another
  // pitch - stopping the wrong one and leaving the edited one held forever.
  fixture f;
  f.set({note(0, 1000, 60), note(0, 1000, 64), note(0, 1000, 67)});

  REQUIRE(f.tick(0, 100).size() == 3);

  f.node.remove_note(note(0, 1000, 64));

  auto msgs = f.tick(100, 200);
  REQUIRE(msgs.size() == 1);
  CHECK(msgs[0].status == note_off);
  CHECK(msgs[0].note == 64);

  // the others are still held, and are released at their own end
  auto rest = f.tick(900, 1100);
  REQUIRE(rest.size() == 2);
  CHECK(rest[0].status == note_off);
  CHECK(rest[1].status == note_off);
  CHECK(std::min(rest[0].note, rest[1].note) == 60);
  CHECK(std::max(rest[0].note, rest[1].note) == 67);
}

TEST_CASE("midi node: mustStop releases everything held", "[midi][node]")
{
  fixture f;
  f.set({note(0, 10000, 60), note(0, 10000, 64)});

  REQUIRE(f.tick(0, 100).size() == 2);

  f.node.mustStop = true;
  auto msgs = f.tick(100, 200);

  REQUIRE(msgs.size() == 2);
  for(const auto& m : msgs)
    CHECK(m.status == note_off);
}

TEST_CASE("midi node: channel is stored zero-based", "[midi][node]")
{
  fixture f;
  f.node.set_channel(10); // 1-based in the model
  f.set({note(0, 50, 60)});

  auto msgs = f.tick(0, 100);

  REQUIRE(msgs.size() == 2);
  for(const auto& m : msgs)
    CHECK(m.channel == 9);
}

TEST_CASE("midi node: velocity survives the MIDI 2 round-trip", "[midi][node]")
{
  fixture f;
  f.set({note(0, 50, 60, 1), note(0, 50, 62, 64), note(0, 50, 64, 127)});

  auto msgs = f.tick(0, 10);

  REQUIRE(msgs.size() == 3);
  CHECK(msgs[0].velocity == 1);
  CHECK(msgs[1].velocity == 64);
  CHECK(msgs[2].velocity == 127);
}

TEST_CASE("midi node: a paused tick emits nothing", "[midi][node]")
{
  fixture f;
  f.set({note(0, 50, 60)});

  CHECK(f.tick(50, 50).empty());
}

TEST_CASE("midi node: replace_notes stops what was playing", "[midi][node]")
{
  fixture f;
  f.set({note(0, 10000, 60)});
  REQUIRE(f.tick(0, 100).size() == 1);

  ossia::nodes::midi::note_set fresh;
  fresh.insert(note(200, 100, 72));
  f.node.replace_notes(std::move(fresh));

  auto msgs = f.tick(100, 150);
  REQUIRE(msgs.size() == 1);
  CHECK(msgs[0].status == note_off);
  CHECK(msgs[0].note == 60);

  auto next = f.tick(150, 250);
  REQUIRE(next.size() == 1);
  CHECK(next[0].status == note_on);
  CHECK(next[0].note == 72);
}

TEST_CASE("midi node: update_note moves a note without stranding it", "[midi][node]")
{
  fixture f;
  f.set({note(100, 100, 60)});
  f.node.update_note(note(100, 100, 60), note(100, 100, 62));

  auto msgs = f.tick(0, 150);
  REQUIRE(msgs.size() == 1);
  CHECK(msgs[0].status == note_on);
  CHECK(msgs[0].note == 62);

  auto off = f.tick(150, 250);
  REQUIRE(off.size() == 1);
  CHECK(off[0].status == note_off);
  CHECK(off[0].note == 62);
}

TEST_CASE("midi node: add_note schedules a note landing on the current date", "[midi][node]")
{
  // m_prev_date is where the next tick starts, and the playing scan takes
  // start >= t.prev_date: a note landing exactly there is still to be played.
  // A strict comparison dropped it, and every note at date 0 with it.
  fixture f;
  f.node.add_note(note(0, 50, 60)); // m_prev_date is still 0

  auto msgs = f.tick(0, 100);

  REQUIRE(msgs.size() == 2);
  CHECK(msgs[0].status == note_on);
  CHECK(msgs[0].note == 60);
  CHECK(msgs[1].status == note_off);
}

TEST_CASE("midi node: a note added during playback plays on the next tick", "[midi][node]")
{
  fixture f;
  f.tick(0, 100); // m_prev_date becomes 100

  f.node.add_note(note(100, 50, 72)); // exactly on the boundary
  f.node.add_note(note(120, 50, 74));

  auto msgs = f.tick(100, 200);

  REQUIRE(msgs.size() == 4);
  CHECK(msgs[0].status == note_on);
  CHECK(msgs[0].note == 72);
  CHECK(msgs[0].timestamp == 0);
  CHECK(msgs[1].status == note_on);
  CHECK(msgs[1].note == 74);
  CHECK(msgs[1].timestamp == 20);
  CHECK(msgs[2].status == note_off);
  CHECK(msgs[2].note == 72);
  CHECK(msgs[2].timestamp == 50);
  CHECK(msgs[3].status == note_off);
  CHECK(msgs[3].note == 74);
  CHECK(msgs[3].timestamp == 70);
}

TEST_CASE("midi node: transport into the middle of a note resumes it", "[midi][node]")
{
  // The note-on scan only matches notes starting at or after the beginning of
  // a tick, so a note the playhead lands inside of can never be picked up by
  // it. It is entered from the start of the tick instead.
  fixture f;
  f.set({note(0, 1000, 60), note(2000, 100, 72)});

  f.node.transport_impl(ossia::time_value{500});
  auto msgs = f.tick(500, 600);

  REQUIRE(msgs.size() == 1);
  CHECK(msgs[0].status == note_on);
  CHECK(msgs[0].note == 60);
  CHECK(msgs[0].timestamp == 0);

  // and it ends at its own end date, not later
  auto off = f.tick(900, 1100);
  REQUIRE(off.size() == 1);
  CHECK(off[0].status == note_off);
  CHECK(off[0].note == 60);
  CHECK(off[0].timestamp == 100); // 1000 - 900
}

TEST_CASE("midi node: transporting releases what was playing", "[midi][node]")
{
  fixture f;
  f.set({note(0, 100, 60), note(5000, 100, 72)});

  REQUIRE(f.tick(0, 50).size() == 1);

  f.node.transport_impl(ossia::time_value{4000});
  auto msgs = f.tick(4000, 4100);

  REQUIRE(msgs.size() == 1);
  CHECK(msgs[0].status == note_off);
  CHECK(msgs[0].note == 60);
}

TEST_CASE("midi node: transporting back replays what follows", "[midi][node]")
{
  fixture f;
  f.set({note(100, 50, 60)});

  REQUIRE(f.tick(0, 200).size() == 2); // played and released

  f.node.transport_impl(ossia::time_value{0});
  auto msgs = f.tick(0, 200);

  REQUIRE(msgs.size() == 2);
  CHECK(msgs[0].status == note_on);
  CHECK(msgs[0].note == 60);
  CHECK(msgs[1].status == note_off);
}

TEST_CASE("midi node: rewinding past a note releases it", "[midi][node]")
{
  // Backward playback used to do nothing at all: the whole play/stop block was
  // behind t.forward(), so held notes stayed held.
  fixture f;
  f.set({note(100, 1000, 60)});

  REQUIRE(f.tick(0, 200).size() == 1); // note starts at 100

  auto msgs = f.tick(200, 50, 0, -1.); // rewind before its start

  REQUIRE(msgs.size() == 1);
  CHECK(msgs[0].status == note_off);
  CHECK(msgs[0].note == 60);
}

TEST_CASE("midi node: rewinding keeps a note the playhead is still inside", "[midi][node]")
{
  fixture f;
  f.set({note(100, 1000, 60)});

  REQUIRE(f.tick(0, 500).size() == 1);

  CHECK(f.tick(500, 200, 0, -1.).empty()); // still after its start

  // and it is released normally once time moves forward past its end
  auto off = f.tick(200, 1200);
  REQUIRE(off.size() == 1);
  CHECK(off[0].status == note_off);
}

TEST_CASE("midi node: rewinding over a note sounds it in reverse", "[midi][node]")
{
  // The mirror of forward playback: the end is crossed first, so the note is
  // entered there and left at its start. It sounds over the same span of the
  // timeline in both directions.
  fixture f;
  f.set({note(100, 50, 60)}); // sounds over [100; 150[

  REQUIRE(f.tick(0, 300).size() == 2); // played and released going forward

  auto back = f.tick(300, 0, 0, -1.);
  REQUIRE(back.size() == 2);
  CHECK(back[0].status == note_on);
  CHECK(back[0].note == 60);
  CHECK(back[0].timestamp == 150); // 300 -> 150 is 150 frames of rewinding
  CHECK(back[1].status == note_off);
  CHECK(back[1].note == 60);
  CHECK(back[1].timestamp == 200); // and 300 -> 100 is 200

  // and it is playable again going forward
  auto msgs = f.tick(0, 300);
  REQUIRE(msgs.size() == 2);
  CHECK(msgs[0].status == note_on);
  CHECK(msgs[1].status == note_off);
}

TEST_CASE("midi node: rewinding re-arms a note exactly once", "[midi][node]")
{
  // m_notes is a multiset: re-arming a note already in it would duplicate the
  // entry and produce two note-ons for one note.
  fixture f;
  f.set({note(100, 50, 60)});

  for(int pass = 0; pass < 3; pass++)
  {
    INFO("pass " << pass);

    auto forward = f.tick(0, 300);
    REQUIRE(forward.size() == 2);
    CHECK(forward[0].status == note_on);
    CHECK(forward[1].status == note_off);

    auto back = f.tick(300, 0, 0, -1.);
    REQUIRE(back.size() == 2);
    CHECK(back[0].status == note_on);
    CHECK(back[1].status == note_off);
  }
}

TEST_CASE("midi node: rewinding does not re-enter a note it is inside", "[midi][node]")
{
  // Only crossing the end enters a note. Landing inside one going backwards
  // leaves it silent, the same way it does going forwards.
  fixture f;
  f.set({note(0, 1000, 60)});

  CHECK(f.tick(900, 500, 0, -1.).empty());
}

TEST_CASE("midi node: transport() only applies after the next tick", "[midi][node]")
{
  // The request is deferred to the end of run(), so that a transport asked for
  // from another thread cannot rearrange the notes under a tick in flight.
  fixture f;
  f.set({note(0, 50, 60)});

  f.node.transport(ossia::time_value{1000});

  auto during = f.tick(0, 100); // still the old position
  REQUIRE(during.size() == 2);
  CHECK(during[0].status == note_on);

  CHECK(f.tick(1000, 1100).empty()); // nothing there
}
