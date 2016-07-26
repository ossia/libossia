#include "ExpressionAtom_impl.hpp"

namespace impl
{

JamomaExpressionAtom::JamomaExpressionAtom(
    const Value& value1,
    Operator op,
    const Value& value2) :
mFirstValue(value1),
mOperator(op),
mSecondValue(value2)
{}

JamomaExpressionAtom::JamomaExpressionAtom(const JamomaExpressionAtom& other) :
mFirstValue(other.mFirstValue),
mOperator(other.mOperator),
mSecondValue(other.mSecondValue)
{}

std::shared_ptr<ExpressionAtom> JamomaExpressionAtom::clone() const
{
  return std::make_shared<JamomaExpressionAtom>(*this);
}

JamomaExpressionAtom::~JamomaExpressionAtom()
{}

# pragma mark -
# pragma mark Execution

bool JamomaExpressionAtom::evaluate() const
{
  return do_evaluation(mFirstValue, mSecondValue);
}

void JamomaExpressionAtom::update() const
{
  // pull value of the first operand if it is a Destination
  if (mFirstValue.getType() == OSSIA::Type::DESTINATION)
  {
    auto& d = mFirstValue.get<Destination>();
    if (const auto& addr = d.value->getAddress())
    {
      addr->pullValue();
    }
  }

  // pull value of the second operand if it is a Destination
  if (mSecondValue.getType() == OSSIA::Type::DESTINATION)
  {
    auto& d = mSecondValue.get<Destination>();
    if (const auto& addr = d.value->getAddress())
    {
      addr->pullValue();
    }
  }
}

# pragma mark -
# pragma mark Operator

bool JamomaExpressionAtom::operator== (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::ATOM)
  {
    const JamomaExpressionAtom e = dynamic_cast<const JamomaExpressionAtom&>(expression);
    return Value{mFirstValue} == Value{e.mFirstValue} &&
           mOperator == e.mOperator &&
           Value{mSecondValue} == Value{e.mSecondValue};
  }
  else
    return false;
}

bool JamomaExpressionAtom::operator!= (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::ATOM)
  {
    const JamomaExpressionAtom e = dynamic_cast<const JamomaExpressionAtom&>(expression);
    return Value{mFirstValue} != Value{e.mFirstValue} ||
           mOperator != e.mOperator ||
           Value{mSecondValue} != Value{e.mSecondValue};
  }
  else
    return true;
}

# pragma mark -
# pragma mark Callback Container

Expression::iterator JamomaExpressionAtom::addCallback(ResultCallback callback)
{
  auto it = CallbackContainer::addCallback(std::move(callback));

  if (callbacks().size() == 1)
  {
    // start first operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mFirstValue.getType() == OSSIA::Type::DESTINATION)
    {
      auto& d = mFirstValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        mFirstValueCallbackIndex = addr->addCallback(
              [&] (const OSSIA::Value& result) { firstValueCallback(result); });
      }
    }

    // start second operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mSecondValue.getType() == OSSIA::Type::DESTINATION)
    {
      auto& d = mSecondValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        mSecondValueCallbackIndex = addr->addCallback(
              [&] (const OSSIA::Value& result) { secondValueCallback(result); });
      }
    }
  }

  return it;
}

void JamomaExpressionAtom::removeCallback(Expression::iterator callback)
{
  CallbackContainer::removeCallback(callback);

  if (callbacks().size() == 0)
  {
    // stop first operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mFirstValue.getType() == OSSIA::Type::DESTINATION)
    {
      auto& d = mFirstValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        addr->removeCallback(mFirstValueCallbackIndex);
      }
    }

    // start second operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mSecondValue.getType() == OSSIA::Type::DESTINATION)
    {
      auto& d = mSecondValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        addr->removeCallback(mSecondValueCallbackIndex);
      }
    }
  }
}

# pragma mark -
# pragma mark Accessors

const Value& JamomaExpressionAtom::getFirstOperand() const
{
  return mFirstValue;
}

ExpressionAtom::Operator JamomaExpressionAtom::getOperator() const
{
  return mOperator;
}

const Value& JamomaExpressionAtom::getSecondOperand() const
{
  return mSecondValue;
}

# pragma mark -
# pragma mark Implementation Specific

bool JamomaExpressionAtom::do_evaluation(const Value& first, const Value& second) const
{
  switch (mOperator)
  {
    case Operator::EQUAL :
    {
      return first == second;
    }
    case Operator::DIFFERENT :
    {
      return first != second;
    }
    case Operator::GREATER_THAN :
    {
      return first > second;
    }
    case Operator::LOWER_THAN :
    {
      return first < second;
    }
    case Operator::GREATER_THAN_OR_EQUAL :
    {
      return first >= second;
    }
    case Operator::LOWER_THAN_OR_EQUAL :
    {
      return first <= second;
    }
    default :
      return false;
  }
}

void JamomaExpressionAtom::firstValueCallback(const Value& value)
{
  if(mSecondValue.valid())
    send(do_evaluation(value, mSecondValue));
}

void JamomaExpressionAtom::secondValueCallback(const Value& value)
{
  if(mSecondValue.valid())
    send(do_evaluation(mFirstValue, value));
}

}
