/*!
 * \file ExpressionAtom.h
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

#include "Editor/Expression.h"
#include "Network/AddressValue.h"
#include <memory>

namespace OSSIA {

class ExpressionAtom : public Expression {

    public:

      enum class Operator {
        EQUAL,
        GREATER_THAN,
        LOWER_THAN,
        GREATER_THAN_OR_EQUAL,
        LOWER_THAN_OR_EQUAL
      };

    # pragma mark -
# pragma mark Life cycle
      static std::shared_ptr<ExpressionAtom> create(
          std::shared_ptr<ExpressionValue>,
          Operator,
          std::shared_ptr<ExpressionValue>);
      virtual std::shared_ptr<ExpressionAtom> clone() const = 0;
      virtual ~ExpressionAtom() = default;

    # pragma mark -
# pragma mark Execution
      virtual bool evaluate() const override = 0;

    # pragma mark -
# pragma mark Accessors //TODO is it necessary ?
      virtual const std::shared_ptr<ExpressionValue> & getFirstOperand() const = 0;
      virtual const std::shared_ptr<ExpressionValue> & getSecondOperand() const = 0;
      virtual Operator getOperator() const = 0;

};

}
