#include <ossia/editor/expression/detail/Expression_impl.hpp>

namespace ossia
{
namespace expressions
{
std::shared_ptr<expression_base> expression_base::create(bool result)
{
  return std::make_shared<::impl::JamomaExpression>(result);
}

expression_base::~expression_base() = default;

std::shared_ptr<expression_base> expression_false()
{
  return expression_base::create(false);
}

std::shared_ptr<expression_base> expression_true()
{
  return expression_base::create(true);
}
}
}
