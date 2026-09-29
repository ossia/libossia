#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/network/value/value.hpp>

#include <string>
#include <vector>

namespace ossia
{
class OSSIA_EXPORT math_expression
{
public:
  math_expression();
  ~math_expression();

  void seed_random(uint64_t seed1, uint64_t seed2);
  void add_variable(const std::string& var, double& value);
  void add_constant(const std::string& var, double& value);
  void add_vector(const std::string& var, std::vector<double>& value);
  void rebase_vector(const std::string& var, std::vector<double>& value);
  void add_constants();
  void register_symbol_table();

  bool set_expression(const std::string& expr);
  bool recompile();

  //! Whether the current expression compiled successfully.
  bool valid() const noexcept;

  //! Whether the expression *text* references that variable. This is a lexical
  //! query: it answers for expressions that did not compile, too.
  bool has_variable(std::string_view var) const noexcept;

  //! The error of the last compilation of this expression; empty if it compiled.
  std::string error() const;

  //! NaN if the expression is not valid or its evaluation was interrupted.
  double value();

  //! No value if the expression is not valid or its evaluation was interrupted.
  ossia::value result();

  //! Whether the last evaluation ran out of its loop budget and was cut short.
  //! Its effects on the variables up to that point stay.
  bool interrupted() const noexcept;

private:
  math_expression(const math_expression&) = delete;
  math_expression(math_expression&&) = delete;
  math_expression& operator=(const math_expression&) = delete;
  math_expression& operator=(math_expression&&) = delete;

  struct impl;
  impl* impl{};
};
}
