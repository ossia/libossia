#include <ossia/detail/config.hpp>

#include <ossia/math/math_expression.hpp>

#include "include_catch.hpp"

#include <chrono>
#include <cmath>
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

// A runaway loop is cut short instead of hanging the evaluating thread, and
// the next evaluation starts with a fresh budget.
TEST_CASE("runaway_loop_is_interrupted", "math_expression")
{
  double a{1e12};
  ossia::math_expression e;
  e.add_variable("a", a);
  e.register_symbol_table();
  REQUIRE(e.set_expression("var i := 0; while(i < a) { i += 1; }; i"));

  const auto t0 = std::chrono::steady_clock::now();
  CHECK_FALSE(e.result().valid());
  CHECK(e.interrupted());
  CHECK(std::chrono::steady_clock::now() - t0 < std::chrono::seconds(5));

  a = 1000.;
  CHECK(e.value() == 1000.);
  CHECK_FALSE(e.interrupted());
}

TEST_CASE("nested_runaway_loops_share_one_budget", "math_expression")
{
  ossia::math_expression e;
  e.register_symbol_table();
  REQUIRE(e.set_expression(
      "var s := 0;"
      "for(var i := 0; i >= 0; i += 1) {"
      "  for(var j := 0; j >= 0; j += 1) { for(var k := 0; k >= 0; k += 1) { s += 1; } }"
      "};"
      "s"));

  const auto t0 = std::chrono::steady_clock::now();
  CHECK(std::isnan(e.value()));
  CHECK(e.interrupted());
  CHECK(std::chrono::steady_clock::now() - t0 < std::chrono::seconds(5));
}

TEST_CASE("bounded_loop_runs_to_completion", "math_expression")
{
  ossia::math_expression e;
  e.register_symbol_table();
  REQUIRE(e.set_expression("var s := 0; for(var i := 0; i < 100000; i += 1) { s += i; }; s"));
  for(int n = 0; n < 16; n++)
  {
    CHECK(e.value() == 4999950000.);
    CHECK_FALSE(e.interrupted());
  }
}
