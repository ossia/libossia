#include "detail/ExpressionAtom_impl.hpp"

namespace OSSIA
{
  std::shared_ptr<ExpressionAtom> ExpressionAtom::create(
      const Value& value1,
      Operator op,
      const Value& value2)
  {
    return std::make_shared<impl::JamomaExpressionAtom>(value1, op, value2);
  }

  ExpressionAtom::~ExpressionAtom() = default;
}
