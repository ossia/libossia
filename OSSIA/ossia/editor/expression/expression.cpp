#include <ossia/editor/expression/detail/Expression_impl.hpp>

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  std::shared_ptr<Expression> Expression::create(bool result)
  {
    return std::make_shared<impl::JamomaExpression>(result);
  }

  Expression::~Expression() = default;
}


