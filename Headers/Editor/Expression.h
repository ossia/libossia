/*!
 * \file Expression.h
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
#include <functional>

#include "Misc/CallbackContainer.h"

namespace OSSIA
{
  
/*! to get the result back */
using ResultCallback = std::function<void(bool)>;

class Expression
{

public:
  
# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param bool result to return
   \return std::shared_ptr<#Expression> */
  static std::shared_ptr<Expression> create(bool = false);
  
  /*! destructor */
  virtual ~Expression() = default;
  
# pragma mark -
# pragma mark Execution
  
  /*! evaluate the expression 
   \return bool result of the evaluation */
  virtual bool evaluate() const = 0;
  
};
  
static std::shared_ptr<Expression> ExpressionFalse = Expression::create(false);
static std::shared_ptr<Expression> ExpressionTrue = Expression::create(true);
  
}

