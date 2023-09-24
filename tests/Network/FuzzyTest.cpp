// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/domain/domain_base.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include "include_catch.hpp"

using namespace ossia;

////////////////////////////////
// float domain, float value //
///////////////////////////////

static inline bool string_ends_with(const std::string& src, const std::string& suffix)
{
#if defined(__cpp_lib_starts_ends_with)
  return src.ends_with(suffix);
#else
  return src.size() >= suffix.size()
         && 0 == src.compare(src.size() - suffix.size(), suffix.size(), suffix);
#endif
}

TEST_CASE("Matching")
{
  GIVEN("A device")
  {
    ossia::net::generic_device device{"test"};

    auto vco_node = device.create_child("vco");
    vco_node->create_child("frequency")->create_parameter();
    vco_node->create_child("waveform")->create_parameter();
    vco_node->create_child("cyclic_ratio")->create_parameter();

    auto filter_node = device.create_child("filter");
    for(int i = 0; i < 4; i++)
    {
      auto band = filter_node->create_child("band.1");
      band->create_child("Q")->create_parameter();
      band->create_child("frequency")->create_parameter();
      band->create_child("Gain")->create_parameter();
    }

    auto adsr_node = device.create_child("adsr");
    adsr_node->create_child("attack")->create_parameter();
    adsr_node->create_child("decay")->create_parameter();
    adsr_node->create_child("sustain")->create_parameter();
    adsr_node->create_child("release")->create_parameter();

    ossia::net::fuzzysearch_options opt;
    WHEN("Searching for Q case-sensitively")
    {
      opt.case_sensitive = true;
      std::vector<ossia::net::fuzzysearch_result> matches;
      ossia::net::fuzzysearch({&device.get_root_node()}, {"Q"}, matches, opt);
      INFO(matches.front().oscname);
      THEN("We find them through Q")
      {
        REQUIRE(!matches.empty());
        REQUIRE(matches.front().oscname.back() == 'Q');
      }
    }
    WHEN("Searching for q case-sensitively")
    {
      opt.case_sensitive = true;
      std::vector<ossia::net::fuzzysearch_result> matches;
      ossia::net::fuzzysearch({&device.get_root_node()}, {"q"}, matches, opt);
      INFO(matches.front().oscname);
      THEN("We find them through Q")
      {
        REQUIRE(!matches.empty());
        REQUIRE(string_ends_with(matches.front().oscname, "frequency"));
      }
    }
    WHEN("Searching for Q case-insensitively")
    {
      opt.case_sensitive = false;
      std::vector<ossia::net::fuzzysearch_result> matches_Q, matches_q;
      ossia::net::fuzzysearch({&device.get_root_node()}, {"Q"}, matches_Q, opt);
      ossia::net::fuzzysearch({&device.get_root_node()}, {"q"}, matches_q, opt);

      THEN("We find them through Q or q indiscriminately")
      {
        REQUIRE(!matches_Q.empty());
        REQUIRE(matches_Q == matches_q);
        REQUIRE(
            (matches_Q.front().oscname.find('Q') != std::string::npos
             || matches_Q.front().oscname.find('q') != std::string::npos));
      }
    }
  }
}
