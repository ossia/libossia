
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/editor/scenario/time_interval.hpp>

#include <map>
#include <utility>
using namespace ossia;
using namespace std::placeholders;

TEST_CASE("get_quantification_date_0", "get_quantification_date_0")
{
  ossia::token_request req{0_tv, 100_tv, 15000_tv, 0_tv, 1., {4, 4}, ossia::root_tempo};
  REQUIRE(req.get_quantification_date(0.25));
  REQUIRE(req.get_quantification_date(0.25) == 0_tv);
  REQUIRE(req.get_quantification_date(1.));
  REQUIRE(req.get_quantification_date(1.) == 0_tv);
  REQUIRE(req.get_quantification_date(2.));
  REQUIRE(req.get_quantification_date(2.) == 0_tv);
  REQUIRE(req.get_quantification_date(100.));
  REQUIRE(req.get_quantification_date(100.) == 0_tv);
}
TEST_CASE("get_quantification_date_bar_rate_1", "get_quantification_date_bar_rate_1")
{
  ossia::token_request req{100_tv, 200_tv, 15000_tv,         0_tv,
                           1.,     {4, 4}, ossia::root_tempo};

  GIVEN("quantization at the beginning of a bar")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 4.0; // Start of bar 2 (4 quarters = 1 bar in 4/4)
    req.musical_end_last_bar = 4.0;
    req.musical_end_position = 4.1;
    REQUIRE(req.get_quantification_date(1.));
    REQUIRE(req.get_quantification_date(1.) == 100_tv);
  }

  GIVEN("quantization crossing a bar")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 3.9;
    req.musical_end_last_bar = 4.0;
    req.musical_end_position = 4.1;
    // Should quantize at position 4.0 (bar 2)
    // ratio = (4.0 - 3.9) / (4.1 - 3.9) = 0.1 / 0.2 = 0.5
    REQUIRE(req.get_quantification_date(1.));
    REQUIRE(req.get_quantification_date(1.) == 150_tv);
  }

  GIVEN("quantization crossing bars")
  {
    for(int i = 1; i < 1000000; i++)
    {
      INFO(i);
      req.musical_start_last_signature = 0.;
      req.musical_start_last_bar = i * 4.;
      req.musical_start_position = ((i + 1) * 4.) - 0.1;
      req.musical_end_last_bar = ((i + 1) * 4.);
      req.musical_end_position = ((i + 1) * 4.) + 0.1;
      auto res = req.get_quantification_date(1.);
      REQUIRE(res);
      REQUIRE(res >= 149_tv);
      REQUIRE(res <= 151_tv);
    }
  }

  GIVEN("quantization missing a bar")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 4.0;
    req.musical_start_position = 4.1;
    req.musical_end_last_bar = 4.0;
    req.musical_end_position = 7.9; // Doesn't reach bar 3 at position 8.0
    REQUIRE(!req.get_quantification_date(1.));
  }

  GIVEN("quantization at the start of the song")
  {
    ossia::token_request req2{0_tv, 100_tv, 15000_tv,         0_tv,
                              1.,   {4, 4}, ossia::root_tempo};
    req2.musical_start_last_signature = 0.;
    req2.musical_start_last_bar = 0.;
    req2.musical_start_position = 0.0;
    req2.musical_end_last_bar = 0.0;
    req2.musical_end_position = 0.1;
    REQUIRE(req2.get_quantification_date(1.));
    REQUIRE(req2.get_quantification_date(1.) == 0_tv);
  }
}

TEST_CASE("get_quantification_date_bar_rate_0_5", "get_quantification_date_bar_rate_0_5")
{
  ossia::token_request req{100_tv, 200_tv, 15000_tv,         0_tv,
                           1.,     {4, 4}, ossia::root_tempo};

  GIVEN("quantization at the beginning of every 2 bars")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 8.0;
    req.musical_start_position = 8.0; // Start of bar 3 (which is 2 bars after bar 1)
    req.musical_end_last_bar = 8.0;
    req.musical_end_position = 8.1;
    REQUIRE(req.get_quantification_date(0.5));
    REQUIRE(req.get_quantification_date(0.5) == 100_tv);
  }

  GIVEN("quantization crossing 2-bar boundary")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 7.9;
    req.musical_end_last_bar = 8.0;
    req.musical_end_position = 8.1;
    // Should quantize at position 8.0 (2 bars * 4 quarters)
    // ratio = (8.0 - 7.9) / (8.1 - 7.9) = 0.1 / 0.2 = 0.5
    REQUIRE(req.get_quantification_date(0.5));
    REQUIRE(req.get_quantification_date(0.5) == 150_tv);
  }

  GIVEN("quantization missing the 2-bar mark")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 4.1; // Just after bar 2
    req.musical_end_last_bar = 4.0;
    req.musical_end_position = 7.9; // Just before bar 3 (position 8.0)
    REQUIRE(!req.get_quantification_date(0.5));
  }

  GIVEN("quantization at odd bar (should not trigger)")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 3.9;
    req.musical_end_last_bar = 4.0;
    req.musical_end_position = 4.1;
    // This crosses bar 2 (position 4.0), but for rate=0.5 we only quantize every 2 bars
    REQUIRE(!req.get_quantification_date(0.5));
  }

  GIVEN("quantization at the start of the song")
  {
    ossia::token_request req2{0_tv, 100_tv, 15000_tv,         0_tv,
                              1.,   {4, 4}, ossia::root_tempo};
    req2.musical_start_last_signature = 0.;
    req2.musical_start_last_bar = 0.;
    req2.musical_start_position = 0.0;
    req2.musical_end_last_bar = 0.0;
    req2.musical_end_position = 0.1;
    // Position 0 is a multiple of any bar count
    REQUIRE(req2.get_quantification_date(0.5));
    REQUIRE(req2.get_quantification_date(0.5) == 0_tv);
  }
}
TEST_CASE("get_quantification_date_some", "get_quantification_date_some")
{
  ossia::token_request req{100_tv, 200_tv, 15000_tv,         0_tv,
                           1.,     {4, 4}, ossia::root_tempo};
  GIVEN("quantization in the middle")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 0.95;
    req.musical_end_last_bar = 0.;
    req.musical_end_position = 1.05;
    REQUIRE(req.get_quantification_date(4.));
    REQUIRE(req.get_quantification_date(4.) == 150_tv);
  }
  GIVEN("quantization before the beginning 1")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 0.99;
    req.musical_end_last_bar = 0.;
    req.musical_end_position = 1.05;
    REQUIRE(req.get_quantification_date(4.));
    REQUIRE(req.get_quantification_date(4.) == 116_tv);
  }
  GIVEN("quantization before the beginning 2")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 0.999;
    req.musical_end_last_bar = 0.;
    req.musical_end_position = 1.05;
    REQUIRE(req.get_quantification_date(4.));
    REQUIRE(req.get_quantification_date(4.) == 101_tv);
  }
  GIVEN("quantization before the beginning 3")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 0.999999999;
    req.musical_end_last_bar = 0.;
    req.musical_end_position = 1.05;
    REQUIRE(req.get_quantification_date(4.));
    REQUIRE(req.get_quantification_date(4.) == 100_tv);
  }
  GIVEN("quantization before the beginning 4")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = std::nextafter(1., 0.);
    req.musical_end_last_bar = 0.;
    req.musical_end_position = 1.05;
    REQUIRE(req.get_quantification_date(4.));
    REQUIRE(req.get_quantification_date(4.) == 100_tv);
  }
  GIVEN("quantization at the beginning")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 1.;
    req.musical_end_last_bar = 0.;
    req.musical_end_position = 1.10;
    REQUIRE(req.get_quantification_date(4.));
    REQUIRE(req.get_quantification_date(4.) == 100_tv);
  }

  GIVEN("quantization before the end")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 1.90;
    req.musical_end_last_bar = 0.;
    req.musical_end_position = 1.999999999999999;
    REQUIRE(!req.get_quantification_date(4.));
  }
  GIVEN("quantization at the end")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = 1.90;
    req.musical_end_last_bar = 0.;
    req.musical_end_position = 2.;
    req.get_quantification_date(4.);
    REQUIRE(!req.get_quantification_date(4.));
    //REQUIRE(!req.get_quantification_date(4.) == 200_tv);
  }
  GIVEN("quantization after the end")
  {
    req.musical_start_last_signature = 0.;
    req.musical_start_last_bar = 0.;
    req.musical_start_position = std::nextafter(2., 3.);
    REQUIRE(req.musical_start_position != 2.);
    req.musical_end_last_bar = 0.;
    req.musical_end_position = 2.1;
    REQUIRE(!req.get_quantification_date(4.));
  }
}
TEST_CASE("test_quant", "test_quant")
{
  std::shared_ptr<ossia::time_sync> start_node{std::make_shared<ossia::time_sync>()};
  std::shared_ptr<ossia::time_sync> end_node{std::make_shared<ossia::time_sync>()};

  std::shared_ptr<ossia::time_event> start_event{std::make_shared<ossia::time_event>(
      ossia::time_event::exec_callback{}, *start_node,
      ossia::expressions::make_expression_true())};
  std::shared_ptr<ossia::time_event> end_event{std::make_shared<ossia::time_event>(
      ossia::time_event::exec_callback{}, *end_node,
      ossia::expressions::make_expression_true())};

  std::shared_ptr<ossia::time_interval> interval{ossia::time_interval::create(
      {}, *start_event, *end_event, ossia::time_value{15000}, ossia::time_value{15000},
      ossia::time_value{15000})};

  //
}

// Every musical grid point must be reported by exactly one tick: the tick
// owns [prev_date; date[ and the next one starts where it ended. A grid point
// sitting musically on the very end of a tick used to fire twice - once at
// the tick's last flick, because truncating its date to a whole flick pulled
// it inside, and once more at the next tick's first flick. A quantized
// trigger or a step sequencer then executed the same point twice, one sample
// apart. This sweep reproduces the exact tick streams that exposed it.
TEST_CASE("quantification_points_fire_exactly_once", "[quantification]")
{
  constexpr double Q = 352800000.0; // flicks per quarter through model time
  constexpr double FPS48 = 705600000.0 / 48000.0;

  for(double rate : {1., 1.5, 2., 3., 4., 8.})
  {
    for(double speed : {1.0, 0.5, 1.37, 2.0})
    {
      for(int L : {128, 512})
      {
        // Count the reported points per musical position, over a long run of
        // consecutive ticks advanced with floor + carried residue like
        // time_interval::take_step.
        std::map<long long, int> seen;
        double residue = 0.;
        int64_t d = 0;
        double last_end = 0.;
        for(int t = 0; t < 6000; t++)
        {
          const double exact = L * FPS48 * speed + residue;
          const double step = std::floor(exact);
          residue = exact - step;
          const int64_t nd = d + int64_t(step);

          ossia::token_request tok;
          tok.prev_date = ossia::time_value{d};
          tok.date = ossia::time_value{nd};
          tok.speed = 1.;
          tok.tempo = 120.;
          tok.signature = ossia::time_signature{4, 4};
          tok.start_sample = 0;
          tok.length_sample = L;
          tok.musical_start_last_signature = 0.;
          tok.musical_start_position = d / Q;
          tok.musical_start_last_bar
              = std::floor(tok.musical_start_position / 4.) * 4.;
          tok.musical_end_position = nd / Q;
          tok.musical_end_last_bar = std::floor(tok.musical_end_position / 4.) * 4.;

          for(const auto& p : tok.get_quantification_dates(rate))
          {
            // Key each point by its musical position on an eighth-of-a-quarter
            // lattice: reporting noise is < 1e-6 quarters, distinct points of
            // these rates are >= 1/3 quarter apart.
            const double frac = double(p.date.impl - d) / double(nd - d);
            const double mus = tok.musical_start_position
                               + frac
                                     * (tok.musical_end_position
                                        - tok.musical_start_position);
            seen[llround(mus * 8.)]++;
          }
          last_end = nd / Q;
          d = nd;
        }

        for(const auto& [key, count] : seen)
        {
          // Ignore the last partial bar: those points may legitimately still
          // be waiting for the tick that owns them.
          if(key / 8. < last_end - 4.)
          {
            INFO(
                "rate " << rate << " speed " << speed << " L " << L << " point "
                        << key / 8. << " fired " << count << " times");
            REQUIRE(count == 1);
          }
        }
      }
    }
  }
}
