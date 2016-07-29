#include <ossia/editor/expression/expression_bool.hpp>

namespace ossia
{
namespace expressions
{

expression_bool::~expression_bool() = default;

expression_bool::expression_bool(bool result):
  mResult{result}
{

}

bool expression_bool::operator==(const expression_base& exp) const
{
  if (exp.getType() == expression_base::Type::BOOL)
  {
    const expression_bool e = dynamic_cast<const expression_bool&>(exp);
    return mResult == e.mResult;
  }
  else
    return false;
}

bool expression_bool::operator!=(const expression_base& exp) const
{
  if (exp.getType() == expression_base::Type::BASE)
  {
    const expression_bool e = dynamic_cast<const expression_bool&>(exp);
    return m_result != e.m_result;
  }
  else
  {
    return true;
  }
}

}
}
