#include "ExpressionComposition_impl.hpp"

namespace impl
{

JamomaExpressionComposition::JamomaExpressionComposition(std::shared_ptr<expression_base> expr1, Operator op, std::shared_ptr<expression_base> expr2) :
mFirstExpression(expr1),
mOperator(op),
mSecondExpression(expr2)
{}

JamomaExpressionComposition::~JamomaExpressionComposition()
{}
# pragma mark -
# pragma mark Execution

bool JamomaExpressionComposition::evaluate() const
{
  return do_evaluation(mFirstExpression->evaluate(), mSecondExpression->evaluate());
}

void JamomaExpressionComposition::update() const
{
  mFirstExpression->update();
  mSecondExpression->update();
}

# pragma mark -
# pragma mark Operator

bool JamomaExpressionComposition::operator== (const expression_base& expression) const
{
  if (expression.getType() == expression_base::Type::COMPOSITION)
  {
    const JamomaExpressionComposition e = dynamic_cast<const JamomaExpressionComposition&>(expression);
    return *mFirstExpression == *e.mFirstExpression && mOperator == e.mOperator && *mSecondExpression == *e.mSecondExpression;
  }
  else
    return false;
}

bool JamomaExpressionComposition::operator!= (const expression_base& expression) const
{
  if (expression.getType() == expression_base::Type::COMPOSITION)
  {
    const JamomaExpressionComposition e = dynamic_cast<const JamomaExpressionComposition&>(expression);
    return *mFirstExpression != *e.mFirstExpression || mOperator != e.mOperator || *mSecondExpression != *e.mSecondExpression;
  }
  else
    return true;
}

# pragma mark -
# pragma mark Callback Container

void JamomaExpressionComposition::onFirstCallbackAdded()
{
  // start first expression observation
  mFirstResultCallbackIndex = mFirstExpression->addCallback([&] (bool result) { firstResultCallback(result); });

  // start second expression observation
  mSecondResultCallbackIndex = mSecondExpression->addCallback([&] (bool result) { secondResultCallback(result); });
}

void JamomaExpressionComposition::onRemovingLastCallback()
{
  // stop first expression observation
  mFirstExpression->removeCallback(mFirstResultCallbackIndex);

  // stop second expression observation
  mSecondExpression->removeCallback(mSecondResultCallbackIndex);
}

# pragma mark -
# pragma mark Accessors

const std::shared_ptr<expression_base> & JamomaExpressionComposition::getFirstOperand() const
{
  return mFirstExpression;
}

expression_composition::Operator JamomaExpressionComposition::getOperator() const
{
  return mOperator;
}

const std::shared_ptr<expression_base> & JamomaExpressionComposition::getSecondOperand() const
{
  return mSecondExpression;
}

# pragma mark -
# pragma mark Implementation Specific

bool JamomaExpressionComposition::do_evaluation(bool first, bool second) const
{
  switch (mOperator)
  {
    case Operator::AND :
    {
      return first && second;
    }
    case Operator::OR :
    {
      return first || second;
    }
    case Operator::XOR :
    {
      return first ^ second;
    }
    default :
      return false;
  }
}

void JamomaExpressionComposition::firstResultCallback(bool first_result)
{
  bool result = do_evaluation(first_result, mSecondExpression->evaluate());
  send(result);
}

void JamomaExpressionComposition::secondResultCallback(bool second_result)
{
  bool result = do_evaluation(mFirstExpression->evaluate(), second_result);
  send(result);
}
}
