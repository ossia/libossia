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

#include <ossia/detail/callback_container.hpp>
#include <ossia_export.h>

namespace OSSIA
{

/*! to get the result back
 \param the returned result */
using ResultCallback = std::function<void(bool)>;

class OSSIA_EXPORT Expression : public CallbackContainer<ResultCallback>
{

public:

  using iterator = typename CallbackContainer<ResultCallback>::iterator;

#if 0
# pragma mark -
# pragma mark Enumerations
#endif

  /*! type of expression */
  enum class Type
  {
    BASE,
    ATOM,
    COMPOSITION,
    NOT,
    PULSE
  };

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param bool result to return
   \return std::shared_ptr<#Expression> */
  static std::shared_ptr<Expression> create(bool = false);

  /*! destructor */
  virtual ~Expression();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! evaluate the expression
   \return bool result of the evaluation */
  virtual bool evaluate() const = 0;

  /*! pull the value of any #Destination operand */
  virtual void update() const = 0;

#if 0
# pragma mark -
# pragma mark Operator
#endif

  /*! equal operator */
  virtual bool operator== (const Expression&) const = 0;

  /*! different operator */
  virtual bool operator!= (const Expression&) const = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the type of the expression
   \return #Type of the expression */
  virtual Expression::Type getType() const
  {return Expression::Type::BASE;}
};

OSSIA_EXPORT std::shared_ptr<Expression> ExpressionFalse();
OSSIA_EXPORT std::shared_ptr<Expression> ExpressionTrue();

}

