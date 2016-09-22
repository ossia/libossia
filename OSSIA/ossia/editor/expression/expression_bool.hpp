#pragma once
#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
/**
 * @brief expression_bool : a constant boolean value.
 * @see expression.hpp
 */
class OSSIA_EXPORT expression_bool final :
    public expression_callback_container
{
  bool mResult;

public:
  expression_bool(bool result);
  expression_bool(expression_bool&& other) = delete;
  expression_bool& operator=(expression_bool&& other) = delete;

  expression_bool(const expression_bool& other) = delete;
  expression_bool& operator=(const expression_bool& other) = delete;

  ~expression_bool();

  bool evaluate() const
  {
    return mResult;
  }
  void update() const
  {
  }
};
}
}
