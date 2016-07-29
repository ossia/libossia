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

namespace ossia
{
namespace expressions
{
/*! to get the result back
 \param the returned result */
using expression_result_callback = std::function<void(bool)>;

class OSSIA_EXPORT expression_base : public callback_container<expression_result_callback>
{
public:
  using iterator = typename callback_container<expression_result_callback>::iterator;

#if 0
# pragma mark -
# pragma mark Enumerations
#endif

  /*! type of expression */
  enum class Type
  {
    BOOL,
    ATOM,
    COMPOSITION,
    NOT,
    PULSE
  };

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! destructor */
  virtual ~expression_base();

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
  virtual bool operator== (const expression_base&) const = 0;

  /*! different operator */
  virtual bool operator!= (const expression_base&) const = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the type of the expression
   \return #Type of the expression */
  virtual expression_base::Type getType() const = 0;
};
}
}
