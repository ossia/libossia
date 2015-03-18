/*!
 * \file ExpressionNot.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef EXPRESSIONNOT_H_
#define EXPRESSIONNOT_H_

#include "Editor/Expression.h"

namespace OSSIA {

class ExpressionNot : public Expression {

public:

  // Factories, destructor
  static std::shared_ptr<ExpressionNot> create(std::shared_ptr<Expression>);
  virtual std::shared_ptr<ExpressionNot> clone() const = 0;
  virtual ~ExpressionNot() = default;

  // Lecture
  virtual bool evaluate() const override = 0;

  // Accessors
  virtual const std::shared_ptr<Expression> & getExpression() const = 0;

};

}

#endif // EXPRESSIONNOT_H_
