/*!
 * \file ExpressionAtom.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef EXPRESSIONATOM_H_
#define EXPRESSIONATOM_H_

#include "Editor/Expression.h"
#include "Network/AddressValue.h"
#include <memory>
namespace OSSIA {

class ExpressionAtom : public Expression {

public:

  enum class Operator {
	IMPULSE,
	EQUAL,
	GREATER_THAN,
	LOWER_THAN,
	GREATER_THAN_OR_EQUAL,
	LOWER_THAN_OR_EQUAL
  };

  // Factories, destructor
  static std::shared_ptr<ExpressionAtom> create(std::shared_ptr<Address>,
						Operator = Operator::IMPULSE,
						AddressValue operand = AddressValueType::NO_VALUE);
  virtual std::shared_ptr<ExpressionAtom> clone() const = 0;
  virtual ~ExpressionAtom() = default;

  // Lecture
  virtual bool evaluate() const override = 0;

  // Accessors //todo is it necessary ?
  virtual const std::shared_ptr<Address> & getAddress() const = 0;
  virtual Operator getOperator() const = 0;
  virtual AddressValue getOperand() const = 0;

};

}



#endif /* EXPRESSIONATOM_H_ */
