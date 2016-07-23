#include "Editor/ExpressionAtom_impl.h"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<ExpressionAtom> ExpressionAtom::create(
      const Value& value1,
      Operator op,
      const Value& value2)
  {
    return make_shared<impl::JamomaExpressionAtom>(value1, op, value2);
  }

  ExpressionAtom::~ExpressionAtom() = default;
}
