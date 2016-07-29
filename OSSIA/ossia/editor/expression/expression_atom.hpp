/*!
 * \file ExpressionAtom.h
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
#include <ossia/editor/value/value.hpp>

#include <ossia/network/base/address.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
class OSSIA_EXPORT expression_atom :
    public callback_container<expression_result_callback>
{
public:
  enum class Comparator
  {
    EQUAL,
    DIFFERENT,
    GREATER_THAN,
    LOWER_THAN,
    GREATER_THAN_OR_EQUAL,
    LOWER_THAN_OR_EQUAL
  };

  expression_atom(const value& lhs,
                  Comparator op = Comparator::EQUAL,
                  const value& rhs = Impulse{});

  virtual ~expression_atom();

  bool evaluate() const;
  void update() const;

  const value& getFirstOperand() const;
  Comparator getOperator() const;
  const value& getSecondOperand() const;

private:
  void onFirstCallbackAdded();
  void onRemovingLastCallback();

  bool do_evaluation(const value& first, const value& second) const;

  void firstValueCallback(const ossia::value& value);
  void secondValueCallback(const ossia::value& value);

  ossia::value mFirstValue;
  Comparator mOperator;
  ossia::value mSecondValue;

  net::address::callback_index mFirstValueCallbackIndex;
  net::address::callback_index mSecondValueCallbackIndex;

};

}
}
