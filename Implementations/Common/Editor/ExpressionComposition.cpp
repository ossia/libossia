#include "Editor/ExpressionComposition_impl.h"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<ExpressionComposition> ExpressionComposition::create(shared_ptr<Expression> first_expr, Operator op, shared_ptr<Expression> second_expr)
  {
    return make_shared<impl::JamomaExpressionComposition>(first_expr, op, second_expr);
  }


  ExpressionComposition::~ExpressionComposition() = default;

}

