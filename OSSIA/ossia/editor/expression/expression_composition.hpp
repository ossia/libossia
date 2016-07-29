/*!
 * \file ExpressionComposition.h
 *
 * \defgroup Editor
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <memory>

#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
class OSSIA_EXPORT expression_composition final :
    public callback_container<expression_result_callback>
{
public:
  enum class Operator
  {
    AND,
    OR,
    XOR
  };

  expression_composition(
      std::unique_ptr<expression_base> expr1,
      Operator op,
      std::unique_ptr<expression_base> expr2);

  virtual ~expression_composition();

  bool evaluate() const;

  void update() const;

  expression_base& getFirstOperand() const;
  Operator getOperator() const;
  expression_base&  getSecondOperand() const;

private:
  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

  bool do_evaluation(bool first, bool second) const;

  void firstResultCallback(bool first_result);
  void secondResultCallback(bool second_result);

  std::unique_ptr<expression_base> mFirstExpression;
  std::unique_ptr<expression_base> mSecondExpression;

  expression_callback_iterator mFirstResultCallbackIndex;
  expression_callback_iterator mSecondResultCallbackIndex;

  Operator  mOperator;
};
}
}
