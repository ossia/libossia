#pragma once
#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
class OSSIA_EXPORT expression_bool final :
    public callback_container<expression_result_callback>
{
  bool mResult;

public:
  expression_bool(bool result);

  ~expression_bool();

  bool evaluate() const { return mResult; }
  void update() const { }

};

const expression_bool expression_true{true};
const expression_bool expression_false{false};

}
}
