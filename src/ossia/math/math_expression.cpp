#include <ossia/math/math_expression.hpp>
#include <ossia/detail/logger.hpp>
#include <rnd/random.hpp>
#define exprtk_disable_string_capabilities 1
#include <exprtk.hpp>
#include <fmt/ranges.h>
#include <ossia/detail/math.hpp>
namespace ossia
{

template <typename T>
struct rand_gen
    : public exprtk::ifunction<T>
{
  rnd::pcg engine;

  rand_gen() : exprtk::ifunction<T>{2}
  {
  }

  T operator()(const T& min, const T& max) noexcept
  {
    return std::uniform_real_distribution<>{min, max}(engine);
  }
};

struct math_expression::impl {
  rand_gen<double> random;
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
}

math_expression::~math_expression()
{
  delete impl;
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

bool math_expression::set_expression(const std::string& expr)
{
  if (expr != impl->cur_expr_txt)
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
  impl->valid = impl->parser.compile(impl->cur_expr_txt, impl->expr);
  if (!impl->valid)
  {
    ossia::logger().error("Error while parsing: {}", impl->parser.error());
  }

  impl->variables.clear();
  exprtk::collect_variables(impl->cur_expr_txt, impl->variables);

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

ossia::value math_expression::result()
{
  double v = impl->expr.value();
  if(!ossia::safe_isnan(v))
    return v;

  std::vector<ossia::value> ret;
  const auto& r = impl->expr.results();

  for (std::size_t i = 0; i < r.count(); ++i)
  {
    using type_t = typename exprtk::results_context<double>::type_store_t;
    const type_t& t = r[i];

    switch (t.type)
    {
      case type_t::e_scalar:
      {
        ret.push_back(*(double*)t.data);
        break;
      }

      case type_t::e_vector:
      {
        std::vector<ossia::value> vec;
        vec.reserve(t.size);
        for(std::size_t i = 0; i < t.size; i++)
          vec.emplace_back(t.vec_data[t.size]);
        ret.push_back(std::move(vec));
        break;
      }

      default:
        continue;
    }
  }
  return ret;
}

}
