#pragma once

#include <memory>

#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
class OSSIA_EXPORT expression_composition final
    : public expression_callback_container
{
public:
  enum class Operator
  {
    AND, //! &&
    OR, //! ||
    XOR //! ^
  };

  expression_composition(
      expression_ptr expr1, Operator op, expression_ptr expr2);
  expression_composition(expression_composition&& other) = delete;
  expression_composition& operator=(expression_composition&& other) = delete;

  expression_composition(const expression_composition& other) = delete;
  expression_composition& operator=(const expression_composition& other)
      = delete;

  virtual ~expression_composition();

  bool evaluate() const;

  void update() const;

  expression_base& getFirstOperand() const;
  Operator getOperator() const;
  expression_base& getSecondOperand() const;

private:
  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

  bool do_evaluation(bool first, bool second) const;

  void firstResultCallback(bool first_result);
  void secondResultCallback(bool second_result);

  expression_ptr mFirstExpression;
  expression_ptr mSecondExpression;

  expression_callback_iterator mFirstResultCallbackIndex;
  expression_callback_iterator mSecondResultCallbackIndex;

  Operator mOperator;
};
}
}
