
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/editor/scenario/time_interval.hpp>

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
