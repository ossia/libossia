// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// Headless test of the Max-independent part of ossia.v8connector
// (src/ossia-max/src/v8connector_branch.cpp).
//
// The device is built the way ossia-max builds it (generic_device over a
// multiplex protocol, oscquery_asio server on a network context). A parameter
// is created by hand, as an ossia.parameter does; two branches with the same
// name are created, as two ossia.v8connector instances do; the tree is then
// read back through an OSCQuery mirror client.

#include "OSCQueryBundleTestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/domain/domain_functions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/value/vec.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

#include <spdlog/sinks/callback_sink.h>

#include <v8connector_branch.hpp>

#include <chrono>
#include <set>
#include <string>
#include <thread>

using namespace ossia::max_binding::v8connector_detail;

namespace
{
// Drain a branch after a refused call: the queue holds exactly that refusal.
void expect_refused(branch& b, const char* address)
{
  auto in = b.drain();
  REQUIRE(in.size() == 1);
  REQUIRE(in[0].address == "refused");
  REQUIRE(
      in[0].value.get<std::vector<ossia::value>>()[0]
      == ossia::value(std::string(address)));
}

// The OSCQuery mirror of a client's device, set up as ossia.client sets up a
// connection by URL: a path the server removes is removed, not kept as a
// zombie. The tree changes the server sends wait in the mirror's command
// queue until run_commands is called, which the client does when it polls.
ossia::oscquery::oscquery_mirror_protocol* polled_mirror(ossia::net::device_base& dev)
{
  auto* mirror
      = dynamic_cast<ossia::oscquery::oscquery_mirror_protocol*>(&dev.get_protocol());
  if(mirror)
    mirror->set_zombie_on_remove(false);
  return mirror;
}

std::vector<attribute> attrs(std::initializer_list<std::pair<const char*, scalars>> l)
{
  std::vector<attribute> out;
  for(auto& [n, v] : l)
    out.push_back({n, v});
  return out;
}

leaf_spec spec_of(const char* address, std::vector<attribute> a)
{
  leaf_spec spec;
  auto err = make_leaf_spec(address, a, spec);
  REQUIRE(err.empty());
  return spec;
}

std::string S(const char* s)
{
  return s;
}

}

TEST_CASE("v8connector_declarations", "v8connector_declarations")
{
  leaf_spec spec;

  SECTION("numeric types require min and max")
  {
    auto err = make_leaf_spec("cutoff", attrs({{"type", {S("float")}}}), spec);
    REQUIRE(!err.empty());
    REQUIRE(err.find("@min and @max") != std::string::npos);

    REQUIRE(make_leaf_spec(
                "cutoff",
                attrs({{"type", {S("float")}}, {"min", {20}}, {"max", {20000}}}), spec)
                .empty());
    REQUIRE(make_leaf_spec(
                "cutoff", attrs({{"type", {S("float")}}, {"range", {20, 20000}}}), spec)
                .empty());
    REQUIRE(spec.min == std::vector<float>{20.f});
    REQUIRE(spec.max == std::vector<float>{20000.f});

    REQUIRE(
        make_leaf_spec("pos", attrs({{"type", {S("vec2f")}}, {"range", {-1, 1}}}), spec)
            .empty());
    REQUIRE(spec.min == std::vector<float>{-1.f, -1.f});
    REQUIRE(spec.max == std::vector<float>{1.f, 1.f});

    REQUIRE(!make_leaf_spec(
                 "pos", attrs({{"type", {S("vec2f")}}, {"range", {-1, 1, 0}}}), spec)
                 .empty());
    REQUIRE(!make_leaf_spec(
                 "x", attrs({{"type", {S("float")}}, {"min", {1}}, {"max", {0}}}), spec)
                 .empty());
    REQUIRE(
        !make_leaf_spec(
             "x", attrs({{"type", {S("float")}}, {"range", {0, 1}}, {"min", {0}}}), spec)
             .empty());
  }

  SECTION("non numeric types take no range, strings take an enumeration")
  {
    REQUIRE(make_leaf_spec("s", attrs({{"type", {S("string")}}}), spec).empty());
    REQUIRE(make_leaf_spec(
                "s", attrs({{"type", {S("symbol")}}, {"range", {S("a"), S("b")}}}), spec)
                .empty());
    REQUIRE(spec.type == "string");
    REQUIRE(spec.enumeration == std::vector<std::string>{"a", "b"});
    REQUIRE(!make_leaf_spec(
                 "b", attrs({{"type", {S("bool")}}, {"min", {0}}, {"max", {1}}}), spec)
                 .empty());
    REQUIRE(
        make_leaf_spec("i", attrs({{"type", {S("impulse")}}, {"critical", {1}}}), spec)
            .empty());
    REQUIRE(spec.critical);
  }

  SECTION("unknown things are refused")
  {
    REQUIRE(!make_leaf_spec("x", attrs({{"min", {0}}, {"max", {1}}}), spec).empty());
    REQUIRE(!make_leaf_spec(
                 "x", attrs({{"type", {S("flt")}}, {"min", {0}}, {"max", {1}}}), spec)
                 .empty());
    REQUIRE(!make_leaf_spec(
                 "x",
                 attrs(
                     {{"type", {S("float")}},
                      {"min", {0}},
                      {"max", {1}},
                      {"colour", {S("red")}}}),
                 spec)
                 .empty());
    REQUIRE(!make_leaf_spec(
                 "x",
                 attrs(
                     {{"type", {S("float")}},
                      {"min", {0}},
                      {"max", {1}},
                      {"unit", {S("gain.decibelz")}}}),
                 spec)
                 .empty());
    REQUIRE(!make_leaf_spec(
                 "x",
                 attrs(
                     {{"type", {S("float")}},
                      {"min", {0}},
                      {"max", {1}},
                      {"clip", {S("sometimes")}}}),
                 spec)
                 .empty());
    REQUIRE(!make_leaf_spec(
                 "", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}}), spec)
                 .empty());
    REQUIRE(
        !make_leaf_spec(
             "a//b", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}}), spec)
             .empty());
  }

  SECTION("addresses are normalized")
  {
    REQUIRE(normalize_relative_address("/gain/") == "gain");
    REQUIRE(normalize_relative_address(" engine/seed ") == "engine/seed");
    REQUIRE(normalize_relative_address("dev:/x").empty());
  }
}

TEST_CASE("v8connector_tree", "v8connector_tree")
{
  auto ctx = ossia::net::create_network_context();
  auto proto = std::make_unique<ossia::net::multiplex_protocol>();
  auto* multiplex = proto.get();
  ossia::net::generic_device dev{std::move(proto), "v8_headless"};
  auto& root = dev.get_root_node();

  // A parameter created by hand, as ossia.parameter does
  {
    auto& n = ossia::net::create_node(root, "/manual/gain");
    auto* p = ossia::try_setup_parameter("float", n);
    p->set_domain(ossia::make_domain(0.f, 1.f));
    p->push_value(0.25f);
  }

  // Two branch nodes with the same name, as two instances do
  auto& bn1 = ossia::net::create_node(root, "/synth");
  auto& bn2 = ossia::net::create_node(root, "/synth");
  REQUIRE(bn1.osc_address() == "/synth");
  REQUIRE(bn2.osc_address() == "/synth.1");

  branch b1, b2;
  int lost = 0;
  b1.set_node_lost_callback([&] { ++lost; });
  b1.attach(bn1);
  b2.attach(bn2);

  const auto gain = spec_of(
      "gain", attrs(
                  {{"type", {S("float")}},
                   {"min", {-60}},
                   {"max", {6}},
                   {"unit", {S("gain.db")}},
                   {"default", {-12}},
                   {"description", {S("main gain")}},
                   {"clip", {S("both")}}}));
  REQUIRE(b1.add(gain).empty());
  REQUIRE(b2.add(gain).empty());
  REQUIRE(b1.add(spec_of(
                     "cutoff", attrs(
                                   {{"type", {S("float")}},
                                    {"range", {20, 20000}},
                                    {"unit", {S("time.hz")}}})))
              .empty());
  REQUIRE(b1.add(spec_of(
                     "voices", attrs(
                                   {{"type", {S("int")}},
                                    {"min", {1}},
                                    {"max", {16}},
                                    {"default", {4}}})))
              .empty());
  REQUIRE(b1.add(spec_of("pos", attrs({{"type", {S("vec2f")}}, {"range", {-1, 1}}})))
              .empty());
  REQUIRE(b1.add(spec_of(
                     "mode", attrs(
                                 {{"type", {S("string")}},
                                  {"range", {S("sine"), S("saw")}},
                                  {"default", {S("sine")}}})))
              .empty());
  REQUIRE(
      b1.add(spec_of(
                 "engine/reset", attrs({{"type", {S("impulse")}}, {"critical", {1}}})))
          .empty());
  REQUIRE(
      b1.add(gain).empty()); // the same declaration again is accepted and does nothing
  REQUIRE(b1.size() == 6);
  {
    auto other = gain;
    other.max = {12.f};
    REQUIRE(!b1.add(other).empty()); // other attributes at a declared address: refused
  }

  SECTION("the default of a leaf comes out of its own branch once it is created")
  {
    // as ossia-max outputs the defaults of its parameters after they registered
    auto in = b1.drain();
    REQUIRE(in.size() == 4); // the three defaults, then the refusal of the second gain
    REQUIRE(in[3].address == "refused");
    in.pop_back();
    REQUIRE(in[0].address == "gain");
    REQUIRE(in[0].value == ossia::value(-12.f));
    REQUIRE(in[1].address == "voices");
    REQUIRE(in[1].value == ossia::value(4));
    REQUIRE(in[2].address == "mode");
    REQUIRE(in[2].value == ossia::value(std::string("sine")));
    REQUIRE(b1.drain().empty());
  }

  auto* gain1 = ossia::net::find_node(root, "/synth/gain")->get_parameter();
  auto* gain2 = ossia::net::find_node(root, "/synth.1/gain")->get_parameter();
  REQUIRE(gain1);
  REQUIRE(gain2);

  SECTION("set: no echo to the pushing branch, clipping, validation")
  {
    int other_listener = 0;
    gain1->add_callback([&](const ossia::value&) { ++other_listener; });
    b1.drain();

    REQUIRE(b1.set("gain", {-6}).empty());
    REQUIRE(b1.drain().empty());
    REQUIRE(other_listener == 1);
    REQUIRE(gain1->value() == ossia::value(-6.f));

    REQUIRE(b1.set("gain", {100}).empty());
    // stored as sent, as ossia.parameter does; bounded when it leaves
    REQUIRE(gain1->value() == ossia::value(100.f));
    // the value that leaves differs from the value sent: it comes back to the
    // instance that sent it, as if it came from another source
    auto corrected = b1.drain();
    REQUIRE(corrected.size() == 1);
    REQUIRE(corrected[0].address == "gain");
    REQUIRE(corrected[0].value == ossia::value(6.f));
    REQUIRE(b1.set("gain", {-116}).empty());
    corrected = b1.drain();
    REQUIRE(corrected.size() == 1);
    REQUIRE(corrected[0].value == ossia::value(-60.f));
    REQUIRE(b1.set("gain", {-6}).empty()); // in range: nothing comes back
    REQUIRE(b1.drain().empty());
    REQUIRE(b1.set("voices", {3.7f})
                .empty()); // converted to int: the effective value comes back
    corrected = b1.drain();
    REQUIRE(corrected.size() == 1);
    REQUIRE(corrected[0].address == "voices");
    REQUIRE(corrected[0].value == ossia::value(4));
    REQUIRE(b1.set("voices", {4}).empty());
    REQUIRE(b1.drain().empty());

    REQUIRE(!b1.set("nothere", {1}).empty());
    expect_refused(b1, "nothere");
    REQUIRE(!b1.set("pos", {1}).empty());
    expect_refused(b1, "pos");
    REQUIRE(b1.set("pos", {0.5f, -0.5f}).empty());

    int wakes = 0;
    b1.set_wake_callback([&] { ++wakes; });
    gain1->push_value(0.5f);
    auto in = b1.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "gain");
    REQUIRE(in[0].value == ossia::value(0.5f));
    REQUIRE(wakes == 1);

    // another source pushes out of range straight on the parameter, as an
    // ossia.remote does: the parameter stores it, the branch outputs it bounded
    gain1->push_value(-116.f);
    REQUIRE(gain1->value() == ossia::value(-116.f));
    in = b1.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].value == ossia::value(-60.f));
    auto* voices1 = ossia::net::find_node(root, "/synth/voices")->get_parameter();
    voices1->set_bounding(ossia::bounding_mode::CLIP);
    voices1->push_value(40);
    in = b1.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "voices");
    REQUIRE(in[0].value == ossia::value(16));
  }

  SECTION(
      "the declared leaves are served by the oscquery server next to the hand made "
      "parameter")
  {
    const uint16_t osc = free_port(false);
    const uint16_t ws = free_port(true);
    multiplex->expose_to(
        std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(
            ctx, osc, ws, false));
    std::thread net([&] { ctx->run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    {
      ossia::net::generic_device client{
          std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
              "ws://127.0.0.1:" + std::to_string(ws), free_port(false)),
          "client"};
      REQUIRE(client.get_protocol().update(client.get_root_node()));
      auto& croot = client.get_root_node();

      REQUIRE(ossia::net::find_node(croot, "/manual/gain") != nullptr);
      auto* sg = ossia::net::find_node(croot, "/synth/gain");
      REQUIRE(sg != nullptr);
      REQUIRE(ossia::net::find_node(croot, "/synth.1/gain") != nullptr);
      REQUIRE(ossia::net::find_node(croot, "/synth/engine/reset") != nullptr);

      auto* p = sg->get_parameter();
      REQUIRE(p != nullptr);
      REQUIRE(p->get_value_type() == ossia::val_type::FLOAT);
      REQUIRE(p->get_domain().get_min() == ossia::value(-60.f));
      REQUIRE(p->get_domain().get_max() == ossia::value(6.f));
      REQUIRE(p->get_bounding() == ossia::bounding_mode::CLIP);
      REQUIRE(ossia::get_pretty_unit_text(p->get_unit()) == "gain.db");
      REQUIRE(
          ossia::net::get_description(*sg) == std::optional<std::string>{"main gain"});

      auto* pos = ossia::net::find_node(croot, "/synth/pos")->get_parameter();
      REQUIRE(pos->get_value_type() == ossia::val_type::VEC2F);
      REQUIRE(pos->get_domain().get_min() == ossia::value(ossia::make_vec(-1.f, -1.f)));
      REQUIRE(pos->get_domain().get_max() == ossia::value(ossia::make_vec(1.f, 1.f)));

      auto* voices = ossia::net::find_node(croot, "/synth/voices")->get_parameter();
      REQUIRE(voices->value() == ossia::value(4));

      auto* mode = ossia::net::find_node(croot, "/synth/mode")->get_parameter();
      REQUIRE(mode->get_value_type() == ossia::val_type::STRING);
      REQUIRE(ossia::get_values(mode->get_domain()).size() == 2);
    }

    ossia::net::stop_network_context(*ctx);
    net.join();
  }

  SECTION(
      "the output is filtered by the parameter, as at the outlet of every ossia object")
  {
    // @repetitions 0: the repetition filter is on, two identical values give one output
    REQUIRE(b1.add(spec_of(
                       "rep", attrs(
                                  {{"type", {S("float")}},
                                   {"min", {0}},
                                   {"max", {1}},
                                   {"repetitions", {0}}})))
                .empty());
    // @repetitions 1 (the default): two identical values give two outputs
    REQUIRE(
        b1.add(spec_of(
                   "norep", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}})))
            .empty());
    // @mute 1: nothing leaves
    REQUIRE(b1.add(spec_of(
                       "muted", attrs(
                                    {{"type", {S("float")}},
                                     {"min", {0}},
                                     {"max", {1}},
                                     {"mute", {1}}})))
                .empty());
    // @enable 0: nothing leaves either
    REQUIRE(b1.add(spec_of(
                       "off", attrs(
                                  {{"type", {S("float")}},
                                   {"min", {0}},
                                   {"max", {1}},
                                   {"enable", {0}}})))
                .empty());
    b1.drain();

    auto* rep = ossia::net::find_node(root, "/synth/rep")->get_parameter();
    auto* norep = ossia::net::find_node(root, "/synth/norep")->get_parameter();
    auto* muted = ossia::net::find_node(root, "/synth/muted")->get_parameter();
    auto* off = ossia::net::find_node(root, "/synth/off")->get_parameter();
    REQUIRE(rep->get_repetition_filter() == ossia::repetition_filter::ON);
    REQUIRE(norep->get_repetition_filter() == ossia::repetition_filter::OFF);
    REQUIRE(muted->get_muted());
    REQUIRE(off->get_disabled());

    // another source pushes the same value twice
    rep->push_value(0.3f);
    rep->push_value(0.3f);
    auto in = b1.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "rep");
    REQUIRE(in[0].value == ossia::value(0.3f));
    rep->push_value(0.4f);
    REQUIRE(b1.drain().size() == 1);

    norep->push_value(0.3f);
    norep->push_value(0.3f);
    in = b1.drain();
    REQUIRE(in.size() == 2);
    REQUIRE(in[0].address == "norep");
    REQUIRE(in[1].value == ossia::value(0.3f));

    muted->push_value(0.5f);
    off->push_value(0.5f);
    REQUIRE(b1.drain().empty());
    REQUIRE(muted->value() == ossia::value(0.5f)); // stored, not output

    // the same filter applies to a bound address: the follower of rep sees one value
    branch f;
    f.set_resolution_context({&root}, {&dev});
    REQUIRE(f.bind("synth/rep").empty());
    f.drain();
    rep->push_value(0.7f);
    rep->push_value(0.7f);
    in = f.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "synth/rep");
    REQUIRE(f.bind("synth/muted").empty());
    REQUIRE(f.drain().empty()); // the value found on bind is filtered too
  }

  SECTION("remove and clear")
  {
    REQUIRE(b1.remove("engine/reset"));
    REQUIRE(
        ossia::net::find_node(root, "/synth/engine")
        == nullptr); // empty container pruned
    REQUIRE(!b1.remove("engine/reset"));
    REQUIRE(b1.size() == 5);

    b2.clear();
    REQUIRE(ossia::net::find_node(root, "/synth.1") != nullptr);
    REQUIRE(bn2.children().empty());
  }

  SECTION("the enclosing node disappears: declarations survive and come back")
  {
    b1.drain();
    root.remove_child(bn1);
    REQUIRE(lost == 1);
    REQUIRE(!b1.attached());
    REQUIRE(!b1.set("gain", {0}).empty());
    expect_refused(b1, "gain");

    auto& bn3 = ossia::net::create_node(root, "/synth");
    REQUIRE(bn3.osc_address() == "/synth");
    b1.attach(bn3);
    REQUIRE(ossia::net::find_node(root, "/synth/cutoff") != nullptr);
    REQUIRE(ossia::net::find_node(root, "/synth/engine/reset") != nullptr);
    // the defaults come out again after the new registration
    auto in = b1.drain();
    REQUIRE(in.size() == 3);
    REQUIRE(in[0].address == "gain");
    REQUIRE(in[0].value == ossia::value(-12.f));
    REQUIRE(b1.set("gain", {-3}).empty());
  }
}

TEST_CASE("v8connector_bind", "v8connector_bind")
{
  auto proto = std::make_unique<ossia::net::multiplex_protocol>();
  ossia::net::generic_device dev{std::move(proto), "v8_bind"};
  auto& root = dev.get_root_node();

  // A parameter created by hand, as ossia.parameter does
  {
    auto& n = ossia::net::create_node(root, "/manual/gain");
    auto* p = ossia::try_setup_parameter("float", n);
    p->set_domain(ossia::make_domain(0.f, 1.f));
    p->push_value(0.25f);
  }

  // Instance A declares, instance B follows. Both resolve like an ossia.remote
  // placed in a patcher whose device is dev.
  auto& bn_a = ossia::net::create_node(root, "/synth");
  auto& bn_b = ossia::net::create_node(root, "/mirror");
  branch a, b;
  a.attach(bn_a);
  b.attach(bn_b);
  std::vector<std::pair<std::string, std::size_t>> changes;
  b.set_binding_changed_callback(
      [&](const std::string& addr, std::size_t n) { changes.emplace_back(addr, n); });
  a.set_resolution_context({&root}, {&dev});
  b.set_resolution_context({&root}, {&dev});

  const auto gain = spec_of(
      "gain", attrs(
                  {{"type", {S("float")}},
                   {"min", {-60}},
                   {"max", {6}},
                   {"default", {-12}},
                   {"clip", {S("both")}}}));
  REQUIRE(a.add(gain).empty());
  REQUIRE(
      a.add(
           spec_of("voices", attrs({{"type", {S("int")}}, {"min", {1}}, {"max", {16}}})))
          .empty());

  SECTION("bind follows an existing parameter, set goes both ways, no echo")
  {
    REQUIRE(b.bind("synth/gain").empty());
    REQUIRE(b.bound_count("synth/gain") == 1);
    REQUIRE(changes.back() == std::make_pair(std::string("synth/gain"), std::size_t(1)));
    REQUIRE(b.bind("synth/gain").empty()); // twice: accepted, nothing changes
    REQUIRE(b.bound_count("synth/gain") == 1);
    REQUIRE(ossia::net::find_node(root, "/mirror/synth") == nullptr); // nothing created

    // the current value is delivered on bind
    auto in = b.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "synth/gain");
    REQUIRE(in[0].value == ossia::value(-12.f));
    a.drain();

    // B sets: A receives, B does not
    REQUIRE(b.set("synth/gain", {-20}).empty());
    in = a.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "gain");
    REQUIRE(in[0].value == ossia::value(-20.f));
    REQUIRE(b.drain().empty());

    // A sets: B receives under the bound address, A does not
    REQUIRE(a.set("gain", {-10}).empty());
    in = b.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "synth/gain");
    REQUIRE(in[0].value == ossia::value(-10.f));
    REQUIRE(a.drain().empty());

    // the owner's type and range apply to the follower's pushes, and the
    // effective value comes back to the follower that sent it
    REQUIRE(b.set("synth/gain", {100}).empty());
    auto* gp = ossia::net::find_node(root, "/synth/gain")->get_parameter();
    REQUIRE(gp->value() == ossia::value(100.f));
    in = b.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "synth/gain");
    REQUIRE(in[0].value == ossia::value(6.f));
    in = a.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].value == ossia::value(6.f));
    REQUIRE(!b.set("synth/gain", {S("loud")}).empty());
    expect_refused(b, "synth/gain");

    // another source pushes out of range straight on the parameter, as an
    // ossia.remote does: both instances output the bounded value
    gp->push_value(-116.f);
    in = a.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "gain");
    REQUIRE(in[0].value == ossia::value(-60.f));
    in = b.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "synth/gain");
    REQUIRE(in[0].value == ossia::value(-60.f));

    // unbind: nothing flows any more, the parameter is untouched
    REQUIRE(b.unbind("synth/gain"));
    REQUIRE(!b.unbind("synth/gain"));
    expect_refused(b, "synth/gain");
    REQUIRE(a.set("gain", {0}).empty());
    REQUIRE(b.drain().empty());
    REQUIRE(ossia::net::find_node(root, "/synth/gain") != nullptr);
  }

  SECTION("the followed parameter disappears and comes back")
  {
    REQUIRE(b.bind("synth/gain").empty());
    b.drain();
    a.drain();
    REQUIRE(a.remove("gain"));
    REQUIRE(b.bound_count("synth/gain") == 0);
    REQUIRE(changes.back() == std::make_pair(std::string("synth/gain"), std::size_t(0)));
    REQUIRE(!b.set("synth/gain", {1}).empty());
    expect_refused(b, "synth/gain");

    REQUIRE(a.add(gain).empty());
    REQUIRE(b.bound_count("synth/gain") == 1);
    // the follower outputs the value it finds, once: the quiet default did
    // not fire its callback, the resolution read the final value
    auto in = b.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "synth/gain");
    REQUIRE(in[0].value == ossia::value(-12.f));
    // the owner outputs its default locally
    in = a.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "gain");
    REQUIRE(in[0].value == ossia::value(-12.f));
    REQUIRE(b.set("synth/gain", {-30}).empty());
    in = a.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].value == ossia::value(-30.f));

    // the whole owner node disappears (its model registers again) and comes back
    root.remove_child(bn_a);
    REQUIRE(b.bound_count("synth/gain") == 0);
    auto& bn_a2 = ossia::net::create_node(root, "/synth");
    b.drain();
    a.attach(bn_a2);
    REQUIRE(b.bound_count("synth/gain") == 1);
    in = b.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].value == ossia::value(-12.f));
    REQUIRE(b.set("synth/gain", {-40}).empty());
    REQUIRE(
        ossia::net::find_node(root, "/synth/gain")->get_parameter()->value()
        == ossia::value(-40.f));
  }

  SECTION("a bind made before the declaration outputs the default once it exists")
  {
    REQUIRE(b.bind("synth/cutoff").empty());
    REQUIRE(b.bound_count("synth/cutoff") == 0);
    REQUIRE(b.drain().empty());
    REQUIRE(a.add(spec_of(
                      "cutoff", attrs(
                                    {{"type", {S("float")}},
                                     {"range", {20, 20000}},
                                     {"default", {1000}}})))
                .empty());
    REQUIRE(b.bound_count("synth/cutoff") == 1);
    auto in = b.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "synth/cutoff");
    REQUIRE(in[0].value == ossia::value(1000.f));
  }

  SECTION("a bind to a parameter that does not exist yet waits for it")
  {
    REQUIRE(b.bind("later/x").empty());
    REQUIRE(b.bound_count("later/x") == 0);
    auto& n = ossia::net::create_node(root, "/later/x");
    ossia::try_setup_parameter("int", n);
    REQUIRE(b.bound_count("later/x") == 1);
  }

  SECTION("absolute, global and pattern addresses, as ossia.remote")
  {
    REQUIRE(b.bind("/manual/gain").empty());
    REQUIRE(b.bound_count("/manual/gain") == 1);
    REQUIRE(b.bind("v8_bind:/manual/gain").empty());
    REQUIRE(b.bound_count("v8_bind:/manual/gain") == 1);
    REQUIRE(b.bind("synth/*").empty());
    REQUIRE(b.bound_count("synth/*") == 2);
    b.drain();
    REQUIRE(a.set("voices", {8}).empty());
    auto in = b.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(in[0].address == "synth/voices");
    REQUIRE(in[0].value == ossia::value(8));
    // a new match of the pattern is followed when it appears
    REQUIRE(
        a.add(spec_of("cutoff", attrs({{"type", {S("float")}}, {"range", {20, 20000}}})))
            .empty());
    REQUIRE(b.bound_count("synth/*") == 3);
  }

  SECTION("bind and add do not share an address inside one instance")
  {
    REQUIRE(!b.bind("").empty());
    REQUIRE(a.bind("manual/gain").empty());
    REQUIRE(!a.bind("gain").empty()); // declared by add in a
    REQUIRE(!a.add(spec_of(
                       "manual/gain",
                       attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}})))
                 .empty());
  }
}

TEST_CASE("v8connector_node_on_first_add", "v8connector_node_on_first_add")
{
  auto proto = std::make_unique<ossia::net::multiplex_protocol>();
  ossia::net::generic_device dev{std::move(proto), "v8_node"};
  auto& root = dev.get_root_node();
  {
    auto& n = ossia::net::create_node(root, "/manual/gain");
    ossia::try_setup_parameter("float", n);
  }

  // The host side: a factory that creates the node under the device root, as
  // ossia.model does, and counts how many times it is asked.
  int created = 0;
  branch b;
  b.set_node_factory([&]() -> ossia::net::node_base* {
    ++created;
    return &ossia::net::create_node(root, "/mirror");
  });
  b.set_resolution_context({&root}, {&dev});

  SECTION("an instance that only binds creates no node, like an ossia.remote")
  {
    b.reattach(); // what registration does
    REQUIRE(created == 0);
    REQUIRE(b.bind("manual/gain").empty());
    REQUIRE(b.bound_count("manual/gain") == 1);
    REQUIRE(created == 0);
    REQUIRE(!b.attached());
    REQUIRE(ossia::net::find_node(root, "/mirror") == nullptr);
    REQUIRE(b.set("manual/gain", {0.5f}).empty());
    REQUIRE(created == 0);
  }

  SECTION("the first add creates the node, which then stays like an ossia.model")
  {
    b.reattach();
    REQUIRE(created == 0);
    REQUIRE(
        b.add(spec_of("x", attrs({{"type", {S("int")}}, {"min", {0}}, {"max", {10}}})))
            .empty());
    REQUIRE(created == 1);
    REQUIRE(b.attached());
    REQUIRE(ossia::net::find_node(root, "/mirror/x") != nullptr);
    REQUIRE(b.add(spec_of("y", attrs({{"type", {S("bool")}}}))).empty());
    REQUIRE(created == 1);

    b.clear();
    REQUIRE(b.attached());
    REQUIRE(ossia::net::find_node(root, "/mirror") != nullptr);
    REQUIRE(ossia::net::find_node(root, "/mirror")->children().empty());
    REQUIRE(created == 1);

    REQUIRE(b.add(spec_of("z", attrs({{"type", {S("string")}}}))).empty());
    REQUIRE(b.remove("z"));
    REQUIRE(b.attached());
    REQUIRE(created == 1);

    // the host detaches and registers again (its node was destroyed): the
    // node is asked for again only because there are leaves
    REQUIRE(b.add(spec_of("w", attrs({{"type", {S("impulse")}}}))).empty());
    b.detach();
    root.remove_child(*ossia::net::find_node(root, "/mirror"));
    b.reattach();
    REQUIRE(created == 2);
    REQUIRE(ossia::net::find_node(root, "/mirror/w") != nullptr);
  }

  SECTION("a factory that cannot create yet is asked again later")
  {
    bool ready = false;
    int asked = 0;
    b.set_node_factory([&]() -> ossia::net::node_base* {
      ++asked;
      return ready ? &ossia::net::create_node(root, "/mirror") : nullptr;
    });
    REQUIRE(b.add(spec_of("x", attrs({{"type", {S("bool")}}}))).empty());
    REQUIRE(asked == 1);
    REQUIRE(!b.attached());
    ready = true;
    b.reattach();
    REQUIRE(asked == 2);
    REQUIRE(b.attached());
    REQUIRE(ossia::net::find_node(root, "/mirror/x") != nullptr);
  }
}

namespace
{
std::string served(const ossia::net::node_base& n)
{
  return ossia::oscquery::json_writer::query_namespace(n).GetString();
}

bool is_refusal(const inbound& in, const std::string& address)
{
  if(in.address != "refused" || in.value.get_type() != ossia::val_type::LIST)
    return false;
  const auto& l = in.value.get<std::vector<ossia::value>>();
  return l.size() == 2 && l[0] == ossia::value(address)
         && l[1].get_type() == ossia::val_type::STRING
         && !l[1].get<std::string>().empty();
}
}

TEST_CASE("v8connector_units_clip_refusals", "v8connector_units_clip_refusals")
{
  auto proto = std::make_unique<ossia::net::multiplex_protocol>();
  ossia::net::generic_device dev{std::move(proto), "v8_units"};
  auto& root = dev.get_root_node();
  branch b;
  b.set_node_factory([&]() -> ossia::net::node_base* {
    return &ossia::net::create_node(root, "/types");
  });
  b.set_resolution_context({&root}, {&dev});

  SECTION(
      "color units are kept by the parameter, after creation and after re-registration")
  {
    // the atoms the helper sends for these two declarations
    REQUIRE(b.declare(
                 "color", attrs(
                              {{"type", {S("vec3f")}},
                               {"min", {0}},
                               {"max", {1}},
                               {"unit", {S("color.rgb")}},
                               {"default", {1, 0.5f, 0}}}))
                .empty());
    REQUIRE(b.declare(
                 "rgba", attrs(
                             {{"type", {S("vec4f")}},
                              {"min", {0}},
                              {"max", {1}},
                              {"unit", {S("color.rgba")}},
                              {"default", {0, 0.5f, 1, 1}}}))
                .empty());
    auto check = [&] {
      auto* c = ossia::net::find_node(root, "/types/color")->get_parameter();
      auto* r = ossia::net::find_node(root, "/types/rgba")->get_parameter();
      REQUIRE(ossia::get_pretty_unit_text(c->get_unit()) == "color.rgb");
      REQUIRE(c->get_value_type() == ossia::val_type::VEC3F);
      REQUIRE(ossia::get_pretty_unit_text(r->get_unit()) == "color.rgba");
      REQUIRE(r->get_value_type() == ossia::val_type::VEC4F);
      REQUIRE(c->value() == ossia::value(ossia::make_vec(1.f, 0.5f, 0.f)));
    };
    check();
    // the parent registers again: its node is destroyed, then recreated
    b.detach();
    root.remove_child(*ossia::net::find_node(root, "/types"));
    b.reattach();
    check();
  }

  SECTION("a type the unit would change is refused, with the reason")
  {
    REQUIRE(!b.declare(
                  "rgb", attrs(
                             {{"type", {S("vec4f")}},
                              {"min", {0}},
                              {"max", {1}},
                              {"unit", {S("color.rgb")}}}))
                 .empty());
    REQUIRE(!b.declare(
                  "g", attrs(
                           {{"type", {S("int")}},
                            {"min", {-60}},
                            {"max", {6}},
                            {"unit", {S("gain.db")}}}))
                 .empty());
    auto in = b.drain();
    REQUIRE(in.size() == 2);
    REQUIRE(is_refusal(in[0], "rgb"));
    const auto reason
        = in[0].value.get<std::vector<ossia::value>>()[1].get<std::string>();
    REQUIRE(reason.find("requires type 'vec3f'") != std::string::npos);
    REQUIRE(is_refusal(in[1], "g"));
    REQUIRE(ossia::net::find_node(root, "/types") == nullptr); // nothing created
    REQUIRE(b.size() == 0);
  }

  SECTION("clip none is accepted as the free mode and served CLIPMODE none")
  {
    REQUIRE(b.declare(
                 "x", attrs(
                          {{"type", {S("float")}},
                           {"min", {0}},
                           {"max", {1}},
                           {"clip", {S("none")}}}))
                .empty());
    auto* p = ossia::net::find_node(root, "/types/x")->get_parameter();
    REQUIRE(p->get_bounding() == ossia::bounding_mode::FREE);
    REQUIRE(
        served(*ossia::net::find_node(root, "/types/x")).find("\"CLIPMODE\":\"none\"")
        != std::string::npos);
    // replaying the same declaration with "none" or "free" is the same leaf
    REQUIRE(b.declare(
                 "x", attrs(
                          {{"type", {S("float")}},
                           {"min", {0}},
                           {"max", {1}},
                           {"clip", {S("free")}}}))
                .empty());
    REQUIRE(b.size() == 1);
    REQUIRE(!b.declare(
                  "y", attrs(
                           {{"type", {S("float")}},
                            {"min", {0}},
                            {"max", {1}},
                            {"clip", {S("sometimes")}}}))
                 .empty());
  }

  SECTION("every refusal is queued as refused <address> <reason>, nothing is created")
  {
    b.drain();
    REQUIRE(!b.declare("cutoff", attrs({{"type", {S("float")}}})).empty()); // no range
    auto in = b.drain();
    REQUIRE(in.size() == 1);
    REQUIRE(is_refusal(in[0], "cutoff"));
    REQUIRE(ossia::net::find_node(root, "/types/cutoff") == nullptr);
    REQUIRE(b.size() == 0);

    REQUIRE(
        b.declare("gain", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}}))
            .empty());
    b.drain();
    REQUIRE(
        !b.declare("gain", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {2}}}))
             .empty());
    REQUIRE(!b.set("nothere", {1}).empty());
    REQUIRE(!b.set("gain", {S("loud")}).empty());
    REQUIRE(!b.remove("nothere"));
    REQUIRE(!b.unbind("nothere"));
    REQUIRE(!b.bind("gain").empty()); // declared by add in this instance
    in = b.drain();
    REQUIRE(in.size() == 6);
    REQUIRE(is_refusal(in[0], "gain"));
    REQUIRE(is_refusal(in[1], "nothere"));
    REQUIRE(is_refusal(in[2], "gain"));
    REQUIRE(is_refusal(in[3], "nothere"));
    REQUIRE(is_refusal(in[4], "nothere"));
    REQUIRE(is_refusal(in[5], "gain"));
    // a refusal wakes the host like a value does
    int wakes = 0;
    b.set_wake_callback([&] { ++wakes; });
    b.refuse("z", "reason");
    REQUIRE(wakes == 1);
  }
}

// A device that a branch follows disappears and comes back under the same
// name, as an ossia.client connected by name does when the zeroconf
// announcement of its server goes away and comes back. The host gives the
// branch the new device, as it does when a device is created; it does not
// tell the branch that the old one is gone, because ossia-max's client clears
// its device before announcing the removal.
TEST_CASE("v8connector_device_return", "v8connector_device_return")
{
  // The server: leaves under /bench, served over OSCQuery.
  auto ctx = ossia::net::create_network_context();
  auto server_multiplex = std::make_unique<ossia::net::multiplex_protocol>();
  auto* multiplex = server_multiplex.get();
  ossia::net::generic_device server{std::move(server_multiplex), "remote"};
  std::vector<ossia::net::parameter_base*> leaves;
  for(int i = 0; i < 4; i++)
  {
    auto& n = ossia::net::create_node(
        server.get_root_node(), "/bench/p" + std::to_string(i));
    auto* p = ossia::try_setup_parameter("float", n);
    p->set_domain(ossia::make_domain(0.f, 1.f));
    p->push_value(0.f);
    leaves.push_back(p);
  }
  const uint16_t osc = free_port(false);
  const uint16_t ws = free_port(true);
  multiplex->expose_to(std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(
      ctx, osc, ws, false));
  std::thread net([&] { ctx->run(); });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // The global device of Max, where the instance registers.
  ossia::net::generic_device max_dev{
      std::make_unique<ossia::net::multiplex_protocol>(), "max"};
  auto& max_root = max_dev.get_root_node();

  auto connect = [&] {
    auto d = std::make_shared<ossia::net::generic_device>(
        std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
            "ws://127.0.0.1:" + std::to_string(ws), free_port(false)),
        "remote");
    REQUIRE(d->get_protocol().update(d->get_root_node()));
    return d;
  };

  // The server pushes the value until the branch receives it: a client that
  // has just connected may not be known to the server yet.
  auto received = [](branch& b, ossia::net::parameter_base& p, const ossia::value& v) {
    for(int i = 0; i < 400; i++)
    {
      if(i % 20 == 0)
        p.push_value(v);
      for(auto& in : b.drain())
        if(in.address == "remote:/bench/p0" && in.value == v)
          return true;
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return false;
  };

  branch b;
  std::shared_ptr<ossia::net::generic_device> client = connect();
  b.set_resolution_context({&max_root}, {&max_dev, client.get()});
  REQUIRE(b.bind("remote:/bench/p0").empty());
  REQUIRE(b.bound_count("remote:/bench/p0") == 1);

  for(int cycle = 0; cycle < 20; cycle++)
  {
    INFO("cycle " << cycle);
    // The device goes away; the host does not call drop_device.
    client.reset();
    REQUIRE(b.bound_count("remote:/bench/p0") == 0);

    // It comes back under the same name: the host registers the instance
    // again with the devices it knows.
    client = connect();
    b.set_resolution_context({&max_root}, {&max_dev, client.get()});
    REQUIRE(b.bound_count("remote:/bench/p0") == 1);

    // Values are received again.
    const bool ok = received(b, *leaves[0], ossia::value((cycle + 1) / 32.f));
    if(!ok)
    {
      client.reset();
      ossia::net::stop_network_context(*ctx);
      net.join();
    }
    REQUIRE(ok);
  }

  client.reset();
  ossia::net::stop_network_context(*ctx);
  net.join();
}

// An ossia.client connected by name gets its device from the zeroconf
// listener, which builds an OSCQuery mirror and reads the tree once. The tree
// changes the server sends afterwards wait in the mirror's command queue until
// the client polls it.
TEST_CASE("v8connector_client_by_name", "v8connector_client_by_name")
{
  auto ctx = ossia::net::create_network_context();
  auto server_multiplex = std::make_unique<ossia::net::multiplex_protocol>();
  auto* multiplex = server_multiplex.get();
  ossia::net::generic_device server{std::move(server_multiplex), "remote"};
  {
    auto& n = ossia::net::create_node(server.get_root_node(), "/bench/first");
    ossia::try_setup_parameter("float", n)->push_value(0.f);
  }
  const uint16_t osc = free_port(false);
  const uint16_t ws = free_port(true);
  multiplex->expose_to(std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(
      ctx, osc, ws, false));
  std::thread net([&] { ctx->run(); });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // As ZeroconfOscqueryListener::addInstance builds it.
  auto client = std::make_shared<ossia::net::generic_device>(
      std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
          "ws://127.0.0.1:" + std::to_string(ws), free_port(false)),
      "remote");
  REQUIRE(client->get_protocol().update(client->get_root_node()));
  auto& croot = client->get_root_node();
  REQUIRE(ossia::net::find_node(croot, "/bench/first") != nullptr);

  ossia::net::generic_device max_dev{
      std::make_unique<ossia::net::multiplex_protocol>(), "max"};
  branch b;
  b.set_resolution_context({&max_dev.get_root_node()}, {&max_dev, client.get()});
  REQUIRE(b.bind("remote:/bench/late").empty());
  REQUIRE(b.bound_count("remote:/bench/late") == 0);

  // Without polling, a parameter added on the server never shows up.
  {
    auto& n = ossia::net::create_node(server.get_root_node(), "/bench/late");
    ossia::try_setup_parameter("float", n)->push_value(0.5f);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  REQUIRE(ossia::net::find_node(croot, "/bench/late") == nullptr);

  // The client polls the mirror, as it does for a connection by URL.
  auto* mirror = polled_mirror(*client);
  auto poll_until = [&](auto done) {
    for(int i = 0; i < 400 && mirror && !done(); i++)
    {
      mirror->run_commands();
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return done();
  };

  bool ok = mirror != nullptr;
  ok = ok && poll_until([&] {
    return ossia::net::find_node(croot, "/bench/late") != nullptr;
  });
  // The bind made before the parameter existed resolves when it appears.
  ok = ok && b.bound_count("remote:/bench/late") == 1;

  // A path removed on the server is removed on the client.
  if(ok)
  {
    server.get_root_node().find_child("bench")->remove_child("late");
    ok = poll_until(
        [&] { return ossia::net::find_node(croot, "/bench/late") == nullptr; });
    ok = ok && b.bound_count("remote:/bench/late") == 0;
  }

  client.reset();
  ossia::net::stop_network_context(*ctx);
  net.join();
  REQUIRE(ok);
}

namespace
{
bool drained_refusal(branch& b, const std::string& address, const std::string& reason)
{
  for(auto& in : b.drain())
    if(in.address == "refused")
    {
      const auto& l = in.value.get<std::vector<ossia::value>>();
      if(l[0] == ossia::value(address) && l[1] == ossia::value(reason))
        return true;
    }
  return false;
}
}

// [ossia.v8connector] without a name: the leaves sit directly under the node
// of the enclosing model, next to the ossia.parameter objects.
TEST_CASE("v8connector_without_name", "v8connector_without_name")
{
  auto proto = std::make_unique<ossia::net::multiplex_protocol>();
  ossia::net::generic_device dev{std::move(proto), "v8_shared"};
  auto& root = dev.get_root_node();

  // The node of an ossia.model, with an ossia.parameter and an empty child
  // node made by another object.
  auto* model = &ossia::net::create_node(root, "/synth");
  {
    auto* p
        = ossia::try_setup_parameter("float", ossia::net::create_node(*model, "manual"));
    p->push_value(0.25f);
    ossia::net::create_node(*model, "fx");
  }

  auto shared = [&](branch& b) {
    b.set_owns_node(false);
    b.set_node_factory([&]() -> ossia::net::node_base* { return model; });
    b.set_resolution_context({&root}, {&dev});
  };
  branch a, b;
  shared(a);
  shared(b);
  const auto gain = attrs(
      {{"type", {S("float")}}, {"min", {-60}}, {"max", {6}}, {"default", {-12}}});

  REQUIRE(a.declare("gain", gain).empty());
  auto* a_gain = ossia::net::find_node(root, "/synth/gain");
  REQUIRE(a_gain != nullptr);
  REQUIRE(a_gain->get_parameter() != nullptr);
  REQUIRE(a.node() == model);
  a.drain();

  SECTION("an address taken by another instance is refused, never renamed")
  {
    REQUIRE(!b.declare("gain", gain).empty());
    REQUIRE(drained_refusal(b, "gain", "already exists"));
    REQUIRE(b.size() == 0);
    REQUIRE(ossia::net::find_node(root, "/synth/gain") == a_gain);
    REQUIRE(ossia::net::find_node(root, "/synth/gain.1") == nullptr);
    REQUIRE(a_gain->get_parameter()->value() == ossia::value(-12.f));
  }

  SECTION("an address taken by an ossia.parameter is refused")
  {
    REQUIRE(
        !a.declare("manual", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}}))
             .empty());
    REQUIRE(drained_refusal(a, "manual", "already exists"));
    REQUIRE(
        ossia::net::find_node(root, "/synth/manual")->get_parameter()->value()
        == ossia::value(0.25f));
  }

  SECTION("for comparison, ossia.parameter renames: a second one becomes gain.1")
  {
    // object_base.cpp, case object_class::param: a node that already holds a
    // parameter is replaced by create_node(parent, name), which picks a free name.
    auto* n = &ossia::net::create_node(*a_gain->get_parent(), a_gain->get_name());
    REQUIRE(n != a_gain);
    REQUIRE(n->get_name() == "gain.1");
  }

  SECTION("remove takes only the leaves of its instance, and only the nodes it created")
  {
    REQUIRE(
        b.declare(
             "env/attack", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}}))
            .empty());
    REQUIRE(
        b.declare("fx/mix", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}}))
            .empty());
    REQUIRE(ossia::net::find_node(root, "/synth/env/attack") != nullptr);
    b.drain();

    REQUIRE(!a.remove("env/attack"));
    REQUIRE(drained_refusal(a, "env/attack", "unknown address 'env/attack'"));
    REQUIRE(!b.remove("gain"));
    REQUIRE(ossia::net::find_node(root, "/synth/gain") == a_gain);

    REQUIRE(b.remove("env/attack"));
    REQUIRE(ossia::net::find_node(root, "/synth/env") == nullptr);
    REQUIRE(b.remove("fx/mix"));
    // fx existed before: it stays
    REQUIRE(ossia::net::find_node(root, "/synth/fx") != nullptr);
    REQUIRE(ossia::net::find_node(root, "/synth/manual") != nullptr);
    REQUIRE(ossia::net::find_node(root, "/synth/gain") == a_gain);
  }

  SECTION("the replay on ready does not conflict with itself")
  {
    REQUIRE(a.declare("gain", gain).empty());
    for(auto& in : a.drain())
      REQUIRE(in.address != "refused");
    REQUIRE(ossia::net::find_node(root, "/synth/gain") == a_gain);
    REQUIRE(ossia::net::find_node(root, "/synth/gain.1") == nullptr);
  }

  SECTION(
      "detached, the instance leaves the node as it found it; an address taken "
      "meanwhile is refused on return")
  {
    REQUIRE(
        a.declare(
             "env/attack", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}}))
            .empty());
    a.detach();
    REQUIRE(ossia::net::find_node(root, "/synth/gain") == nullptr);
    REQUIRE(ossia::net::find_node(root, "/synth/env") == nullptr);
    REQUIRE(ossia::net::find_node(root, "/synth/manual") != nullptr);
    REQUIRE(ossia::net::find_node(root, "/synth/fx") != nullptr);
    REQUIRE(ossia::net::find_node(root, "/synth") == model);

    // an ossia.parameter takes gain while the instance is away
    ossia::try_setup_parameter("float", ossia::net::create_node(*model, "gain"));
    a.drain();
    a.reattach();
    REQUIRE(drained_refusal(a, "gain", "already exists"));
    REQUIRE(a.size() == 1);
    REQUIRE(ossia::net::find_node(root, "/synth/env/attack") != nullptr);
  }

  SECTION("the model registers again: the leaves come back under its new node")
  {
    root.remove_child("synth");
    REQUIRE(!a.attached());
    model = &ossia::net::create_node(root, "/synth");
    a.reattach();
    REQUIRE(ossia::net::find_node(root, "/synth/gain") != nullptr);
  }
}

// Without a name and without an enclosing model, the leaves sit at the root of
// the device, which the branch also knows for its bindings. The device goes away.
TEST_CASE("v8connector_without_name_device_root", "v8connector_without_name_device_root")
{
  auto d = std::make_unique<ossia::net::generic_device>(
      std::make_unique<ossia::net::multiplex_protocol>(), "v8_root");
  branch a;
  a.set_owns_node(false);
  a.set_node_factory(
      [&]() -> ossia::net::node_base* { return d ? &d->get_root_node() : nullptr; });
  a.set_resolution_context({&d->get_root_node()}, {d.get()});
  REQUIRE(a.declare("gain", attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1}}}))
              .empty());
  REQUIRE(ossia::net::find_node(d->get_root_node(), "/gain") != nullptr);
  REQUIRE(a.bind("gain.1").empty());

  d.reset();
  REQUIRE(!a.attached());
  // the branch no longer knows the device: a new context does not touch it
  ossia::net::generic_device other{
      std::make_unique<ossia::net::multiplex_protocol>(), "other"};
  a.set_resolution_context({&other.get_root_node()}, {&other});
  REQUIRE(a.size() == 1);
}

// A server removes a parameter and creates it again at the same address, as
// a server that restarts publishes its tree again. The client receives
// PATH_REMOVED then PATH_ADDED; a bind to that address follows the new
// parameter, values included.
TEST_CASE("v8connector_parameter_recreated", "v8connector_parameter_recreated")
{
  auto ctx = ossia::net::create_network_context();
  auto server_multiplex = std::make_unique<ossia::net::multiplex_protocol>();
  auto* multiplex = server_multiplex.get();
  ossia::net::generic_device server{std::move(server_multiplex), "remote"};
  auto make_leaf = [&] {
    auto& n = ossia::net::create_node(server.get_root_node(), "/bench/p0");
    auto* p = ossia::try_setup_parameter("float", n);
    p->set_domain(ossia::make_domain(0.f, 1.f));
    p->push_value(0.f);
    return p;
  };
  auto* leaf = make_leaf();
  const uint16_t osc = free_port(false);
  const uint16_t ws = free_port(true);
  multiplex->expose_to(std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(
      ctx, osc, ws, false));
  std::thread net([&] { ctx->run(); });
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // A client connected by URL, polled as ossia.client polls it.
  auto client = std::make_shared<ossia::net::generic_device>(
      std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
          "ws://127.0.0.1:" + std::to_string(ws), free_port(false)),
      "remote");
  REQUIRE(client->get_protocol().update(client->get_root_node()));
  auto* mirror = polled_mirror(*client);
  auto& croot = client->get_root_node();

  ossia::net::generic_device max_dev{
      std::make_unique<ossia::net::multiplex_protocol>(), "max"};
  branch b;
  b.set_resolution_context({&max_dev.get_root_node()}, {&max_dev, client.get()});
  REQUIRE(b.bind("remote:/bench/p0").empty());
  REQUIRE(b.bound_count("remote:/bench/p0") == 1);

  auto poll_until = [&](auto done) {
    for(int i = 0; i < 400 && !done(); i++)
    {
      mirror->run_commands();
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return done();
  };
  auto received = [&](ossia::net::parameter_base& p, const ossia::value& v) {
    for(int i = 0; i < 400; i++)
    {
      if(i % 20 == 0)
        p.push_value(v);
      mirror->run_commands();
      for(auto& in : b.drain())
        if(in.address == "remote:/bench/p0" && in.value == v)
          return true;
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return false;
  };

  bool ok = received(*leaf, ossia::value(0.25f));
  INFO("before: " << ok);

  // The client reads the whole tree again (update): every parameter is
  // removed, then created again at the same address.
  bool reloaded = false, reload_bound = false, after_reload = false;
  if(ok)
  {
    reloaded = client->get_protocol().update(croot);
    reload_bound = poll_until([&] { return b.bound_count("remote:/bench/p0") == 1; });
    after_reload = received(*leaf, ossia::value(0.5f));
  }

  // The server removes the parameter, then creates it again.
  bool removed = false, back = false, bound_again = false, after = false;
  if(ok)
  {
    server.get_root_node().find_child("bench")->remove_child("p0");
    removed = poll_until(
        [&] { return ossia::net::find_node(croot, "/bench/p0") == nullptr; });
    leaf = make_leaf();
    back = poll_until(
        [&] { return ossia::net::find_node(croot, "/bench/p0") != nullptr; });
    bound_again = poll_until([&] { return b.bound_count("remote:/bench/p0") == 1; });
    after = received(*leaf, ossia::value(0.75f));
  }

  client.reset();
  ossia::net::stop_network_context(*ctx);
  net.join();
  REQUIRE(ok);
  REQUIRE(reloaded);
  REQUIRE(reload_bound);
  REQUIRE(after_reload);
  REQUIRE(removed);
  REQUIRE(back);
  REQUIRE(bound_again);
  REQUIRE(after);
}

// The sets of a branch during a tick leave at flush in OSC bundles: through a
// client's mirror to a bound server, and through a served device to its
// clients. Only what changes leaves; with bundles off, one message per value.
TEST_CASE("v8connector_bundles", "v8connector_bundles")
{
  const int n = 3000;
  sink_server server{n};
  auto client = std::make_shared<ossia::net::generic_device>(
      std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
          server.url(), free_port(false)),
      "remote");
  REQUIRE(client->get_protocol().update(client->get_root_node()));
  ossia::net::generic_device max_dev{
      std::make_unique<ossia::net::multiplex_protocol>(), "max"};
  branch b;
  b.set_resolution_context({&max_dev.get_root_node()}, {&max_dev, client.get()});
  std::vector<std::string> addresses;
  for(int i = 0; i < n; i++)
  {
    addresses.push_back("remote:/load/p" + std::to_string(i));
    REQUIRE(b.bind(addresses.back()).empty());
  }
  REQUIRE(b.bound_count(addresses[2999]) == 1);
  b.drain();
  server.sink.clear();

  auto tick = [&](auto value_of) {
    for(int i = 0; i < n; i++)
      REQUIRE(b.set(addresses[i], {value_of(i)}).empty());
  };
  auto settle = [] { std::this_thread::sleep_for(std::chrono::milliseconds(200)); };

  SECTION("3000 sets of one tick leave at flush, in datagrams of at most 1400 bytes")
  {
    b.set_bundle(true);
    tick([](int i) { return float(i); });
    settle();
    {
      std::lock_guard lock{server.sink.mutex};
      REQUIRE(server.sink.messages.empty());
    }
    REQUIRE(b.flush() == std::size_t(n));
    REQUIRE(server.sink.wait_messages(n, 3000));
    std::lock_guard lock{server.sink.mutex};
    INFO("datagrams " << server.sink.datagrams.size());
    REQUIRE(server.sink.messages.size() == std::size_t(n));
    for(auto size : server.sink.datagrams)
      REQUIRE(size <= 1400);
    REQUIRE(server.sink.datagrams.size() < std::size_t(n) / 20);
  }

  SECTION("only what changes leaves, as the repetition filter says")
  {
    for(int i = 0; i < n; i++)
      ossia::net::find_node(client->get_root_node(), "/load/p" + std::to_string(i))
          ->get_parameter()
          ->set_repetition_filter(ossia::repetition_filter::ON);
    b.set_bundle(true);
    // every value differs from the one the parameter holds
    tick([](int i) { return float(i) + 1000.f; });
    REQUIRE(b.flush() == std::size_t(n));
    REQUIRE(server.sink.wait_messages(n, 3000));
    server.sink.clear();
    // 1 % changes
    tick([](int i) { return i % 100 == 0 ? float(i) + 1000.5f : float(i) + 1000.f; });
    REQUIRE(b.flush() == std::size_t(n / 100));
    REQUIRE(server.sink.wait_messages(n / 100, 3000));
    settle();
    std::lock_guard lock{server.sink.mutex};
    REQUIRE(server.sink.messages.size() == std::size_t(n / 100));
    REQUIRE(server.sink.datagrams.size() == 1);
  }

  SECTION("bundles off: one datagram per value, as before")
  {
    b.set_bundle(false);
    tick([](int i) { return float(i); });
    REQUIRE(b.flush() == 0);
    REQUIRE(server.sink.wait_messages(n, 5000));
    std::lock_guard lock{server.sink.mutex};
    REQUIRE(server.sink.datagrams.size() == std::size_t(n));
  }

  client.reset();
}

// Leaves declared under a served device (ossia.device): the bundle leaves
// through the OSCQuery server, which sends what it is given as one datagram.
// The branch hands it groups of at most 1400 bytes.
TEST_CASE("v8connector_bundles_served_leaves", "v8connector_bundles_served_leaves")
{
  const int n = 3000;
  sink_server server{0};
  udp_sink client_sink{free_port(false)};
  raw_client client{server.url()};
  REQUIRE(client.ws.connected());
  client.stream_to(client_sink.socket.local_endpoint().port());
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  branch b;
  auto& root = server.dev->get_root_node();
  b.set_node_factory([&]() -> ossia::net::node_base* {
    return &ossia::net::create_node(root, "/synth");
  });
  b.set_resolution_context({&root}, {server.dev.get()});
  for(int i = 0; i < n; i++)
    REQUIRE(b.declare(
                 "p" + std::to_string(i),
                 attrs({{"type", {S("float")}}, {"min", {0}}, {"max", {1000}}}))
                .empty());
  b.drain();
  b.set_bundle(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  // Once the leaves exist: a LISTEN on an unknown address listens to nothing
  client.listen_load(n, "/synth/p");
  client_sink.clear();

  for(int i = 0; i < n; i++)
    REQUIRE(b.set("p" + std::to_string(i), {float(i)}).empty());
  REQUIRE(b.flush() == std::size_t(n));
  const bool all = client_sink.wait_messages(n, 3000);
  std::lock_guard lock{client_sink.mutex};
  std::size_t largest = 0;
  for(auto size : client_sink.datagrams)
    largest = std::max(largest, size);
  INFO(
      "datagrams " << client_sink.datagrams.size() << ", messages "
                   << client_sink.messages.size() << ", largest " << largest);
  REQUIRE(all);
  REQUIRE(client_sink.messages.size() == std::size_t(n));
  REQUIRE(largest <= 1400);
  REQUIRE(client_sink.datagrams.size() < std::size_t(n) / 20);
}

namespace
{
// A server as ossia.device runs it in Max: no thread of its own, its network
// context polled by the main thread between other work.
struct main_thread_server
{
  ossia::net::network_context_ptr ctx = ossia::net::create_network_context();
  std::unique_ptr<ossia::net::generic_device> dev;
  uint16_t ws = free_port(true);
  explicit main_thread_server(int leaves)
  {
    auto mp = std::make_unique<ossia::net::multiplex_protocol>();
    auto* multiplex = mp.get();
    dev = std::make_unique<ossia::net::generic_device>(std::move(mp), "load_demo");
    for(int i = 0; i < leaves; i++)
      ossia::try_setup_parameter(
          "float",
          ossia::net::create_node(dev->get_root_node(), "/load/p" + std::to_string(i)));
    multiplex->expose_to(
        std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(
            ctx, free_port(false), ws, false));
  }
  void poll()
  {
    ctx->context.poll();
    ctx->context.restart();
  }
  std::string url() const { return "ws://127.0.0.1:" + std::to_string(ws); }
};
}
#include <spdlog/sinks/callback_sink.h>

namespace
{
struct burst_result
{
  std::size_t bound = 0;
  long long total_ms = 0;
  long long longest_us = 0;
  long long sum_us = 0;
  int polls = 0;
  int http_errors = 0;
};

// A client connected before the server declares its n leaves, with n binds
// waiting (the load example in Max): each leaf arrives alone (PATH_ADDED,
// then ATTRIBUTES_CHANGED and an HTTP request for its value). The main
// thread polls the server, the client's clock its mirror, until the last
// leaf is bound.
burst_result run_burst(int n)
{
  std::atomic_int http_errors{0};
  auto sink = std::make_shared<spdlog::sinks::callback_sink_mt>(
      [&](const spdlog::details::log_msg& m) {
    if(std::string_view(m.payload.data(), m.payload.size()).find("HTTP Error")
       != std::string_view::npos)
      http_errors++;
  });
  ossia::logger().sinks().push_back(sink);

  burst_result r;
  main_thread_server server{0};
  for(int i = 0; i < 10; i++)
  {
    server.poll();
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
  // The client connects from another thread: the server answers only while
  // this thread polls it.
  std::unique_ptr<ossia::net::generic_device> client;
  std::atomic_bool done{false};
  std::thread connect([&] {
    try
    {
      auto dev = std::make_unique<ossia::net::generic_device>(
          std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
              server.url(), free_port(false)),
          "load_demo");
      if(dev->get_protocol().update(dev->get_root_node()))
        client = std::move(dev);
    }
    catch(...)
    {
    }
    done = true;
  });
  for(int i = 0; i < 10000 && !done; i++)
  {
    server.poll();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  connect.join();
  REQUIRE(client);
  auto* mirror = polled_mirror(*client);

  branch b;
  b.set_resolution_context({&client->get_root_node()}, {client.get()});
  for(int i = 0; i < n; i++)
    REQUIRE(b.bind("load/p" + std::to_string(i)).empty());

  // the emitter declares its leaves after the connection
  for(int i = 0; i < n; i++)
    ossia::try_setup_parameter(
        "float", ossia::net::create_node(
                     server.dev->get_root_node(), "/load/p" + std::to_string(i)));

  const auto t0 = std::chrono::steady_clock::now();
  const auto deadline = t0 + std::chrono::seconds(60);
  while(b.bound_count("load/p" + std::to_string(n - 1)) == 0
        && std::chrono::steady_clock::now() < deadline)
  {
    server.poll();
    const auto a = std::chrono::steady_clock::now();
    mirror->run_commands();
    const auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::steady_clock::now() - a)
                        .count();
    r.longest_us = std::max<long long>(r.longest_us, us);
    r.sum_us += us;
    r.polls++;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  r.total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now() - t0)
                   .count();
  for(int i = 0; i < n; i++)
    r.bound += b.bound_count("load/p" + std::to_string(i));
  client.reset();
  auto& sinks = ossia::logger().sinks();
  sinks.erase(std::remove(sinks.begin(), sinks.end(), sink), sinks.end());
  r.http_errors = http_errors;
  return r;
}
}

// The poll must not stall as the leaves come: before, every creation resolved
// every waiting binding again, 15 s in one run_commands for 3000 leaves, while
// the server of the same Max, polled by that thread, let the HTTP requests
// time out. The time of a poll depends on the machine, its growth with the
// number of leaves much less: the test compares the longest run_commands for
// 3000 leaves and for 1000, about 3 if the cost is linear, 9 if quadratic.
// Measured ratios, three runs each: on a Mac (M1 Max, Release), 3.3 to 3.9
// at a load of 6 to 13 and 4.8 to 6.8 at a load of 85 with the selective
// resolution, 15.1 to 16.1 and 10.7 to 16.2 resolving every binding again;
// on the GitHub runners, 2.1 to 3.1 (Linux), 1.9 to 6.0 (Windows) and 2.3 to
// 5.4 (macOS).
TEST_CASE(
    "v8connector_binds_follow_a_burst_of_leaves",
    "v8connector_binds_follow_a_burst_of_leaves")
{
  const auto small = run_burst(1000);
  const auto large = run_burst(3000);
  const double ratio
      = double(large.longest_us) / double(std::max<long long>(small.longest_us, 1));

  INFO(
      "1000 leaves: bound " << small.bound << " in " << small.total_ms
                            << " ms, longest run_commands " << small.longest_us
                            << " us, sum " << small.sum_us << " us over " << small.polls
                            << " polls, HTTP errors " << small.http_errors);
  INFO(
      "3000 leaves: bound " << large.bound << " in " << large.total_ms
                            << " ms, longest run_commands " << large.longest_us
                            << " us, sum " << large.sum_us << " us over " << large.polls
                            << " polls, HTTP errors " << large.http_errors);
  INFO("ratio of the longest run_commands " << ratio);
  REQUIRE(small.bound == 1000);
  REQUIRE(large.bound == 3000);
  REQUIRE(ratio < 9.);
  // The HTTP errors are shown, not asserted: their number depends on how long
  // a poll lasts against the timeouts of the system, a time in absolute
  // terms. The mirror asks the value of each new leaf by HTTP, one connection
  // per request and no limit on those in flight: up to 5600 for 3000 leaves
  // on the GitHub runners, while the server is polled by this thread only.
  // On Windows the connections beyond the listen backlog are refused
  // (WSAECONNREFUSED at connect, from 2900 in flight; none with a backlog of
  // 4000). On macOS in Debug the one run_commands of the burst lasts 17 to
  // 19 s and the connections time out (ETIMEDOUT at connect), also with a
  // backlog of 4096 asked. Linux had none. The binds are all made in every case.
}

// A connector bound through an ossia.client whose device changes while the
// old one lives on: the client first had the zeroconf listener's mirror of
// the device (connect by name), shared and kept alive by the listener, then
// its own device (connect oscquery <url>). Its connector's resolution context
// goes from the first to the second; the first is not destroyed, so nothing
// tells the connector to forget it (the client clears its device before it
// announces the removal). Expected: one target, the second device's, and a
// value set leaves through it only.
TEST_CASE(
    "v8connector_context_change_leaves_a_shared_device",
    "v8connector_context_change_leaves_a_shared_device")
{
  ossia::net::generic_device shared{
      std::make_unique<ossia::net::multiplex_protocol>(), "demo"};
  ossia::net::generic_device own{
      std::make_unique<ossia::net::multiplex_protocol>(), "demo"};
  ossia::net::generic_device max_dev{
      std::make_unique<ossia::net::multiplex_protocol>(), "max"};
  auto leaf = [](ossia::net::generic_device& d) {
    auto* p = ossia::try_setup_parameter(
        "float", ossia::net::create_node(d.get_root_node(), "/image/scale"));
    p->set_domain(ossia::make_domain(0.f, 1.f));
    p->push_value(0.f);
    return p;
  };
  auto* in_shared = leaf(shared);
  auto* in_own = leaf(own);

  branch b;
  b.attach(ossia::net::create_node(max_dev.get_root_node(), "/demo_ui"));
  b.set_resolution_context({&shared.get_root_node()}, {&shared, &max_dev});
  REQUIRE(b.bind("/image/scale").empty());
  REQUIRE(b.bound_count("/image/scale") == 1);

  // the client leaves the shared mirror for its own device; the shared one lives on
  b.set_resolution_context({&own.get_root_node()}, {&own, &max_dev});
  const auto count = b.bound_count("/image/scale");
  REQUIRE(b.set("/image/scale", {0.75f}).empty());
  INFO("step: one target after the change, on the client's own device");
  REQUIRE(count == 1);
  REQUIRE(ossia::convert<float>(in_shared->value()) == 0.f);
  REQUIRE(ossia::convert<float>(in_own->value()) == 0.75f);

  // a value of the shared device no longer reaches the branch
  b.drain();
  in_shared->push_value(0.25f);
  bool from_shared = false;
  for(auto& in : b.drain())
    from_shared = from_shared || in.address == "/image/scale";
  REQUIRE(!from_shared);
}
