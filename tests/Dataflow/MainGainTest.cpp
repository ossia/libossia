// The gain of /out/main scales every hardware output once, after the tick has
// summed everything into them.

#include <ossia/detail/config.hpp>

#include <ossia/audio/audio_parameter.hpp>
#include <ossia/audio/audio_protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <array>
#include <functional>
#include <memory>

namespace
{
constexpr int frames = 8;

struct Card
{
  ossia::audio_protocol* proto{};
  ossia::net::generic_device dev;

  std::array<std::array<float, frames>, 2> out_buffers{};
  std::array<float*, 2> outputs{out_buffers[0].data(), out_buffers[1].data()};

  Card()
      : dev{[this] {
        auto p = std::make_unique<ossia::audio_protocol>();
        proto = p.get();
        return p;
      }(), "audio"}
  {
    proto->setup_tree(0, 2);
  }

  ossia::audio_tick_state state()
  {
    return {.inputs = nullptr, .outputs = outputs.data(), .n_in = 0, .n_out = 2,
            .frames = frames};
  }

  static ossia::audio_port constant(std::size_t channels, double v)
  {
    ossia::audio_port p;
    p.set_channels(channels);
    for(std::size_t c = 0; c < channels; c++)
      p.channel(c).assign(frames, v);
    return p;
  }

  // One audio callback: silence, the graph's writes, then the main gain.
  void tick(const std::function<void()>& writes)
  {
    auto st = state();
    proto->setup_buffers(st);
    writes();
    proto->apply_main_gain(st);
  }
};
}

TEST_CASE("/out/main scales the root's output once", "[audio][main][gain]")
{
  Card c;
  c.proto->main_audio_out->set_value(0.5f);

  // First callback ramps from unity; the second one is steady.
  auto write = [&] { c.proto->main_audio_out->push_value(Card::constant(2, 1.)); };
  c.tick(write);
  CHECK(c.out_buffers[0][frames - 1] == Catch::Approx(0.5));

  c.tick(write);
  for(auto& chan : c.out_buffers)
    for(float s : chan)
      CHECK(s == Catch::Approx(0.5));
}

TEST_CASE("/out/main scales what is sent straight to /out/N", "[audio][main][gain]")
{
  Card c;
  c.proto->main_audio_out->set_value(0.5f);
  c.proto->audio_outs[1]->set_value(0.5f);

  auto write = [&] {
    c.proto->main_audio_out->push_value(Card::constant(2, 1.));
    c.proto->audio_outs[1]->push_value(Card::constant(1, 1.));
  };
  c.tick(write);
  c.tick(write);

  // left: 1 * main; right: (1 + 1 * 0.5) * main
  for(float s : c.out_buffers[0])
    CHECK(s == Catch::Approx(0.5));
  for(float s : c.out_buffers[1])
    CHECK(s == Catch::Approx(0.75));
}

TEST_CASE("A main gain change ramps over one buffer", "[audio][main][gain]")
{
  Card c;
  auto write = [&] { c.proto->main_audio_out->push_value(Card::constant(1, 1.)); };
  c.tick(write);
  CHECK(c.out_buffers[0][0] == Catch::Approx(1.));

  c.proto->main_audio_out->set_value(0.f);
  c.tick(write);
  for(int i = 1; i < frames; i++)
    CHECK(c.out_buffers[0][i] < c.out_buffers[0][i - 1]);
  CHECK(c.out_buffers[0][frames - 1] == Catch::Approx(0.));

  c.tick(write);
  for(float s : c.out_buffers[0])
    CHECK(s == Catch::Approx(0.));
}
