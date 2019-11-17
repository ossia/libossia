#include <ossia/detail/config.hpp>

#include <ossia/dataflow/graph/tick_methods.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE ("test_cuts", "test_cuts")
{
  using namespace ossia;

  ossia::flat_set<int64_t> cuts{3, 7, 9, 35};
  ossia::token_request_vec tokens;
  tokens.push_back(ossia::simple_token_request{0_tv, 5_tv});
  tokens.push_back(ossia::simple_token_request{5_tv, 10_tv});
  tokens.push_back(ossia::simple_token_request{10_tv, 15_tv});
  // split_score_tick<&ossia::execution_state::commit>::do_cuts(cuts, tokens, 0_tv);

  // from 0 to 3
  // from 3 to 5
  // from 5 to 7
  // from 7 to 9
  // from 9 to 10
  // from 10


}
