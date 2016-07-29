#include <ossia/editor/expression/detail/ExpressionNot_impl.hpp>

namespace ossia
{
namespace expressions
{
  std::shared_ptr<expression_not> expression_not::create(std::shared_ptr<expression_base> expr)
  {
    return std::make_shared<::impl::JamomaExpressionNot>(expr);
  }

  expression_not::~expression_not() = default;
}
}
