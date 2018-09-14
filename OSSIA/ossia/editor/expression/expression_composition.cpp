// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/expression/expression_composition.hpp>

namespace ossia
{
namespace expressions
{

expression_composition::expression_composition(
    expression_ptr expr1, binary_operator op, expression_ptr expr2)
    : m_first(std::move(expr1)), m_second(std::move(expr2)), m_operator(op)
{
  if (!m_first || !m_second)
    throw std::runtime_error("An argument to expression_composition is null");
}

expression_composition::~expression_composition()
{
  if (!expression_callback_container::callbacks_empty())
    expression_callback_container::callbacks_clear();
}

bool expression_composition::evaluate() const
{
  return do_evaluation(
      expressions::evaluate(*m_first), expressions::evaluate(*m_second));
}

void expression_composition::update() const
{
  expressions::update(*m_first);
  expressions::update(*m_second);
}

expression_base& expression_composition::get_first_operand() const
{
  return *m_first;
}

binary_operator expression_composition::get_operator() const
{
  return m_operator;
}

expression_base& expression_composition::get_second_operand() const
{
  return *m_second;
}

void expression_composition::on_first_callback_added()
{
  // start first expression observation
  m_firstIndex = expressions::add_callback(
      *m_first, [&](bool result) { first_callback(result); });

  // start second expression observation
  m_secondIndex = expressions::add_callback(
      *m_second, [&](bool result) { second_callback(result); });
}

void expression_composition::on_removing_last_callback()
{
  // stop first expression observation
  expressions::remove_callback(*m_first, m_firstIndex);

  // stop second expression observation
  expressions::remove_callback(*m_second, m_secondIndex);
}

bool expression_composition::do_evaluation(bool first, bool second) const
{
  switch (m_operator)
  {
    case binary_operator::AND:
    {
      return first && second;
    }
    case binary_operator::OR:
    {
      return first || second;
    }
    case binary_operator::XOR:
    {
      return first ^ second;
    }
    default:
      return false;
  }
}

void expression_composition::first_callback(bool first_result)
{
  bool result = do_evaluation(first_result, expressions::evaluate(*m_second));
  send(result);
}

void expression_composition::second_callback(bool second_result)
{
  bool result = do_evaluation(expressions::evaluate(*m_first), second_result);
  send(result);
}
}
}
