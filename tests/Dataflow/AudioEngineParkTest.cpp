// While an engine is parked, its callback does not run the tick.

#include <ossia/detail/config.hpp>

#include <ossia/audio/dummy_protocol.hpp>

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <thread>

TEST_CASE("A parked engine skips its tick until released", "[audio][engine]")
{
  ossia::dummy_engine engine{48000, 64, 0, 2};
  std::atomic_int ticks{};
  engine.set_tick([&](const ossia::audio_tick_state&) { ticks++; });

  auto wait_for_ticks = [&](int n) {
    const int start = ticks;
    for(int i = 0; i < 500 && ticks < start + n; i++)
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
    return ticks - start >= n;
  };
  REQUIRE(wait_for_ticks(3));

  int during{};
  engine.run_parked([&] {
    const int before = ticks;
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    during = ticks - before;
  });
  CHECK(during == 0);

  CHECK(wait_for_ticks(3));
  engine.stop();
}
