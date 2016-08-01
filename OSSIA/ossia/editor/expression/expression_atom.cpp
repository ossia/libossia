#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/expression/expression_atom.hpp>

namespace ossia
{
namespace expressions
{
expression_atom::~expression_atom() = default;

expression_atom::expression_atom(
    const value& lhs, expression_atom::Comparator op, const value& rhs)
    : mFirstValue(lhs), mOperator(op), mSecondValue(rhs)
{
}

bool expression_atom::evaluate() const
{
  return do_evaluation(mFirstValue, mSecondValue);
}

void expression_atom::update() const
{
  // pull value of the first operand if it is a Destination
  if (mFirstValue.getType() == ossia::val_type::DESTINATION)
  {
    auto& d = mFirstValue.get<Destination>();
    if (const auto& addr = d.value->getAddress())
    {
      addr->pullValue();
    }
  }

  // pull value of the second operand if it is a Destination
  if (mSecondValue.getType() == ossia::val_type::DESTINATION)
  {
    auto& d = mSecondValue.get<Destination>();
    if (const auto& addr = d.value->getAddress())
    {
      addr->pullValue();
    }
  }
}

const value& expression_atom::getFirstOperand() const
{
  return mFirstValue;
}

expression_atom::Comparator expression_atom::getOperator() const
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
  if (mFirstValue.getType() == ossia::val_type::DESTINATION)
  {
    auto& d = mFirstValue.get<Destination>();
    if (const auto& addr = d.value->getAddress())
    {
      mFirstValueCallbackIndex = addr->add_callback(
          [&](const ossia::value& result) { firstValueCallback(result); });
    }
  }

  // start second operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (mSecondValue.getType() == ossia::val_type::DESTINATION)
  {
    auto& d = mSecondValue.get<Destination>();
    if (const auto& addr = d.value->getAddress())
    {
      mSecondValueCallbackIndex = addr->add_callback(
          [&](const ossia::value& result) { secondValueCallback(result); });
    }
  }
}

void expression_atom::onRemovingLastCallback()
{
  // stop first operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (mFirstValue.getType() == ossia::val_type::DESTINATION)
  {
    auto& d = mFirstValue.get<Destination>();
    if (const auto& addr = d.value->getAddress())
    {
      addr->remove_callback(mFirstValueCallbackIndex);
    }
  }

  // start second operand observation if it is a Destination
  //! \todo what about Tuple of Destinations ?
  if (mSecondValue.getType() == ossia::val_type::DESTINATION)
  {
    auto& d = mSecondValue.get<Destination>();
    if (const auto& addr = d.value->getAddress())
    {
      addr->remove_callback(mSecondValueCallbackIndex);
    }
  }
}

bool expression_atom::do_evaluation(
    const value& first, const value& second) const
{
  switch (mOperator)
  {
    case Comparator::EQUAL:
    {
      return first == second;
    }
    case Comparator::DIFFERENT:
    {
      return first != second;
    }
    case Comparator::GREATER_THAN:
    {
      return first > second;
    }
    case Comparator::LOWER_THAN:
    {
      return first < second;
    }
    case Comparator::GREATER_THAN_OR_EQUAL:
    {
      return first >= second;
    }
    case Comparator::LOWER_THAN_OR_EQUAL:
    {
      return first <= second;
    }
    default:
      return false;
  }
}

void expression_atom::firstValueCallback(const value& value)
{
  if (mSecondValue.valid())
    send(do_evaluation(value, mSecondValue));
}

void expression_atom::secondValueCallback(const value& value)
{
  if (mSecondValue.valid())
    send(do_evaluation(mFirstValue, value));
}
}
}
