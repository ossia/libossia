
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/editor/scenario/time_interval.hpp>

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
