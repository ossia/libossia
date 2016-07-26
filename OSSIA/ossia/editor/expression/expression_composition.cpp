#include <ossia/editor/expression/detail/ExpressionComposition_impl.hpp>

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  std::shared_ptr<ExpressionComposition> ExpressionComposition::create(
      std::shared_ptr<Expression> first_expr,
      Operator op,
      std::shared_ptr<Expression> second_expr)
  {
    return std::make_shared<impl::JamomaExpressionComposition>(first_expr, op, second_expr);
  }


  ExpressionComposition::~ExpressionComposition() = default;

}

