#include <ossia/editor/expression/detail/ExpressionNot_impl.hpp>

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  std::shared_ptr<ExpressionNot> ExpressionNot::create(std::shared_ptr<Expression> expr)
  {
    return std::make_shared<impl::JamomaExpressionNot>(expr);
  }

  ExpressionNot::~ExpressionNot() = default;
}
