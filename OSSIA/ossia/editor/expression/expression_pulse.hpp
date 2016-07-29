/*!
 * \file ExpressionPulse.h
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
#include <ossia/network/base/node.hpp>
#include <ossia_export.h>

namespace ossia
{
class Destination;

namespace expressions
{
class OSSIA_EXPORT expression_pulse : public expression_base
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param #Destination*
   \return std::shared_ptr<#ExpressionPulse> */
  static std::shared_ptr<expression_pulse> create(const Destination&);

  /*! destructor */
  virtual ~expression_pulse();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! evaluate if a destination value change have been observed since expression creation or since the last update
   \return true after the destination being updated */
  virtual bool evaluate() const override = 0;

  /*! reset the evaluation to false until the next destination update */
  virtual void update() const override = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the type of the expression
   \return #Type of the expression */
  expression_base::Type getType() const override final
  {return expression_base::Type::PULSE;}

  /*! get destination
   \return const #Destination* expression */
  virtual const Destination& getDestination() const = 0;

};
}
}
