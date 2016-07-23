#include "Expression_impl.h"

namespace impl
{
JamomaExpression::JamomaExpression(bool result) :
m_result(result)
{}

JamomaExpression::~JamomaExpression()
{}

# pragma mark -
# pragma mark Execution

bool JamomaExpression::evaluate() const
{
  return m_result;
}

void JamomaExpression::update() const
{}

# pragma mark -
# pragma mark Operator

bool JamomaExpression::operator== (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::BASE)
  {
    const JamomaExpression e = dynamic_cast<const JamomaExpression&>(expression);
    return m_result == e.m_result;
  }
  else
    return false;
}

bool JamomaExpression::operator!= (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::BASE)
  {
    const JamomaExpression e = dynamic_cast<const JamomaExpression&>(expression);
    return m_result != e.m_result;
  }
  else
    return true;
}

# pragma mark -
# pragma mark Callback Container

//! \todo shouldn't they stay unimplemented ?
Expression::iterator JamomaExpression::addCallback(ResultCallback)
{
  return callbacks().end();
}

void JamomaExpression::removeCallback(Expression::iterator callback)
{}
}
