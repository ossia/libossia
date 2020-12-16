#include <ossia/math/math_expression.hpp>
#include <ossia/detail/logger.hpp>
#include <exprtk.hpp>
namespace ossia
{
struct math_expression::impl {
  exprtk::symbol_table<double> syms;
  exprtk::expression<double> expr;
  exprtk::parser<double> parser;
  std::string cur_expr_txt;
  bool valid{};
};

math_expression::math_expression()
  : impl{new struct impl}
{

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
