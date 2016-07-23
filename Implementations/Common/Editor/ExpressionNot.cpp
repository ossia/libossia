#include "Editor/ExpressionNot_impl.h"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<ExpressionNot> ExpressionNot::create(shared_ptr<Expression> expr)
  {
    return make_shared<impl::JamomaExpressionNot>(expr);
  }

  ExpressionNot::~ExpressionNot() = default;
}
