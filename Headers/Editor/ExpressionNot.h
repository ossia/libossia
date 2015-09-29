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

#include "Editor/Expression.h"

namespace OSSIA
{

class ExpressionNot : public Expression
{

public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param std::shared_ptr<Expression>
   \return std::shared_ptr<#ExpressionNot> */
  static std::shared_ptr<ExpressionNot> create(std::shared_ptr<Expression>);
  
  /*! clone
   \return std::shared_ptr<#ExpressionNot> */
  virtual std::shared_ptr<ExpressionNot> clone() const = 0;
  
  /*! destructor */
  virtual ~ExpressionNot() = default;

# pragma mark -
# pragma mark Execution
  
  /*! evaluate the expression atom
   \return bool result of the evaluation */
  virtual bool evaluate() const override = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get expression
   \return const std::shared_ptr<#Expression> expression */
  virtual const std::shared_ptr<Expression> & getExpression() const = 0;

};
}
