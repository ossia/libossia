// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/expression/expression_not.hpp>

namespace ossia
{
namespace expressions
{
expression_not::expression_not(expression_ptr p) : m_expression{std::move(p)}
{
  if (!m_expression)
    throw std::runtime_error("An argument to expression_not is null");
}

expression_not::~expression_not()
{
  if (!expression_callback_container::callbacks_empty())
    expression_callback_container::callbacks_clear();
}

bool expression_not::evaluate() const
{
  return !expressions::evaluate(*m_expression);
}

void expression_not::update() const
{
  expressions::update(*m_expression);
}

expression_base& expression_not::get_expression() const
{
  return *m_expression;
}

void expression_not::on_first_callback_added()
{
  m_callback = expressions::add_callback(
      *m_expression, [&](bool result) { result_callback(result); });
}

void expression_not::on_removing_last_callback()
{
  expressions::remove_callback(*m_expression, m_callback);
}

void expression_not::result_callback(bool result)
{
  send(!result);
}
}
}
