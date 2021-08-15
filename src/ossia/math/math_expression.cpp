#include <ossia/math/math_expression.hpp>
#include <ossia/detail/logger.hpp>
#include <rnd/random.hpp>
#define exprtk_disable_string_capabilities 1
#include <exprtk.hpp>
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
    impl->valid = impl->parser.compile(impl->cur_expr_txt, impl->expr);
    if (!impl->valid)
    {
      ossia::logger().error("Error while parsing: {}", impl->parser.error());
    }
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

}
