#include <ossia/detail/config.hpp>
#include <iterator>
#include <ossia/dataflow/token_request.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../Editor/TestUtils.hpp"
TEST_CASE ("test_loops_integer", "test_loops_integer")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 0_tv;
  r.date = 100_tv;

  std::vector<ossia::token_request> vec;
  r.loop(0_tv, 10_tv, [&] (auto& req) { vec.push_back(req); }, [] (const auto&){ });

  std::vector<ossia::token_request> expected{
    ossia::simple_token_request{0_tv, 10_tv, 0_tv},
    ossia::simple_token_request{0_tv, 10_tv, 10_tv},
    ossia::simple_token_request{0_tv, 10_tv, 20_tv},
    ossia::simple_token_request{0_tv, 10_tv, 30_tv},
    ossia::simple_token_request{0_tv, 10_tv, 40_tv},
    ossia::simple_token_request{0_tv, 10_tv, 50_tv},
    ossia::simple_token_request{0_tv, 10_tv, 60_tv},
    ossia::simple_token_request{0_tv, 10_tv, 70_tv},
    ossia::simple_token_request{0_tv, 10_tv, 80_tv},
    ossia::simple_token_request{0_tv, 10_tv, 90_tv}
  };

  REQUIRE(expected == vec);
}

TEST_CASE ("test_loops_fract", "test_loops_fract")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 0_tv;
  r.date = 97_tv;

  std::vector<ossia::token_request> vec;
  r.loop(0_tv, 10_tv, [&] (auto& req) { vec.push_back(req); }, [](const auto&) { });

  std::vector<ossia::token_request> expected{
    ossia::simple_token_request{0_tv, 10_tv, 0_tv},
    ossia::simple_token_request{0_tv, 10_tv, 10_tv},
    ossia::simple_token_request{0_tv, 10_tv, 20_tv},
    ossia::simple_token_request{0_tv, 10_tv, 30_tv},
    ossia::simple_token_request{0_tv, 10_tv, 40_tv},
    ossia::simple_token_request{0_tv, 10_tv, 50_tv},
    ossia::simple_token_request{0_tv, 10_tv, 60_tv},
    ossia::simple_token_request{0_tv, 10_tv, 70_tv},
    ossia::simple_token_request{0_tv, 10_tv, 80_tv},
    ossia::simple_token_request{0_tv, 7_tv, 90_tv}
  };

  REQUIRE(expected == vec);
}

TEST_CASE ("test_loops_offset", "test_loops_offset")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 2_tv;
  r.date = 12_tv;

  std::vector<ossia::token_request> vec;
  r.loop(0_tv, 5_tv, [&] (auto& req) { vec.push_back(req); }, [] (const auto&){ });

  std::vector<ossia::token_request> expected{
    ossia::simple_token_request{2_tv, 5_tv, 0_tv},
    ossia::simple_token_request{0_tv, 5_tv, 3_tv},
    ossia::simple_token_request{0_tv, 2_tv, 8_tv},
  };

  REQUIRE(expected == vec);
}

TEST_CASE ("test_loops_smaller", "test_loops_smaller")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 0_tv;
  r.date = 15_tv;

  std::vector<ossia::token_request> vec;
  r.loop(0_tv, 20_tv, [&] (auto& req) { vec.push_back(req); }, [](const auto&) { });

  std::vector<ossia::token_request> expected{
    ossia::simple_token_request{0_tv, 15_tv, 0_tv}
  };

  REQUIRE(expected == vec);
}

TEST_CASE ("test_loops_mid", "test_loops_mid")
{
  using namespace ossia;

  token_request r;
  r.prev_date = 15_tv;
  r.date = 30_tv;

  std::vector<ossia::token_request> vec;
  r.loop(0_tv, 20_tv, [&] (auto& req) { vec.push_back(req); }, [] (const auto&){ });

  std::vector<ossia::token_request> expected{
    ossia::simple_token_request{15_tv, 20_tv, 0_tv},
    ossia::simple_token_request{0_tv, 10_tv, 5_tv}
  };

  REQUIRE(expected == vec);
}
