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
  virtual void update() = 0;
  virtual bool evaluate() const = 0;
  virtual void onFirstCallbackAdded(expression_generic&) = 0;
  virtual void onRemovingLastCallback(expression_generic&) = 0;
};

class OSSIA_EXPORT expression_generic final :
    public expression_callback_container
{
public:
  std::unique_ptr<expression_generic_base> expr;

  void update() const
  { expr->update(); }
  bool evaluate() const
  { return expr->evaluate(); }

private:
  void onFirstCallbackAdded() override
  { expr->onFirstCallbackAdded(*this); }
  void onRemovingLastCallback() override
  { expr->onRemovingLastCallback(*this); }
};
}
}
