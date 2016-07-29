#include <ossia/editor/expression/detail/ExpressionComposition_impl.hpp>

namespace ossia
{
namespace expressions
{
  std::shared_ptr<expression_composition> expression_composition::create(
      std::shared_ptr<expression_base> first_expr,
      Operator op,
      std::shared_ptr<expression_base> second_expr)
  {
    return std::make_shared<::impl::JamomaExpressionComposition>(first_expr, op, second_expr);
  }


  expression_composition::~expression_composition() = default;

}
}
