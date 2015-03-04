/*!
 * \file ExpressionComposition.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef EXPRESSIONCOMPOSITION_H_
#define EXPRESSIONCOMPOSITION_H_

#include "Editor/Expression.h"

namespace OSSIA {

class ExpressionComposition : public Expression {

public:

  enum class Operator {
    NOT,
    AND,
    OR,
    XOR
  };

  // Factories, destructor
  static std::shared_ptr<ExpressionComposition>
        create(std::shared_ptr<Expression>,
               Operator,
               std::shared_ptr<Expression>/*todo = NO_EXPRESSION*/);
  virtual std::shared_ptr<ExpressionComposition> clone() const = 0;
  virtual ~ExpressionComposition() = default;

  // Lecture
  virtual bool evaluate() const override = 0;

  // Accessors //todo is it necessary ?
  virtual const std::shared_ptr<Expression> & getFirstOperand() const = 0;
  virtual const std::shared_ptr<Expression> & getSecondOperand() const = 0;
  virtual Operator getOperator() const = 0;

};

}



#endif /* EXPRESSIONCOMPOSITION_H_ */
