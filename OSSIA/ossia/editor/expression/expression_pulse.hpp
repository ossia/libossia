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

#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia_export.h>

namespace ossia
{
class Destination;

namespace expressions
{
class OSSIA_EXPORT expression_pulse :
    public callback_container<expression_result_callback>
{
public:
  expression_pulse(const Destination& destination);
  expression_pulse(expression_pulse&& other) = delete;
  expression_pulse& operator=(expression_pulse&& other) = delete;
  expression_pulse(const expression_pulse& other) = delete;
  expression_pulse& operator=(const expression_pulse& other) = delete;

  virtual ~expression_pulse();

  /*! evaluate if a destination value change have been observed since expression creation or since the last update
   \return true after the destination being updated */
  bool evaluate() const;

  /*! reset the evaluation to false until the next destination update */
  void update() const;

  const Destination& getDestination() const;

private:
  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

  void destinationCallback(const value& value);

  Destination         mDestination;
  mutable bool        mResult{};

  net::address::callback_index   mDestinationCallbackIndex;

};
}
}
