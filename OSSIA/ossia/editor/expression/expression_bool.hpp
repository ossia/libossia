#pragma once
#include <ossia/editor/expression/expression.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
class OSSIA_EXPORT expression_bool final :
    public expression_base
{
  bool mResult;

public:
  expression_bool(bool result);

  ~expression_bool();

  bool evaluate() const override { return mResult; }
  void update() const override { }

  bool operator== (const expression_base& exp) const override;
  bool operator!= (const expression_base& exp) const override;
  expression_base::Type getType() const override
  { return expression_base::Type::BOOL; }
};

const expression_bool expression_true{true};
const expression_bool expression_false{false};

}
}
