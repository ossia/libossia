// A change run between two ticks runs on the audio thread, without the audio
// missing a tick, and runs directly when the engine does not call back.

#include <ossia/detail/config.hpp>

#include <ossia/audio/dummy_protocol.hpp>

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <thread>

namespace
{
bool wait_for(std::atomic_int& ticks, int n)
{
  const int start = ticks;
  for(int i = 0; i < 500 && ticks < start + n; i++)
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  return ticks - start >= n;
}
}

TEST_CASE("A change runs between two ticks of a running engine", "[audio][engine]")
{
  ossia::dummy_engine engine{48000, 64, 0, 2};
  std::atomic_int ticks{};
  std::atomic_bool in_tick{};
  std::atomic_bool overlapped{};
  engine.set_tick([&](const ossia::audio_tick_state&) {
    in_tick = true;
    ticks++;
    in_tick = false;
  });
  REQUIRE(wait_for(ticks, 3));

  for(int i = 0; i < 20; i++)
  {
    std::thread::id ran_on;
    engine.run_between_ticks([&] {
      overlapped = overlapped || in_tick;
      ran_on = std::this_thread::get_id();
    });
    CHECK(ran_on != std::this_thread::get_id());
  }
  CHECK(!overlapped);

  // The ticks kept coming meanwhile.
  CHECK(wait_for(ticks, 3));
  engine.stop();
}

TEST_CASE("A change runs directly when the engine does not run", "[audio][engine]")
{
  ossia::dummy_engine engine{48000, 64, 0, 2};
  engine.stop();
  bool ran = false;
  engine.run_between_ticks([&] { ran = true; });
  CHECK(ran);
}
