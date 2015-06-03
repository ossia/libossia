/*!
 * \file ExpressionComposition.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <memory>

#include "Editor/Expression.h"

namespace OSSIA {

class ExpressionComposition : public Expression {

    public:

      enum class Operator {
        AND,
        OR,
        XOR
      };

      // Constructors, destructor, cloning
      static std::shared_ptr<ExpressionComposition> create(
          std::shared_ptr<Expression>,
          Operator,
          std::shared_ptr<Expression>);
      virtual std::shared_ptr<ExpressionComposition> clone() const = 0;
      virtual ~ExpressionComposition() = default;

      // Execution
      virtual bool evaluate() const override = 0;

      // Accessors //TODO is it necessary ?
      virtual const std::shared_ptr<Expression> & getFirstOperand() const = 0;
      virtual const std::shared_ptr<Expression> & getSecondOperand() const = 0;
      virtual Operator getOperator() const = 0;

};

}
