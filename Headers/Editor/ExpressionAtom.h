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

#include "Editor/Expression.h"
#include "Editor/Value.h"

#include "Network/Address.h"
#include <ossia_export.h>

namespace OSSIA
{

class OSSIA_EXPORT ExpressionAtom : public Expression
{

public:

#if 0
# pragma mark -
# pragma mark Enumerations
#endif

  /*! type of operator */
  enum class Operator
  {
    EQUAL,
    DIFFERENT,
    GREATER_THAN,
    LOWER_THAN,
    GREATER_THAN_OR_EQUAL,
    LOWER_THAN_OR_EQUAL
  };

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory for logical condition
   \param const #Value*
   \param #Operator
   \return std::shared_ptr<#ExpressionAtom> */
  static std::shared_ptr<ExpressionAtom> create(const Value*,
                                                Operator = Operator::EQUAL,
                                                const Value* = new Impulse());

  /*! clone
   \return std::shared_ptr<#ExpressionAtom> */
  virtual std::shared_ptr<ExpressionAtom> clone() const = 0;

  /*! destructor */
  virtual ~ExpressionAtom();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! evaluate the expression atom
   \return bool result of the evaluation */
  virtual bool evaluate() const override = 0;

  /*! pull the value of any #Destination operand */
  virtual void update() const override = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the type of the expression
   \return #Type of the expression */
  Expression::Type getType() const override final
  {return Expression::Type::ATOM;}

  /*! get first operand
   \return const #Value* first operand */
  virtual const Value* getFirstOperand() const = 0;

  /*! get operator
   \return #Operator operator */
  virtual Operator getOperator() const = 0;

  /*! get second operand
   \return const #Value* second operand */
  virtual const Value* getSecondOperand() const = 0;
};

}
