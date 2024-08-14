#include <ossia/detail/logger.hpp>
#include <ossia/math/math_expression.hpp>
#include <ossia/math/safe_math.hpp>

#include <rnd/random.hpp>

#include <PerlinNoise.hpp>

#pragma GCC visibility push(default)
#pragma clang visibility push(default)
#include <cmath>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <exception>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#pragma GCC visibility pop
#pragma clang visibility pop
#define exprtk_disable_rtl_io 1
#define exprtk_disable_rtl_io_file 1
#define exprtk_disable_caseinsensitivity 1
#define exprtk_disable_string_capabilities 1
#pragma GCC visibility push(internal)
#pragma clang visibility push(internal)
#include <exprtk.hpp>
#pragma GCC visibility pop
#pragma clang visibility pop

#include <ossia/detail/fmt.hpp>
#include <ossia/detail/math.hpp>

#include <fmt/ranges.h>
namespace ossia
{

template <typename T>
struct rand_gen : public exprtk::ifunction<T>
{
  rnd::pcg engine;

  rand_gen()
      : exprtk::ifunction<T>{2}
  {
  }

  T operator()(const T& min, const T& max) noexcept override
  {
    return std::uniform_real_distribution<>{min, max}(engine);
  }
};

template <typename T, std::size_t N>
struct perlin;

template <typename T>
struct perlin<T, 1> : public exprtk::ifunction<T>
{
  const siv::PerlinNoise engine{4u}; // chosen by fair dice roll

  perlin()
      : exprtk::ifunction<T>{3}
  {
  }

  T operator()(const T& x, const T& octaves, const T& persistence) noexcept override
  {
    return engine.normalizedOctave1D_01(
        x, std::max(1., octaves), std::clamp(persistence, 0., 1.));
  }
};

struct math_expression::impl
{
  rand_gen<double> random;
  perlin<double, 1> noise1d;
  exprtk::symbol_table<double> syms;
  exprtk::expression<double> expr;
  exprtk::parser<double> parser;
  std::string cur_expr_txt;
  std::vector<std::string> variables;
  bool valid{};
};

math_expression::math_expression()
    : impl{new struct impl}
{
  impl->syms.add_function("random", impl->random);
  impl->syms.add_function("noise", impl->noise1d);
}

math_expression::~math_expression()
{
  delete impl;
}

void math_expression::seed_random(uint64_t seed1, uint64_t seed2)
{
  impl->random.engine.seed(seed1, seed2);
}
void math_expression::add_variable(const std::string& var, double& value)
{
  impl->syms.add_variable(var, value);
}

void math_expression::add_constant(const std::string& var, double& value)
{
  impl->syms.add_constant(var, value);
}

void math_expression::add_vector(const std::string& var, std::vector<double>& value)
{
  impl->syms.add_vector(var, value);
}

void math_expression::remove_vector(const std::string& var)
{
  impl->syms.remove_vector(var);
}

void math_expression::add_constants()
{
  impl->syms.add_constants();
}

void math_expression::register_symbol_table()
{
  impl->expr.register_symbol_table(impl->syms);
}

void math_expression::update_symbol_table()
{
  impl->expr.get_symbol_table(0) = impl->syms;
  recompile();
}

bool math_expression::set_expression(const std::string& expr)
{
  if(expr != impl->cur_expr_txt)
  {
    impl->cur_expr_txt = expr;
    recompile();
  }

  return impl->valid;
}

bool math_expression::has_variable(std::string_view var) const noexcept
{
  return std::binary_search(impl->variables.begin(), impl->variables.end(), var);
}

bool math_expression::recompile()
{
  impl->variables.clear();

  impl->valid = impl->parser.compile(impl->cur_expr_txt, impl->expr);
  if(impl->valid)
  {
    exprtk::collect_variables(impl->cur_expr_txt, impl->variables);
  }
  else
  {
    ossia::logger().error("Error while parsing: {}", impl->parser.error());
  }

  return impl->valid;
}

std::string math_expression::error() const
{
  return impl->parser.error();
}

double math_expression::value()
{
  return impl->expr.value();
}

static std::vector<ossia::value> result_to_vec(auto& r)
{
  using type_t = typename exprtk::results_context<double>::type_store_t;
  const auto N = r.count();

  std::vector<ossia::value> ret;
  for(std::size_t i = 0; i < N; ++i)
  {
    const type_t& t = r[i];

    switch(t.type)
    {
      case type_t::e_scalar: {
        ret.push_back(*(double*)t.data);
        break;
      }

      case type_t::e_vector: {
        std::vector<ossia::value> vec;
        vec.reserve(t.size);
        for(std::size_t j = 0; j < t.size; j++)
        {
          vec.emplace_back(t.vec_data[j]);
        }
        ret.emplace_back(ossia::value{std::move(vec)});
        break;
      }

      default:
        continue;
    }
  }
  return ret;
}
static ossia::value result_to_value(auto& r)
{
  using type_t = typename exprtk::results_context<double>::type_store_t;
  const auto N = r.count();
  switch(N)
  {
    case 2:
      if(r[0].type == type_t::e_scalar && r[1].type == type_t::e_scalar)
        return ossia::vec2f{(float)*(double*)r[0].data, (float)*(double*)r[1].data};
      break;
    case 3:
      if(r[0].type == type_t::e_scalar && r[1].type == type_t::e_scalar
         && r[2].type == type_t::e_scalar)
        return ossia::vec3f{
            (float)*(double*)r[0].data, (float)*(double*)r[1].data,
            (float)*(double*)r[2].data};
      break;
    case 4:
      if(r[0].type == type_t::e_scalar && r[1].type == type_t::e_scalar
         && r[2].type == type_t::e_scalar && r[3].type == type_t::e_scalar)
        return ossia::vec4f{
            (float)*(double*)r[0].data, (float)*(double*)r[1].data,
            (float)*(double*)r[2].data, (float)*(double*)r[3].data};
      break;
    default:
      break;
  }

  return ossia::value{result_to_vec(r)};
}

ossia::value math_expression::result()
{
  double v = impl->expr.value();
  if(!ossia::safe_isnan(v))
    return v;
  else
    return ossia::value{result_to_value(impl->expr.results())};
}

}
