#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/expression/expression_not.hpp>

namespace ossia
{
namespace expressions
{
expression_not::expression_not(expression_ptr p) : mExpression{std::move(p)}
{
  if(!mExpression)
    throw std::runtime_error("An argument to expression_not is null");
}

expression_not::~expression_not()
{
  if(!expression_callback_container::callbacks_empty())
    expression_callback_container::callbacks_clear();
}

bool expression_not::evaluate() const
{
  return !expressions::evaluate(*mExpression);
}

void expression_not::update() const
{
  expressions::update(*mExpression);
}

expression_base& expression_not::getExpression() const
{
  return *mExpression;
}

void expression_not::onFirstCallbackAdded()
{
  mResultCallbackIndex = expressions::add_callback(
      *mExpression, [&](bool result) { resultCallback(result); });
}

void expression_not::onRemovingLastCallback()
{
  expressions::remove_callback(*mExpression, mResultCallbackIndex);
}

void expression_not::resultCallback(bool result)
{
  send(!result);
}
}
}
