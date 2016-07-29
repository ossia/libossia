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

#include <ossia/editor/expression/expression.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
class OSSIA_EXPORT expression_composition : public expression_base
{

public:

#if 0
# pragma mark -
# pragma mark Enumerations
#endif

  /*! type of operator */
  enum class Operator
  {
    AND,
    OR,
    XOR
  };

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \return std::shared_ptr<#ExpressionComposition> */
  static std::shared_ptr<expression_composition> create(std::shared_ptr<expression_base>,
                                                       Operator,
                                                       std::shared_ptr<expression_base>);

  /*! destructor */
  virtual ~expression_composition();

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
  expression_base::Type getType() const override final
  {return expression_base::Type::COMPOSITION;}

  /*! get first operand
   \return const std::shared_ptr<#Expression> first operand */
  virtual const std::shared_ptr<expression_base> & getFirstOperand() const = 0;

  /*! get operator
   \return #Operator operator */
  virtual Operator getOperator() const = 0;

  /*! get second operand
   \return const std::shared_ptr<#Expression> second operand */
  virtual const std::shared_ptr<expression_base> & getSecondOperand() const = 0;
};
}
}
