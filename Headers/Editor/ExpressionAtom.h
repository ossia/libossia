/*!
 * \file ExpressionAtom.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
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

      // Factories, destructor
      static std::shared_ptr<ExpressionAtom> create(
          std::shared_ptr<ExpressionValue>,
          Operator,
          std::shared_ptr<ExpressionValue>);
      virtual std::shared_ptr<ExpressionAtom> clone() const = 0;
      virtual ~ExpressionAtom() = default;

      // Execution
      virtual bool evaluate() const override = 0;

      // Accessors //TODO is it necessary ?
      virtual const std::shared_ptr<ExpressionValue> & getFirstOperand() const = 0;
      virtual const std::shared_ptr<ExpressionValue> & getSecondOperand() const = 0;
      virtual Operator getOperator() const = 0;

};

}
