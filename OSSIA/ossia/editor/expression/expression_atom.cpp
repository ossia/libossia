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
  if(!m_first.valid() || !m_second.valid())
    throw std::runtime_error("expression_atom created with invalid values");
}

expression_atom::~expression_atom()
{
  if(!expression_callback_container::callbacks_empty())
    expression_callback_container::callbacks_clear();
}

bool expression_atom::evaluate() const
{
  return do_evaluation(m_first, m_second);
}

void expression_atom::update() const
{
  // pull value of the first operand if it is a Destination
  if (auto d = m_first.target<Destination>())
  {
    d->address().pull_value();
  }

  // pull value of the second operand if it is a Destination
  if (auto d = m_second.target<Destination>())
  {
    d->address().pull_value();
  }
}

const value& expression_atom::get_first_operand() const
{
  return m_first;
}

comparator expression_atom::get_operator() const
{
  return m_operator;
}

const value& expression_atom::get_second_operand() const
{
  return m_second;
}

void expression_atom::on_first_callback_added()
{
  // start first operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (auto d = m_first.target<Destination>())
  {
    m_firstCallback = d->address().add_callback(
        [&](const ossia::value& result) { firstValueCallback(result); });

  }

  // start second operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (auto d = m_second.target<Destination>())
  {
    m_secondCallback = d->address().add_callback(
        [&](const ossia::value& result) { secondValueCallback(result); });

  }
}

void expression_atom::on_removing_last_callback()
{
  // stop first operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (auto d = m_first.target<Destination>())
  {
    d->address().remove_callback(m_firstCallback);
  }

  // start second operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (auto d = m_second.target<Destination>())
  {
    d->address().remove_callback(m_secondCallback);
  }
}

bool expression_atom::do_evaluation(
    const value& first, const value& second) const
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

void expression_atom::firstValueCallback(const value& value)
{
  if (value.valid())
    send(do_evaluation(value, m_second));
}

void expression_atom::secondValueCallback(const value& value)
{
  if (value.valid())
    send(do_evaluation(m_first, value));
}
}
}
