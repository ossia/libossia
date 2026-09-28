#include <ossia/detail/config.hpp>

#include <ossia/math/math_expression.hpp>

#include "include_catch.hpp"

#include <chrono>
#include <string>
#include <vector>

// has_variable is asked of expressions that may not compile against the
// vector sizes currently in place: it answers from the text.
TEST_CASE("has_variable_scalar", "math_expression")
{
  double x{};
  ossia::math_expression e;
  e.add_variable("x", x);
  e.register_symbol_table();
  REQUIRE(e.set_expression("x + 1"));
  REQUIRE(e.has_variable("x"));
  REQUIRE_FALSE(e.has_variable("xv"));
  REQUIRE_FALSE(e.has_variable("y"));
}

TEST_CASE("has_variable_vector_at_zero", "math_expression")
{
  ossia::math_expression e;
  e.set_expression("xv[0] * 2");
  REQUIRE(e.has_variable("xv"));
  REQUIRE_FALSE(e.has_variable("x"));
}

TEST_CASE("has_variable_vector_past_zero_and_sized", "math_expression")
{
  // Compiles neither with xv as a scalar nor as a vector of size 1.
  ossia::math_expression e;
  e.set_expression("var n := xv[]; [xv[0], xv[1], n]");
  REQUIRE(e.has_variable("xv"));
  REQUIRE_FALSE(e.has_variable("x"));

  e.set_expression("[xv[0], xv[1], xv[2]]");
  REQUIRE(e.has_variable("xv"));
}

TEST_CASE("has_variable_is_recomputed_after_a_change", "math_expression")
{
  ossia::math_expression e;
  e.set_expression("[xv[0], xv[1]]");
  REQUIRE(e.has_variable("xv"));
  e.set_expression("x * 3");
  REQUIRE_FALSE(e.has_variable("xv"));
  REQUIRE(e.has_variable("x"));
}

// Expression text comes from users, presets and network messages: whatever it
// is, compiling it and asking for its variables returns, quickly.
TEST_CASE("hostile_expressions", "math_expression")
{
  std::vector<std::string> texts{
      "",
      "xv[",
      "xv[]]]]",
      "[[[[xv",
      "xv[0",
      "(((((",
      ")))))",
      "var xv := ; xv[1]",
      "\x01\x02\xff\xfe xv",
      "'xv'",
      "for(var i := 0; i < xv[]; i += 1) { xv[i] }",
  };

  {
    std::string nested(20000, '(');
    nested += "xv[1]";
    nested += std::string(20000, ')');
    texts.push_back(std::move(nested));
  }
  {
    std::string sum;
    for(int i = 0; i < 20000; i++)
      sum += "xv[1] + ";
    sum += "1";
    texts.push_back(std::move(sum));
  }
  {
    std::string calls;
    for(int i = 0; i < 2000; i++)
      calls += "sin(";
    calls += "xv[1]";
    calls += std::string(2000, ')');
    texts.push_back(std::move(calls));
  }

  for(const auto& text : texts)
  {
    INFO(text.substr(0, 64));
    const auto t0 = std::chrono::steady_clock::now();
    ossia::math_expression e;
    e.set_expression(text);
    const bool has = e.has_variable("xv");
    (void)e.error();
    if(e.valid())
      (void)e.result();
    const auto elapsed = std::chrono::steady_clock::now() - t0;

    if(text.find("xv") == std::string::npos)
      REQUIRE_FALSE(has);
    CHECK(elapsed < std::chrono::seconds(5));
  }
}
