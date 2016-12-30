#pragma once
#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia_export.h>
#include <memory>

namespace ossia
{
namespace expressions
{
//! This expression allows the users of the library to implement their own behaviour.
struct OSSIA_EXPORT expression_generic_base
{
  virtual ~expression_generic_base();
  virtual bool update() const = 0;
  virtual bool evaluate() const = 0;
  virtual void onFirstCallbackAdded() = 0;
  virtual void onRemovingLastCallback() = 0;
};

class OSSIA_EXPORT expression_generic final :
    public expression_callback_container
{
public:
  std::unique_ptr<expression_generic_base> expr;

  bool update() const
  { return expr->update(); }
  bool evaluate() const
  { return expr->evaluate(); }

private:
  void onFirstCallbackAdded() override
  { expr->onFirstCallbackAdded(); }
  void onRemovingLastCallback() override
  { expr->onRemovingLastCallback(); }
};
}
}
