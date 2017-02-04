#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/expression/expression_atom.hpp>

namespace ossia
{
namespace expressions
{
expression_atom::expression_atom(
    const value& lhs, comparator op, const value& rhs)
    : mFirstValue(lhs), mSecondValue(rhs), mOperator(op)
{
  if(!mFirstValue.valid() || !mSecondValue.valid())
    throw std::runtime_error("expression_atom created with invalid values");
}

expression_atom::~expression_atom()
{
  if(!expression_callback_container::callbacks_empty())
    expression_callback_container::callbacks_clear();
}

bool expression_atom::evaluate() const
{
  return do_evaluation(mFirstValue, mSecondValue);
}

void expression_atom::update() const
{
  // pull value of the first operand if it is a Destination
  if (auto d = mFirstValue.target<Destination>())
  {
    d->value.get().pullValue();
  }

  // pull value of the second operand if it is a Destination
  if (auto d = mSecondValue.target<Destination>())
  {
    d->value.get().pullValue();
  }
}

const value& expression_atom::getFirstOperand() const
{
  return mFirstValue;
}

comparator expression_atom::getOperator() const
{
  return mOperator;
}

const value& expression_atom::getSecondOperand() const
{
  return mSecondValue;
}

void expression_atom::onFirstCallbackAdded()
{
  // start first operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (auto d = mFirstValue.target<Destination>())
  {
    mFirstValueCallbackIndex = d->value.get().add_callback(
        [&](const ossia::value& result) { firstValueCallback(result); });

  }

  // start second operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (auto d = mSecondValue.target<Destination>())
  {
    mSecondValueCallbackIndex = d->value.get().add_callback(
        [&](const ossia::value& result) { secondValueCallback(result); });

  }
}

void expression_atom::onRemovingLastCallback()
{
  // stop first operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (auto d = mFirstValue.target<Destination>())
  {
    d->value.get().remove_callback(mFirstValueCallbackIndex);
  }

  // start second operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (auto d = mSecondValue.target<Destination>())
  {
    d->value.get().remove_callback(mSecondValueCallbackIndex);
  }
}

bool expression_atom::do_evaluation(
    const value& first, const value& second) const
{
  switch (mOperator)
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
    send(do_evaluation(value, mSecondValue));
}

void expression_atom::secondValueCallback(const value& value)
{
  if (value.valid())
    send(do_evaluation(mFirstValue, value));
}
}
}
