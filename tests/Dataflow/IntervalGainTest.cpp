// The gain and pan of an interval's audio outlet scale what its parent receives,
// once.

#include <ossia/detail/config.hpp>

#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph/graph.hpp>
#include <ossia/dataflow/graph_edge_helpers.hpp>
#include <ossia/dataflow/bench_state.hpp>
#include <ossia/dataflow/nodes/forward_node.hpp>
#include <ossia/dataflow/telemetry.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>

namespace
{
constexpr int frames = 16;

class constant_source final : public ossia::nonowning_graph_node
{
public:
  explicit constant_source(std::vector<double> levels)
      : m_levels{std::move(levels)}
  {
    m_outlets.push_back(&audio_out);
  }

  std::string label() const noexcept override { return "constant_source"; }

  void run(const ossia::token_request&, ossia::exec_state_facade) noexcept override
  {
    auto& o = *audio_out;
    o.set_channels(m_levels.size());
    for(std::size_t c = 0; c < m_levels.size(); c++)
      o.channel(c).assign(frames, m_levels[c]);
  }

  ossia::audio_outlet audio_out;

private:
  std::vector<double> m_levels;
};

// source -> interval -> parent, as score builds a process in an interval whose
// outlet propagates to the enclosing scenario.
struct Chain
{
  std::shared_ptr<ossia::graph_interface> g;
  ossia::execution_state e;
  std::shared_ptr<constant_source> source;
  std::shared_ptr<ossia::nodes::interval> interval
      = std::make_shared<ossia::nodes::interval>();
  std::shared_ptr<ossia::nodes::forward_node> parent
      = std::make_shared<ossia::nodes::forward_node>();

  explicit Chain(std::vector<double> levels, const ossia::graph_setup_options& opt = {})
      : g{ossia::make_graph(opt)}
      , source{std::make_shared<constant_source>(std::move(levels))}
  {
    g->add_node(source);
    g->add_node(interval);
    g->add_node(parent);
    g->connect(ossia::make_glutton_edge(*g, 0, 0, source, interval));
    g->connect(ossia::make_glutton_edge(*g, 0, 0, interval, parent));
  }

  const ossia::audio_vector& tick()
  {
    const ossia::simple_token_request tk{
        ossia::time_value{0}, ossia::time_value{frames}};
    for(auto& n :
        {ossia::node_ptr{source}, ossia::node_ptr{interval}, ossia::node_ptr{parent}})
      n->request(tk);

    e.bufferSize = frames;
    e.begin_tick();
    g->state(e);
    e.commit();
    return parent->audio_out.data.get();
  }
};

void check_levels(const ossia::audio_vector& out, const std::vector<double>& expected)
{
  REQUIRE(out.size() == expected.size());
  for(std::size_t c = 0; c < expected.size(); c++)
  {
    REQUIRE(out[c].size() == std::size_t(frames));
    for(double s : out[c])
      CHECK(s == Catch::Approx(expected[c]));
  }
}
}

TEST_CASE("An interval's gain reaches its parent once", "[dataflow][interval][gain]")
{
  SECTION("mono")
  {
    Chain c{{1.}};
    c.interval->audio_out.gain = 0.5;
    check_levels(c.tick(), {0.5});
  }

  SECTION("stereo")
  {
    Chain c{{1., 1.}};
    c.interval->audio_out.gain = 0.5;
    check_levels(c.tick(), {0.5, 0.5});
  }

  SECTION("over several ticks")
  {
    Chain c{{1., 1.}};
    c.interval->audio_out.gain = 0.5;
    c.tick();
    check_levels(c.tick(), {0.5, 0.5});
  }
}

TEST_CASE("An interval's pan reaches its parent once", "[dataflow][interval][pan]")
{
  Chain c{{1., 1.}};
  c.interval->audio_out.pan = ossia::pan_weight{0.5, 1.};
  check_levels(c.tick(), {0.5, 1.});
}

TEST_CASE("An automated gain applies in the tick it arrives", "[dataflow][interval][gain]")
{
  Chain c{{1., 1.}};
  c.interval->audio_out.gain_inlet.data.write_value(0.25f, 0);
  check_levels(c.tick(), {0.25, 0.25});
}

TEST_CASE("A unity interval passes its input through", "[dataflow][interval][gain]")
{
  Chain c{{0.25, -0.75}};
  check_levels(c.tick(), {0.25, -0.75});
}

TEST_CASE("A gain change ramps over one buffer", "[dataflow][interval][gain]")
{
  Chain c{{1.}};
  check_levels(c.tick(), {1.});

  c.interval->audio_out.gain = 0.;
  const auto& out = c.tick();
  REQUIRE(out.size() == 1);
  for(int i = 1; i < frames; i++)
    CHECK(out[0][i] < out[0][i - 1]);
  CHECK(out[0][frames - 1] == Catch::Approx(0.));

  check_levels(c.tick(), {0.});
}

TEST_CASE("A pan change ramps over one buffer", "[dataflow][interval][pan]")
{
  Chain c{{1., 1.}};
  c.tick();

  c.interval->audio_out.pan = ossia::pan_weight{0., 1.};
  const auto& out = c.tick();
  REQUIRE(out.size() == 2);
  CHECK(out[0][0] > 0.);
  CHECK(out[0][frames - 1] == Catch::Approx(0.));
  for(double s : out[1])
    CHECK(s == Catch::Approx(1.));
}

TEST_CASE("Channels past the pan weights keep unity", "[dataflow][interval][pan]")
{
  Chain c{{1., 1., 1., 1.}};
  c.interval->audio_out.pan = ossia::pan_weight{0.5, 0.5};
  c.tick();
  check_levels(c.tick(), {0.5, 0.5, 1., 1.});
  CHECK(c.interval->audio_out.pan.size() == 2);
}

TEST_CASE("A benchmarked graph times the nodes that have a tap", "[dataflow][bench]")
{
  auto bench = std::make_shared<ossia::bench_state>();
  bench->measure = true;
  ossia::graph_setup_options opt;
  opt.bench = bench;

  Chain c{{1.}, opt};

  auto tap = std::make_shared<ossia::telemetry::bench_tap>();
  c.source->bench_tap = tap;

  check_levels(c.tick(), {1.});
  CHECK(tap->pending.runs == 1);
  CHECK(!c.interval->bench_tap);

  bench->measure = false;
  check_levels(c.tick(), {1.});
  CHECK(tap->pending.runs == 1);
}
