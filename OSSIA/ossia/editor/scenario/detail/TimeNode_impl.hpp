/*!
 * \file TimeNode_impl.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include "TimeEvent_impl.hpp" // because the TimeNode::emplace method is a JamomaTimeEvent factory

using namespace ossia;


namespace impl
{
class JamomaTimeNode final : public time_node, public std::enable_shared_from_this<JamomaTimeNode>
{

private:

# pragma mark -
# pragma mark Implementation specific

  time_node::execution_callback   mCallback;

  std::shared_ptr<expression_base>        mExpression;
  bool                          mObserveExpression;
  bool                          mCallbackSet = false;
  expression_base::iterator          mResultCallbackIndex;

  time_value                     mSimultaneityMargin;

  ptr_container<time_event>          mPendingEvents;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaTimeNode(time_node::execution_callback);

  ~JamomaTimeNode();

# pragma mark -
# pragma mark Execution

  void setCallback(time_node::execution_callback) override;

  bool trigger() override;

# pragma mark -
# pragma mark Accessors

  time_value getDate() const override;

  const std::shared_ptr<expression_base> & getExpression() const override;

  time_node & setExpression(const std::shared_ptr<expression_base>) override;

  time_value getSimultaneityMargin() const override;

  time_node & setSimultaneityMargin(time_value) override;

# pragma mark -
# pragma mark TimeEvents

  iterator emplace(const_iterator,
                   time_event::ExecutionCallback,
                   std::shared_ptr<expression_base> = ExpressionTrue()) override;

# pragma mark -
# pragma mark Implementation specific

  /* process all TimeEvents to propagate execution */
  void process(ptr_container<time_event>& statusChangedEvents);

  /* is the TimeNode observing its Expression ? */
  bool isObservingExpression();

  /* enable observation of the Expression */
  void observeExpressionResult(bool);

private:

  void resultCallback(bool result);
};
}
