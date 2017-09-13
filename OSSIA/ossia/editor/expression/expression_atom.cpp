// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/expression/expression_atom.hpp>

namespace ossia
{
namespace expressions
{
expression_atom::expression_atom(
    const value& lhs, comparator op, const value& rhs)
    : m_first(lhs), m_second(rhs), m_operator(op)
{
  if (!lhs.valid() || !rhs.valid())
    throw std::runtime_error("expression_atom created with invalid values");
}

expression_atom::expression_atom(
    const destination& lhs, comparator op, const value& rhs)
    : m_first(lhs), m_second(rhs), m_operator(op)
{
  if (!rhs.valid())
    throw std::runtime_error("expression_atom created with invalid values");
}

expression_atom::expression_atom(
    const value& lhs, comparator op, const destination& rhs)
    : m_first(lhs), m_second(rhs), m_operator(op)
{
  if (!lhs.valid())
    throw std::runtime_error("expression_atom created with invalid values");
}

expression_atom::expression_atom(
    const expression_atom::val_t& lhs, comparator op,
    const expression_atom::val_t& rhs, expression_atom::dummy_t)
    : m_first(lhs), m_second(rhs), m_operator(op)
{
}

expression_atom::expression_atom(
    const destination& lhs, comparator op, const destination& rhs)
    : m_first(lhs), m_second(rhs), m_operator(op)
{
}

expression_atom::~expression_atom()
{
  if (!expression_callback_container::callbacks_empty())
    expression_callback_container::callbacks_clear();
}

bool expression_atom::evaluate() const
{
  return eggs::variants::apply(*this, m_first, m_second);
}

void expression_atom::update() const
{
  // pull value of the first operand if it is a Destination
  if (const destination* d = m_first.target<destination>())
  {
    d->address().pull_value();
  }

  // pull value of the second operand if it is a Destination
  if (const destination* d = m_second.target<destination>())
  {
    d->address().pull_value();
  }
}

const expression_atom::val_t& expression_atom::get_first_operand() const
{
  return m_first;
}

comparator expression_atom::get_operator() const
{
  return m_operator;
}

const expression_atom::val_t& expression_atom::get_second_operand() const
{
  return m_second;
}

void expression_atom::on_first_callback_added()
{
  // start first operand observation if it is a Destination
  if (auto d = m_first.target<destination>())
  {
    m_firstCallback = d->address().add_callback(
        [&](const ossia::value& result) { first_value_callback(result); });
  }

  // start second operand observation if it is a Destination
  if (auto d = m_second.target<destination>())
  {
    m_secondCallback = d->address().add_callback(
        [&](const ossia::value& result) { second_value_callback(result); });
  }
}

void expression_atom::on_removing_last_callback()
{
  // stop first operand observation if it is a Destination
  if (auto d = m_first.target<destination>())
  {
    d->address().remove_callback(m_firstCallback);
  }

  // start second operand observation if it is a Destination
  if (auto d = m_second.target<destination>())
  {
    d->address().remove_callback(m_secondCallback);
  }
}

bool expression_atom::
operator()(const ossia::value& first, const ossia::value& second) const
{
  switch (m_operator)
  {
    case comparator::EQUAL:
    {
      return first == second;
    }
    case comparator::DIFFERENT:
    {
      return first != second;
    }
    case comparator::GREATER:
    {
      return first > second;
    }
    case comparator::LOWER:
    {
      return first < second;
    }
    case comparator::GREATER_EQUAL:
    {
      return first >= second;
    }
    case comparator::LOWER_EQUAL:
    {
      return first <= second;
    }
    default:
      return false;
  }
}

bool expression_atom::
operator()(const ossia::value& first, const ossia::destination& second) const
{
  return operator()(first, second.pull());
}
bool expression_atom::
operator()(const ossia::destination& first, const ossia::value& second) const
{
  return operator()(first.pull(), second);
}
bool expression_atom::operator()(
    const ossia::destination& first, const ossia::destination& second) const
{
  return operator()(first.pull(), second.pull());
}

bool expression_atom::
operator()(const ossia::value& first, const val_t& second) const
{
  return eggs::variants::apply(
      [&](const auto& t) { return this->operator()(first, t); }, second);
}
bool expression_atom::
operator()(const val_t& first, const ossia::value& second) const
{
  return eggs::variants::apply(
      [&](const auto& t) { return this->operator()(t, second); }, first);
}

void expression_atom::first_value_callback(const ossia::value& value)
{
  if (value.valid())
    send((*this)(value, m_second));
}

void expression_atom::second_value_callback(const ossia::value& value)
{
  if (value.valid())
    send((*this)(m_first, value));
}
}
}
