/*!
 * \file ExpressionNot.h
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
class OSSIA_EXPORT expression_not :
    public callback_container<expression_result_callback>
{
public:
  expression_not(std::unique_ptr<expression_base>);

  virtual ~expression_not();

  bool evaluate() const;
  void update() const;

  expression_base& getExpression() const;

private:
  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

  void resultCallback(bool result);

  std::unique_ptr<expression_base> mExpression;
  expression_callback_iterator mResultCallbackIndex;
};
}
}
