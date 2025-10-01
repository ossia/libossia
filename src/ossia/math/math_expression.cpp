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
#include <optional>
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
namespace
{
template <typename T>
struct bit_shiftl
{
  template <typename _Tp, typename _Up>
  constexpr auto operator()(_Tp&& __t, _Up&& __u) const noexcept
  {
    return __t << __u;
  }
};

template <typename T>
struct bit_shiftr
{
  template <typename _Tp, typename _Up>
  constexpr auto operator()(_Tp&& __t, _Up&& __u) const noexcept
  {
    return __t >> __u;
  }
};

template <typename Op, typename T>
struct bit_unop : public exprtk::ifunction<T>
{
  bit_unop()
      : exprtk::ifunction<T>{1}
  {
  }
  T operator()(const T& rhs) noexcept override { return Op{}((int64_t)rhs); }
};

template <typename Op, typename T>
struct bit_binop : public exprtk::ifunction<T>
{
  bit_binop()
      : exprtk::ifunction<T>{2}
  {
  }

  T operator()(const T& lhs, const T& rhs) noexcept override
  {
    return Op{}((int64_t)lhs, (int64_t)rhs);
  }
};

template <typename T>
struct rescale : public exprtk::ifunction<T>
{
  rescale()
      : exprtk::ifunction<T>{5}
  {
  }

  T operator()(
      const T& value, const T& inMin, const T& inMax, const T& outMin,
      const T& outMax) noexcept override
  {
    if((inMax - inMin) == T(0.0))
      return outMin;

    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
  }
};

template <typename T>
struct normalize : public exprtk::ifunction<T>
{
  normalize()
      : exprtk::ifunction<T>{3}
  {
  }

  T operator()(const T& value, const T& inMin, const T& inMax) noexcept override
  {
    if((inMax - inMin) <= T(0.0))
      return 0.0;

    return (value - inMin) / (inMax - inMin);
  }
};

template <typename T>
struct lerp : public exprtk::ifunction<T>
{
  lerp()
      : exprtk::ifunction<T>{3}
  {
  }

  T operator()(const T& value, const T& start, const T& stop) noexcept override
  {
    return start * (1. - value) + stop * value;
  }
};

template <typename T>
struct step : public exprtk::ifunction<T>
{
  step()
      : exprtk::ifunction<T>{2}
  {
  }

  T operator()(const T& value, const T& edge) noexcept override
  {
    return value < edge ? 0. : 1.;
  }
};

template <typename T>
struct smoothstep : public exprtk::ifunction<T>
{
  smoothstep()
      : exprtk::ifunction<T>{3}
  {
  }

  T operator()(const T& value, const T& start, const T& stop) noexcept override
  {
    if((stop - start) <= T(0.0))
      return 0.;

    auto x = std::clamp((value - start) / (stop - start), (T)0.0, (T)1.0);
    return x * x * (3 - 2 * x);
  }
};

template <typename T>
struct smoothstep5 : public exprtk::ifunction<T>
{
  smoothstep5()
      : exprtk::ifunction<T>{3}
  {
  }

  T operator()(const T& value, const T& start, const T& stop) noexcept override
  {
    if((stop - start) <= T(0.0))
      return 0.;

    auto x = std::clamp((value - start) / (stop - start), (T)0.0, (T)1.0);
    return x * x * x * (x * (x * 6 - 15) + 10);
  }
};

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
    if(min < max)
      return std::uniform_real_distribution<>{min, max}(engine);
    else if(max < min)
      return std::uniform_real_distribution<>{max, min}(engine);
    else
      return min;
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

static thread_local exprtk::parser<double> g_exprtk_parser
    = exprtk::parser<double>{exprtk::parser<double>::settings_store{}};
}
struct math_expression::impl
{
  rand_gen<double> random;
  perlin<double, 1> noise1d;
  boost::container::flat_map<std::string, std::shared_ptr<exprtk::vector_view<double>>>
      vector_views;
  exprtk::symbol_table<double> syms;
  exprtk::expression<double> expr;
  std::string cur_expr_txt;
  std::optional<std::vector<std::string>> variables;
  std::vector<std::string> missing_variables;
  bool valid{};

  [[no_unique_address]] bit_binop<std::bit_and<>, double> f_bit_and;
  [[no_unique_address]] bit_binop<std::bit_or<>, double> f_bit_or;
  [[no_unique_address]] bit_binop<std::bit_xor<>, double> f_bit_xor;
  [[no_unique_address]] bit_unop<std::bit_not<>, double> f_bit_not;
  [[no_unique_address]] bit_binop<bit_shiftl<double>, double> f_bit_shiftl;
  [[no_unique_address]] bit_binop<bit_shiftr<double>, double> f_bit_shiftr;
  [[no_unique_address]] rescale<double> f_rescale;
  [[no_unique_address]] normalize<double> f_norm;
  [[no_unique_address]] step<double> f_step;
  [[no_unique_address]] lerp<double> f_lerp;
  [[no_unique_address]] smoothstep<double> f_smoothstep;
  [[no_unique_address]] smoothstep5<double> f_smoothstep5;
};

math_expression::math_expression()
    : impl{new struct impl}
{
  impl->syms.add_function("random", impl->random);
  impl->syms.add_function("noise", impl->noise1d);
  impl->syms.add_function("bitwise_and", impl->f_bit_and);
  impl->syms.add_function("bitwise_or", impl->f_bit_or);
  impl->syms.add_function("bitwise_xor", impl->f_bit_xor);
  impl->syms.add_function("bitwise_not", impl->f_bit_not);
  impl->syms.add_function("bitwise_shiftl", impl->f_bit_shiftl);
  impl->syms.add_function("bitwise_shiftr", impl->f_bit_shiftr);
  impl->syms.add_function("map", impl->f_rescale);
  impl->syms.add_function("norm", impl->f_norm);
  impl->syms.add_function("step", impl->f_step);
  impl->syms.add_function("lerp", impl->f_lerp);
  impl->syms.add_function("smoothstep", impl->f_smoothstep);
  impl->syms.add_function("smoothstep5", impl->f_smoothstep5);
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
  impl->vector_views.reserve(12);
  auto v = std::make_shared<exprtk::vector_view<double>>(
      exprtk::make_vector_view(value, value.size()));
  impl->vector_views[var] = v;
  impl->syms.add_vector(var, *v);
}

void math_expression::rebase_vector(const std::string& var, std::vector<double>& value)
{
  auto it = impl->vector_views.find(var);
  assert(it != impl->vector_views.end());
  it->second->set_size(value.size());
  it->second->rebase(value.data());
}

// void math_expression::remove_vector(const std::string& var)
// {
//   impl->syms.remove_vector(var);
// }

void math_expression::add_constants()
{
  impl->syms.add_constants();
}

void math_expression::register_symbol_table()
{
  impl->expr.register_symbol_table(impl->syms);
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
  if(std::binary_search(
         impl->missing_variables.begin(), impl->missing_variables.end(), var))
    return false;

  if(impl->cur_expr_txt.find(var) == std::string::npos)
  {
    impl->missing_variables.push_back(std::string(var));
    return false;
  }

  auto b = impl->expr;
  if(!impl->variables)
  {
    if(impl->valid)
    {
      impl->variables.emplace();
      exprtk::collect_variables(impl->cur_expr_txt, *impl->variables);
    }
  }
  return std::binary_search(impl->variables->begin(), impl->variables->end(), var);
}

bool math_expression::recompile()
{
  impl->variables = std::nullopt;

  impl->valid = g_exprtk_parser.compile(impl->cur_expr_txt, impl->expr);
  if(!impl->valid)
  {
    ossia::logger().error("Error while parsing: {}", g_exprtk_parser.error());
  }

  return impl->valid;
}

std::string math_expression::error() const
{
  return g_exprtk_parser.error();
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
