#pragma once

#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_value.hpp>
using namespace ossia;

namespace ossia
{
namespace detail
{
class time_node_impl final
    : public ossia::time_node,
      public std::enable_shared_from_this<time_node_impl>
{

private:
  time_node::execution_callback mCallback;

  ossia::expression_ptr mExpression;
  bool mObserveExpression;
  bool mCallbackSet = false;
  expressions::expression_callback_iterator mResultCallbackIndex;

  time_value mSimultaneityMargin;

  ptr_container<time_event> mPendingEvents;

public:
  time_node_impl(time_node::execution_callback);

  ~time_node_impl();

  /* process all TimeEvents to propagate execution */
  void process(ptr_container<time_event>& statusChangedEvents);

private:
  void setCallback(time_node::execution_callback) override;

  bool trigger() override;
  time_value getDate() const override;

  const expression& getExpression() const override;

  time_node& setExpression(expression_ptr) override;

  time_value getSimultaneityMargin() const override;

  time_node& setSimultaneityMargin(time_value) override;
  iterator emplace(
      const_iterator, time_event::ExecutionCallback,
      ossia::expression_ptr /*= ExpressionTrue()*/) override;


  /* is the TimeNode observing its Expression ? */
  bool isObservingExpression();

  /* enable observation of the Expression */
  void observeExpressionResult(bool);

  void resultCallback(bool result);
};
}
}
