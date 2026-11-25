// A node that started a note and goes away before stopping it (removed, or a
// cable taken off it, or its output bound elsewhere) leaves the note sounding
// at the other end. The graph stops it.

#include "../Editor/TestUtils.hpp"
#include "../Network/TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph/graph.hpp>
#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/dataflow/midi_note_tracker.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/protocols/midi/midi_stream.hpp>

#include <libremidi/libremidi.hpp>
#include <libremidi/ump_events.hpp>

#include "include_catch.hpp"

#include <chrono>

using namespace ossia;

namespace
{
using ump_list = std::vector<libremidi::ump>;

bool is_note_on(const libremidi::ump& m)
{
  return (m.get_status_code() & 0xF0) == 0x90;
}
bool is_note_off(const libremidi::ump& m)
{
  return (m.get_status_code() & 0xF0) == 0x80;
}
int pitch(const libremidi::ump& m)
{
  return (m.data[0] >> 8) & 0x7F;
}

// Plays what the test hands it, at the start of the tick it is asked for, and
// the way real nodes do: its output is written anew on each run, with what
// the earlier runs of the tick put in it.
struct source_node final : graph_node
{
  midi_outlet* out = new midi_outlet;
  ump_list to_send;
  //! What to play in the runs after the next one
  std::vector<ump_list> then_send;

  source_node() { m_outlets.push_back(out); }
  std::string label() const noexcept override { return "source"; }
  ump_list batch;
  void begin_execution() noexcept override { batch.clear(); }
  void run(const token_request& t, exec_state_facade e) noexcept override
  {
    for(auto m : to_send)
    {
      m.timestamp = t.physical_start(e.modelToSamples());
      batch.push_back(m);
    }
    out->data.messages.assign(batch.begin(), batch.end());
    to_send.clear();
    if(!then_send.empty())
    {
      to_send = then_send.front();
      then_send.erase(then_send.begin());
    }
  }
};

// A synth: remembers which notes it is playing
struct sink_node final : graph_node
{
  midi_inlet* in = new midi_inlet;
  ump_list received;
  midi_note_tracker sounding;

  sink_node() { m_inlets.push_back(in); }
  std::string label() const noexcept override { return "sink"; }
  void run(const token_request&, exec_state_facade) noexcept override
  {
    for(auto& m : in->data.messages)
    {
      received.push_back(m);
      sounding.track(m);
    }
  }
};

// Passes its input through, the way a MIDI effect does
struct thru_node final : graph_node
{
  midi_inlet* in = new midi_inlet;
  midi_outlet* out = new midi_outlet;

  thru_node()
  {
    m_inlets.push_back(in);
    m_outlets.push_back(out);
  }
  std::string label() const noexcept override { return "thru"; }
  void run(const token_request&, exec_state_facade) noexcept override
  {
    for(auto& m : in->data.messages)
      out->data.messages.push_back(m);
  }
};

struct fake_midi_protocol final
    : ossia::net::protocol_base
    , ossia::net::midi::midi_stream
{
  ump_list sent;
  //! Set for a device that is also read from
  std::unique_ptr<libremidi::midi_in> input;

  void open_input()
  {
    input = std::make_unique<libremidi::midi_in>(
        libremidi::input_configuration{.on_message = [](const libremidi::message&) {}},
        libremidi::dummy_configuration{});
  }

  bool pull(ossia::net::parameter_base&) override { return false; }
  bool push(const ossia::net::parameter_base&, const ossia::value&) override
  {
    return false;
  }
  bool push_raw(const ossia::net::full_parameter_data&) override { return false; }
  bool observe(ossia::net::parameter_base&, bool) override { return false; }
  bool update(ossia::net::node_base&) override { return false; }

  libremidi::midi_in* midi_in() const noexcept override { return input.get(); }
  void enable_registration() override { }
  void push_value(const libremidi::ump& m) override { sent.push_back(m); }
  void push_value(const libremidi::message&) override { }
};

struct Graph
{
  std::unique_ptr<ossia::tc_graph> gg = std::make_unique<ossia::tc_graph>();
  ossia::tc_graph& g = *gg;
  ossia::execution_state e;

  template <typename... Nodes>
  void tick(Nodes&... nodes)
  {
    (nodes->request(simple_token_request{0_tv, 1_tv}), ...);
    e.begin_tick();
    g.state(e);
    e.commit();
  }

  void connect(
      const std::shared_ptr<graph_node>& a, outlet* out,
      const std::shared_ptr<graph_node>& b, inlet* in,
      connection c = immediate_glutton_connection{})
  {
    g.connect(g.allocate_edge(c, out, in, a, b));
  }

  // What score's execution does when a process is deleted
  void remove(const std::shared_ptr<graph_node>& n)
  {
    for_each_inlet(*n, [&](auto& p) { e.unregister_port(p); });
    for_each_outlet(*n, [&](auto& p) { e.unregister_port(p); });
    g.remove_node(n);
    n->clear();
  }
};

using libremidi::from_midi1::control_change;
using libremidi::from_midi1::note_off;
using libremidi::from_midi1::note_on;
}

TEST_CASE("A removed node's notes are stopped", "[dataflow][midi][release]")
{
  Graph gr;
  auto src = std::make_shared<source_node>();
  auto snk = std::make_shared<sink_node>();
  gr.g.add_node(src);
  gr.g.add_node(snk);
  gr.connect(src, src->out, snk, snk->in);

  GIVEN("A source that started three notes and stopped one")
  {
    src->to_send = {note_on(0, 60, 100), note_on(0, 64, 100), note_on(9, 36, 100)};
    gr.tick(src, snk);
    src->to_send = {note_off(0, 64, 0)};
    gr.tick(src, snk);

    REQUIRE(snk->sounding.count() == 2);
    REQUIRE(src->out->held.count() == 2);

    WHEN("The source is removed")
    {
      gr.remove(src);
      const auto before = snk->received.size();
      gr.tick(snk);

      THEN("The sink gets a note off for each of the two notes left, and only those")
      {
        REQUIRE(snk->received.size() == before + 2);
        CHECK(is_note_off(snk->received[before]));
        CHECK(is_note_off(snk->received[before + 1]));
        CHECK(snk->received[before].timestamp == 0);
        CHECK(snk->sounding.empty());
      }

      AND_WHEN("More ticks go by")
      {
        gr.tick(snk);
        const auto after = snk->received.size();
        gr.tick(snk);
        THEN("Nothing is sent a second time")
        {
          CHECK(snk->received.size() == after);
        }
      }
    }

    WHEN("The sink is not running when the source is removed")
    {
      gr.remove(src);
      gr.tick();
      gr.tick();
      CHECK(snk->sounding.count() == 2);

      THEN("It gets the note offs when it runs again")
      {
        gr.tick(snk);
        CHECK(snk->sounding.empty());
      }
    }

    WHEN("Only the cable is removed")
    {
      auto edge = src->out->targets.front();
      gr.g.disconnect(edge);
      gr.tick(src, snk);

      THEN("The sink's notes are stopped")
      {
        CHECK(snk->sounding.empty());
      }
      THEN("The source still knows it holds them, for its other cables")
      {
        CHECK(src->out->held.count() == 2);
      }
    }
  }

  GIVEN("A source that stopped everything it started")
  {
    src->to_send = {note_on(0, 60, 100), note_on(0, 64, 100)};
    gr.tick(src, snk);
    src->to_send = {note_off(0, 60, 0), note_off(0, 64, 0)};
    gr.tick(src, snk);

    WHEN("It is removed")
    {
      gr.remove(src);
      const auto before = snk->received.size();
      gr.tick(snk);
      THEN("Nothing is sent")
      {
        CHECK(snk->received.size() == before);
      }
    }
  }

  GIVEN("A source holding the damper pedal down")
  {
    src->to_send = {control_change(0, 64, 127), note_on(0, 60, 100), note_off(0, 60, 0)};
    gr.tick(src, snk);

    WHEN("It is removed")
    {
      gr.remove(src);
      gr.tick(snk);
      THEN("The pedal is lifted at the sink")
      {
        CHECK(snk->sounding.empty());
        CHECK(snk->sounding.damper == 0);
      }
    }
  }
}

TEST_CASE("Releases reach the sink along with its other inputs", "[dataflow][midi][release]")
{
  Graph gr;
  auto a = std::make_shared<source_node>();
  auto b = std::make_shared<source_node>();
  auto snk = std::make_shared<sink_node>();
  gr.g.add_node(a);
  gr.g.add_node(b);
  gr.g.add_node(snk);
  gr.connect(a, a->out, snk, snk->in);
  gr.connect(b, b->out, snk, snk->in);

  a->to_send = {note_on(0, 60, 100)};
  b->to_send = {note_on(0, 72, 100)};
  gr.tick(a, b, snk);
  REQUIRE(snk->sounding.count() == 2);

  WHEN("One of two sources is removed while the other one plays on")
  {
    // The sink is left with a single cable: its data is then swapped in
    // rather than appended.
    gr.remove(a);
    b->to_send = {note_on(0, 74, 100)};
    const auto before = snk->received.size();
    gr.tick(b, snk);

    THEN("The sink gets the note off of the first and the new note of the second")
    {
      REQUIRE(snk->received.size() == before + 2);
      CHECK(is_note_off(snk->received[before]));
      CHECK(pitch(snk->received[before]) == 60);
      CHECK(is_note_on(snk->received[before + 1]));
      CHECK(pitch(snk->received[before + 1]) == 74);
      CHECK(snk->sounding.count() == 2);
    }
  }
}

TEST_CASE("Releases go through the nodes downstream", "[dataflow][midi][release]")
{
  Graph gr;
  auto src = std::make_shared<source_node>();
  auto thru = std::make_shared<thru_node>();
  auto snk = std::make_shared<sink_node>();
  gr.g.add_node(src);
  gr.g.add_node(thru);
  gr.g.add_node(snk);
  gr.connect(src, src->out, thru, thru->in);
  gr.connect(thru, thru->out, snk, snk->in);

  src->to_send = {note_on(0, 60, 100)};
  gr.tick(src, thru, snk);
  REQUIRE(snk->sounding.count() == 1);

  WHEN("The first node of a chain is removed")
  {
    gr.remove(src);
    gr.tick(thru, snk);
    THEN("The effect after it passes the note off on")
    {
      CHECK(snk->sounding.empty());
      CHECK(thru->out->held.empty());
    }
  }

  WHEN("The middle node of a chain is removed")
  {
    gr.remove(thru);
    gr.tick(src, snk);
    THEN("The sink is told to stop what the effect had passed on")
    {
      CHECK(snk->sounding.empty());
    }
  }
}

TEST_CASE("Notes sent to a device are stopped there", "[dataflow][midi][release]")
{
  auto proto_ptr = std::make_unique<fake_midi_protocol>();
  auto& proto = *proto_ptr;
  ossia::net::generic_device dev{std::move(proto_ptr), "midi"};

  Graph gr;
  gr.e.register_device(&dev);
  gr.e.apply_device_changes();

  auto src = std::make_shared<source_node>();
  src->out->address = &dev.get_root_node();
  gr.e.register_port(*src->out);
  gr.g.add_node(src);

  src->to_send = {note_on(2, 60, 100), note_on(2, 67, 100)};
  gr.tick(src);
  src->to_send = {note_off(2, 67, 0)};
  gr.tick(src);
  REQUIRE(proto.sent.size() == 3);

  WHEN("The node is removed")
  {
    gr.remove(src);
    THEN("The device gets the note off of the note still held")
    {
      REQUIRE(proto.sent.size() == 4);
      CHECK(is_note_off(proto.sent[3]));
      CHECK(pitch(proto.sent[3]) == 60);
      CHECK(proto.sent[3].get_channel_0_15() == 2);
    }
  }

  WHEN("The outlet is bound elsewhere")
  {
    gr.e.unregister_port(*src->out);
    src->out->address = {};
    THEN("The device it leaves gets the note off")
    {
      REQUIRE(proto.sent.size() == 4);
      CHECK(is_note_off(proto.sent[3]));
      CHECK(pitch(proto.sent[3]) == 60);
    }
  }

  WHEN("The graph is no longer run, as when the score is paused")
  {
    ossia::release_held_notes(gr.g, gr.e);
    THEN("The device gets the note off")
    {
      REQUIRE(proto.sent.size() == 4);
      CHECK(is_note_off(proto.sent[3]));
      CHECK(pitch(proto.sent[3]) == 60);
    }
    AND_WHEN("It is run again and the node ends its note")
    {
      src->to_send = {note_off(2, 60, 0)};
      gr.tick(src);
      THEN("The outlet holds nothing")
      {
        CHECK(src->out->held.empty());
      }
    }
  }

  WHEN("The device goes away first")
  {
    gr.e.unregister_device(&dev);
    gr.e.apply_device_changes();
    THEN("Removing the node does not reach for it")
    {
      gr.remove(src);
      CHECK(proto.sent.size() == 3);
    }
  }

  gr.e.unregister_device(&dev);
  gr.e.apply_device_changes();
}

// The other way around: a key held on a keyboard when the inlet reading the
// keyboard is pointed elsewhere. Its note off goes to a device nothing reads
// any more, and the synth behind the inlet would play the note forever.
TEST_CASE(
    "Notes read from a device are stopped when the inlet leaves it",
    "[dataflow][midi][release]")
{
  auto make_device = [](const char* name) {
    auto proto = std::make_unique<fake_midi_protocol>();
    proto->open_input();
    auto ptr = proto.get();
    return std::pair{
        std::make_unique<ossia::net::generic_device>(std::move(proto), name), ptr};
  };
  auto [dev, proto] = make_device("keyboard");
  auto [other_dev, other_proto] = make_device("other");

  Graph gr;
  gr.e.register_device(dev.get());
  gr.e.register_device(other_dev.get());
  gr.e.apply_device_changes();

  auto synth = std::make_shared<sink_node>();
  synth->in->address = &dev->get_root_node();
  gr.e.register_port(*synth->in);
  gr.g.add_node(synth);

  proto->receive_ump(note_on(2, 60, 100));
  proto->receive_ump(note_on(2, 67, 100));
  gr.tick(synth);
  proto->receive_ump(note_off(2, 67, 0));
  gr.tick(synth);
  REQUIRE(synth->received.size() == 3);
  REQUIRE(synth->sounding.count() == 1);

  WHEN("The inlet is bound to nothing")
  {
    gr.e.unregister_port(*synth->in);
    synth->in->address = {};
    gr.tick(synth);
    THEN("The synth gets the note off of the key still held")
    {
      REQUIRE(synth->received.size() == 4);
      CHECK(is_note_off(synth->received[3]));
      CHECK(pitch(synth->received[3]) == 60);
      CHECK(synth->received[3].get_channel_0_15() == 2);
      CHECK(synth->sounding.empty());
    }
    AND_WHEN("It runs again")
    {
      gr.tick(synth);
      THEN("It is not stopped twice")
      {
        CHECK(synth->received.size() == 4);
      }
    }
  }

  WHEN("The inlet is bound to another device")
  {
    gr.e.unregister_port(*synth->in);
    synth->in->address = &other_dev->get_root_node();
    gr.e.register_port(*synth->in);

    other_proto->receive_ump(note_on(2, 62, 100));
    // Let go of on the first keyboard, which the synth no longer hears
    proto->receive_ump(note_off(2, 60, 0));
    gr.tick(synth);
    THEN("The first device's note is stopped, before the second one's starts")
    {
      REQUIRE(synth->received.size() == 5);
      CHECK(is_note_off(synth->received[3]));
      CHECK(pitch(synth->received[3]) == 60);
      CHECK(is_note_on(synth->received[4]));
      CHECK(pitch(synth->received[4]) == 62);
      CHECK(synth->sounding.count() == 1);
    }
    AND_WHEN("It leaves the second one")
    {
      gr.e.unregister_port(*synth->in);
      synth->in->address = {};
      gr.tick(synth);
      THEN("That one's note is stopped too")
      {
        REQUIRE(synth->received.size() == 6);
        CHECK(pitch(synth->received[5]) == 62);
        CHECK(synth->sounding.empty());
      }
    }
  }

  WHEN("The pedal is down and the keys are up")
  {
    proto->receive_ump(control_change(2, 64, 127));
    proto->receive_ump(note_off(2, 60, 0));
    gr.tick(synth);
    gr.e.unregister_port(*synth->in);
    synth->in->address = {};
    gr.tick(synth);
    THEN("The pedal is lifted")
    {
      CHECK(synth->sounding.damper == 0);
      CHECK(synth->sounding.empty());
    }
  }

  WHEN("The device goes away")
  {
    gr.e.unregister_device(dev.get());
    gr.e.apply_device_changes();
    synth->in->address = {};
    gr.tick(synth);
    THEN("The synth gets the note off")
    {
      REQUIRE(synth->received.size() == 4);
      CHECK(is_note_off(synth->received[3]));
      CHECK(pitch(synth->received[3]) == 60);
    }
    AND_WHEN("The node is removed")
    {
      gr.remove(synth);
      THEN("Nothing more happens")
      {
        CHECK(synth->received.size() == 4);
      }
    }
  }

  WHEN("The node is removed")
  {
    gr.remove(synth);
    THEN("It is left alone")
    {
      CHECK(synth->received.size() == 3);
    }
  }

  gr.e.unregister_device(dev.get());
  gr.e.unregister_device(other_dev.get());
  gr.e.apply_device_changes();
}

TEST_CASE("A stopped process's notes are stopped", "[dataflow][midi][release]")
{
  auto proto_ptr = std::make_unique<fake_midi_protocol>();
  auto& proto = *proto_ptr;
  ossia::net::generic_device dev{std::move(proto_ptr), "midi"};

  Graph gr;
  gr.e.register_device(&dev);
  gr.e.apply_device_changes();

  // One outlet to a synth through a cable, another one to a device
  auto src = std::make_shared<source_node>();
  auto dev_src = std::make_shared<source_node>();
  auto snk = std::make_shared<sink_node>();
  dev_src->out->address = &dev.get_root_node();
  gr.e.register_port(*dev_src->out);
  gr.g.add_node(src);
  gr.g.add_node(dev_src);
  gr.g.add_node(snk);
  gr.connect(src, src->out, snk, snk->in);

  src->to_send = {note_on(0, 60, 100)};
  dev_src->to_send = {note_on(0, 48, 100)};
  gr.tick(src, dev_src, snk);
  REQUIRE(snk->sounding.count() == 1);
  REQUIRE(proto.sent.size() == 1);

  WHEN("The process stops after its last tick, in which it starts one more note")
  {
    // What an interval does when it ends inside a tick: the process is asked
    // for the tick's last slice, then stopped; the graph runs afterwards.
    src->to_send = {note_on(0, 64, 100)};
    src->request(simple_token_request{1_tv, 2_tv});
    src->request_note_release();
    dev_src->request(simple_token_request{1_tv, 2_tv});
    dev_src->request_note_release();
    snk->request(simple_token_request{1_tv, 2_tv});
    gr.e.begin_tick();
    gr.g.state(gr.e);
    gr.e.commit();

    THEN("The device is told to stop")
    {
      REQUIRE(proto.sent.size() == 2);
      CHECK(is_note_off(proto.sent[1]));
      CHECK(pitch(proto.sent[1]) == 48);
    }

    THEN("The sink plays the last note, and is told to stop both the tick after")
    {
      CHECK(snk->sounding.count() == 2);
      gr.tick(snk);
      CHECK(snk->sounding.empty());
      CHECK(src->out->held.empty());
    }

    AND_WHEN("More ticks go by")
    {
      gr.tick(snk);
      const auto after = snk->received.size();
      gr.tick(snk);
      gr.tick(src, dev_src, snk);
      THEN("Nothing is stopped a second time")
      {
        CHECK(snk->received.size() == after);
        CHECK(proto.sent.size() == 2);
      }
    }
  }

  WHEN("The process stops and starts again within a tick, as in a loop")
  {
    src->to_send = {note_on(0, 64, 100)};
    src->then_send = {{note_on(0, 67, 100)}};
    src->request(simple_token_request{1_tv, 2_tv});
    src->request_note_release();
    // The new run starts ten frames into the tick.
    src->request(simple_token_request{0_tv, 1_tv, 0_tv, 10_tv});
    snk->request(simple_token_request{1_tv, 2_tv});
    const auto before = snk->received.size();
    gr.e.begin_tick();
    gr.g.state(gr.e);
    gr.e.commit();

    THEN("What was played before the stop is stopped, what is played after is not")
    {
      REQUIRE(snk->received.size() == before + 4);
      CHECK(is_note_on(snk->received[before]));
      CHECK(pitch(snk->received[before]) == 64);
      CHECK(is_note_off(snk->received[before + 1]));
      CHECK(snk->received[before + 1].timestamp == 10);
      CHECK(is_note_off(snk->received[before + 2]));
      CHECK(is_note_on(snk->received[before + 3]));
      CHECK(pitch(snk->received[before + 3]) == 67);

      CHECK(snk->sounding.count() == 1);
      CHECK(src->out->held.count() == 1);

      gr.tick(src, snk);
      gr.tick(src, snk);
      CHECK(snk->sounding.count() == 1);
    }
  }

  gr.e.unregister_device(&dev);
  gr.e.apply_device_changes();
}

TEST_CASE("A process that jumps or is muted has its notes stopped", "[dataflow][midi][release]")
{
  Graph gr;
  auto src = std::make_shared<source_node>();
  auto snk = std::make_shared<sink_node>();
  gr.g.add_node(src);
  gr.g.add_node(snk);
  gr.connect(src, src->out, snk, snk->in);
  ossia::node_process proc{src};

  src->to_send = {note_on(0, 60, 100), note_on(1, 62, 100)};
  gr.tick(src, snk);
  REQUIRE(snk->sounding.count() == 2);

  WHEN("The process is moved elsewhere on the timeline while it plays")
  {
    // A seek: the jump comes first, then the tick from the new position, in
    // which the process starts a note.
    proc.transport(100_tv);
    src->to_send = {note_on(0, 72, 100)};
    const auto before = snk->received.size();
    gr.tick(src, snk);

    THEN("What it held is stopped before what it plays from there")
    {
      REQUIRE(snk->received.size() == before + 3);
      CHECK(is_note_off(snk->received[before]));
      CHECK(is_note_off(snk->received[before + 1]));
      CHECK(is_note_on(snk->received[before + 2]));
      CHECK(pitch(snk->received[before + 2]) == 72);
      CHECK(snk->sounding.count() == 1);

      gr.tick(src, snk);
      CHECK(snk->sounding.count() == 1);
    }
  }

  WHEN("The process is given an offset and does not play in that tick")
  {
    proc.offset(100_tv);
    gr.tick(snk);
    gr.tick(snk);
    THEN("What it held is stopped")
    {
      CHECK(snk->sounding.empty());
    }
  }

  WHEN("The process is muted")
  {
    proc.mute(true);
    gr.tick(src, snk);
    gr.tick(src, snk);
    THEN("What it held is stopped")
    {
      CHECK(snk->sounding.empty());
    }

    AND_WHEN("It is unmuted and plays again")
    {
      proc.mute(false);
      src->to_send = {note_on(0, 65, 100)};
      gr.tick(src, snk);
      THEN("Its new notes are left alone")
      {
        CHECK(snk->sounding.count() == 1);
        gr.tick(src, snk);
        CHECK(snk->sounding.count() == 1);
      }
    }
  }
}

TEST_CASE("Tracking held notes", "[dataflow][midi][release]")
{
  midi_note_tracker t;
  ump_list out;

  SECTION("MIDI 1 messages are stopped by MIDI 1 messages")
  {
    libremidi::ump on;
    on.data[0] = cmidi2_ump_midi1_note_on(5, 9, 36, 100);
    t.track(on);
    t.release(out);
    REQUIRE(out.size() == 1);
    CHECK(out[0].data[0] == uint32_t(cmidi2_ump_midi1_note_off(5, 9, 36, 0)));

    AND_THEN("A MIDI 1 note on of velocity zero is a note off")
    {
      on.data[0] = cmidi2_ump_midi1_note_on(5, 9, 36, 0);
      t.track(on);
      CHECK(t.empty());
    }
  }

  SECTION("The group and channel of a MIDI 2 note are kept")
  {
    t.track(libremidi::ump_events::note_on(3, 1, 61, 0xFFFF));
    t.release(out);
    REQUIRE(out.size() == 1);
    CHECK(out[0].get_type() == libremidi::midi2::message_type::MIDI_2_CHANNEL);
    CHECK(out[0].get_group() == 3);
    CHECK(out[0].get_channel_0_15() == 1);
    CHECK(is_note_off(out[0]));
    CHECK(pitch(out[0]) == 61);
  }

  SECTION("Every channel and pitch has its own bit")
  {
    for(int c = 0; c < 16; c++)
      for(int p = 0; p < 128; p++)
        t.track(note_on(c, p, 1));
    CHECK(t.count() == 16 * 128);

    t.release(out);
    REQUIRE(out.size() == 16 * 128);
    for(auto& m : out)
      t.track(m);
    CHECK(t.empty());
  }

  SECTION("It stays small")
  {
    CHECK(sizeof(midi_note_tracker) <= 264);
  }
}

// Not a pass/fail matter: prints what tracking costs next to a tick.
TEST_CASE("Cost of tracking in a tick", "[dataflow][midi][release][!benchmark]")
{
  Graph gr;
  auto src = std::make_shared<source_node>();
  auto thru = std::make_shared<thru_node>();
  auto snk = std::make_shared<sink_node>();
  gr.g.add_node(src);
  gr.g.add_node(thru);
  gr.g.add_node(snk);
  gr.connect(src, src->out, thru, thru->in);
  gr.connect(thru, thru->out, snk, snk->in);

  for(int notes : {0, 8, 64})
  {
    ump_list chord;
    for(int i = 0; i < notes; i++)
      chord.push_back(i % 2 ? note_off(0, 40 + i / 2, 0) : note_on(0, 40 + i / 2, 100));

    const int N = 200000;
    double best_tick = 1e30, best_track = 1e30;
    for(int rep = 0; rep < 5; rep++)
    {
      auto t0 = std::chrono::steady_clock::now();
      for(int i = 0; i < N; i++)
      {
        src->to_send = chord;
        snk->received.clear();
        gr.tick(src, thru, snk);
      }
      auto t1 = std::chrono::steady_clock::now();
      best_tick
          = std::min(best_tick, std::chrono::duration<double, std::nano>(t1 - t0).count() / N);

      // The same work the two outlets of the chain do after their node ran
      midi_outlet o1, o2;
      for(auto& m : chord)
      {
        o1.data.messages.push_back(m);
        o2.data.messages.push_back(m);
      }
      t0 = std::chrono::steady_clock::now();
      for(int i = 0; i < N; i++)
      {
        static_cast<outlet&>(o1).post_process();
        static_cast<outlet&>(o2).post_process();
      }
      t1 = std::chrono::steady_clock::now();
      best_track = std::min(
          best_track, std::chrono::duration<double, std::nano>(t1 - t0).count() / N);
    }
    std::printf(
        "3-node MIDI chain, %2d messages/tick: tick %8.1f ns, of which tracking %6.1f ns "
        "(%.2f%%)\n",
        notes, best_tick, best_track, 100. * best_track / best_tick);
  }
}
