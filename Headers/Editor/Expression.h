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

/*! to get the result back
 \param the returned result */
using ResultCallback = std::function<void(bool)>;

class Expression : public CallbackContainer<ResultCallback>
{

public:
  
  using iterator = typename CallbackContainer<ResultCallback>::iterator;
  
# pragma mark -
# pragma mark Enumerations
  
  /*! type of expression */
  enum class Type
  {
    BASE,
    ATOM,
    COMPOSITION,
    NOT
  };
  
# pragma mark -
# pragma mark Life cycle

  /*! factory
   \param bool result to return
   \return std::shared_ptr<#Expression> */
  static std::shared_ptr<Expression> create(bool = false);

  /*! destructor */
  virtual ~Expression();

# pragma mark -
# pragma mark Execution

  /*! evaluate the expression
   \return bool result of the evaluation */
  virtual bool evaluate() const = 0;
  
  /*! pull the value of any #Destination operand */
  virtual void update() const = 0;

# pragma mark -
# pragma mark Operator
  
  /*! equal operator */
  virtual bool operator== (const Expression&) const = 0;
  
  /*! different operator */
  virtual bool operator!= (const Expression&) const = 0;
  
# pragma mark -
# pragma mark Accessors
  
  /*! get the type of the expression
   \return #Type of the expression */
  virtual Expression::Type getType() const
  {return Expression::Type::BASE;}
};

static std::shared_ptr<Expression> ExpressionFalse = Expression::create(false);
static std::shared_ptr<Expression> ExpressionTrue = Expression::create(true);

}

