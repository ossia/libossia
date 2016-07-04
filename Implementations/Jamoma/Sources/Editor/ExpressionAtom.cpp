#include "Editor/JamomaExpressionAtom.h"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<ExpressionAtom> ExpressionAtom::create(const Value* value1, Operator op, const Value* value2)
  {
    return make_shared<JamomaExpressionAtom>(value1, op, value2);
  }
}

JamomaExpressionAtom::JamomaExpressionAtom(const Value* value1, Operator op, const Value* value2) :
mFirstValue(value1->clone()),
mOperator(op),
mSecondValue(value2->clone())
{}

JamomaExpressionAtom::JamomaExpressionAtom(const JamomaExpressionAtom * other) :
mFirstValue(other->mFirstValue->clone()),
mOperator(other->mOperator),
mSecondValue(other->mSecondValue->clone())
{}

shared_ptr<ExpressionAtom> JamomaExpressionAtom::clone() const
{
  return make_shared<JamomaExpressionAtom>(this);
}

JamomaExpressionAtom::~JamomaExpressionAtom()
{}

ExpressionAtom::~ExpressionAtom()
{}

# pragma mark -
# pragma mark Execution

bool JamomaExpressionAtom::evaluate() const
{
  return do_evaluation(*mFirstValue, *mSecondValue);
}

void JamomaExpressionAtom::update() const
{
  // pull value of the first operand if it is a Destination
  if (mFirstValue->getType() == Value::Type::DESTINATION)
  {
    Destination* d = (Destination*)mFirstValue;
    if (d->value->getAddress())
    {
      d->value->getAddress()->pullValue();
    }
  }

  // pull value of the second operand if it is a Destination
  if (mSecondValue->getType() == Value::Type::DESTINATION)
  {
    Destination* d = (Destination*)mSecondValue;
    if (d->value->getAddress())
    {
      d->value->getAddress()->pullValue();
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
    return *mFirstValue == *e.mFirstValue && mOperator == e.mOperator && *mSecondValue == *e.mSecondValue;
  }
  else
    return false;
}

bool JamomaExpressionAtom::operator!= (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::ATOM)
  {
    const JamomaExpressionAtom e = dynamic_cast<const JamomaExpressionAtom&>(expression);
    return *mFirstValue != *e.mFirstValue || mOperator != e.mOperator || *mSecondValue != *e.mSecondValue;
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
    if (mFirstValue->getType() == Value::Type::DESTINATION)
    {
      Destination* d = (Destination*)mFirstValue;
      if (d->value->getAddress())
      {
        mFirstValueCallbackIndex = d->value->getAddress()->addCallback([&] (const OSSIA::Value& result) { firstValueCallback(result); });
      }
    }

    // start second operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mSecondValue->getType() == Value::Type::DESTINATION)
    {
      Destination* d = (Destination*)mSecondValue;
      if (d->value->getAddress())
      {
        mSecondValueCallbackIndex = d->value->getAddress()->addCallback([&] (const OSSIA::Value& result) { secondValueCallback(result); });
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
    if (mFirstValue->getType() == Value::Type::DESTINATION)
    {
      Destination* d = (Destination*)mFirstValue;
      if (d->value->getAddress())
      {
        d->value->getAddress()->removeCallback(mFirstValueCallbackIndex);
      }
    }

    // start second operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mSecondValue->getType() == Value::Type::DESTINATION)
    {
      Destination* d = (Destination*)mSecondValue;
      if (d->value->getAddress())
      {
        d->value->getAddress()->removeCallback(mSecondValueCallbackIndex);
      }
    }
  }
}

# pragma mark -
# pragma mark Accessors

const Value* JamomaExpressionAtom::getFirstOperand() const
{
  return mFirstValue;
}

ExpressionAtom::Operator JamomaExpressionAtom::getOperator() const
{
  return mOperator;
}

const Value* JamomaExpressionAtom::getSecondOperand() const
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
    if(mSecondValue)
    {
      send(do_evaluation(value, *mSecondValue));
    }
}

void JamomaExpressionAtom::secondValueCallback(const Value& value)
{
    if(mFirstValue)
    {
      send(do_evaluation(*mFirstValue, value));
    }
}
