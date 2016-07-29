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

#include <ossia/editor/expression/expression.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
class OSSIA_EXPORT expression_not : public expression_base
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param std::shared_ptr<Expression>
   \return std::shared_ptr<#ExpressionNot> */
  static std::shared_ptr<expression_not> create(std::shared_ptr<expression_base>);

  /*! destructor */
  virtual ~expression_not();

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
  {return expression_base::Type::NOT;}

  /*! get expression
   \return const std::shared_ptr<#Expression> expression */
  virtual const std::shared_ptr<expression_base> & getExpression() const = 0;

};
}
}
