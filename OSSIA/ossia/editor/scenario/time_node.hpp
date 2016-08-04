#pragma once

#include <memory>
#include <string>

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia_export.h>

namespace ossia
{

class expression_base;
class state;
class time_event;
class time_value;

/**
 * \brief #TimeNode is use to describe temporal structure to synchronize each
 * attached #TimeEvent evaluation.
 *
 * \details #TimeNode is also a #TimeEvent container.
 */
class OSSIA_EXPORT time_node final :
    public std::enable_shared_from_this<time_node>
{
  public:
    /*! to be notified when it is triggered */
    using execution_callback = std::function<void()>;

  private:
    time_node::execution_callback mCallback;

    ossia::expression_ptr mExpression;
    expressions::expression_callback_iterator mResultCallbackIndex;

    ptr_container<time_event> mPendingEvents;

    time_value mSimultaneityMargin;


    bool mObserveExpression;
    bool mCallbackSet = false;

  public:
    using iterator = ptr_container<time_event>::iterator;
    using const_iterator = ptr_container<time_event>::const_iterator;

    /*
    \param #TimeNode::ExecutionCallback to be be notified when the #TimeNode is
    triggered
    \return std::shared_ptr<#TimeNode> */
    time_node(time_node::execution_callback callback = {});

    /*! destructor */
    ~time_node();

    /*! changes the callback in the #TimeNode
   \param #TimeNode::ExecutionCallback to be be notified when the #TimeNode is
   triggered
   \details this may be unsafe to do during execution */
    void setCallback(time_node::execution_callback);

    /*! evaluate all #TimeEvent's to make them to happen or to dispose them
   \return boolean true if the operation succeeded */
    bool trigger();

    /*! get the date
   \details the date is the sum of its previous #TimeConstraint durations
   \details a #TimeNode with na previous #TimeConstraints have a date equals to
   0.
   \return #TimeValue the date */
    time_value getDate() const;

    /*! get the expression of the #TimeNode
   \return std::shared_ptr<#Expression> */
    const expression& getExpression() const;

    /*! set the expression of the #TimeNode
   \details setting the expression to ExpressionTrue will defer the evaluation
   on #TimeEvent's expression
   \details setting the expression to ExpressionFalse will mute TimeNode
   execution
   \param std::shared_ptr<#Expression>
   \return #TimeNode the time node */
    time_node& setExpression(expression_ptr);

    /*! get the simultaneity margin
   \return #TimeValue the simultaneity margin */
    time_value getSimultaneityMargin() const;

    /*! set the simultaneity margin
   \param #TimeValue the simultaneity margin
   \return #TimeNode the time node */
    time_node& setSimultaneityMargin(time_value);

    /*! create and store a #TimeEvent
   \param #Container<#TimeEvent>::const_iterator where to store the #TimeEvent
   \param #TimeEvent::ExecutionCallback to get #TimeEvent's status back
   \param std::shared<#Expression> an optionnal #Expression to apply to the
   #TimeEvent
   \return std::shared_ptr<#TimeEvent> */
    iterator emplace(
        const_iterator, time_event::ExecutionCallback,
        expression_ptr = expressions::make_expression_true());

    /*! get the #TimeEvents of the #TimeNode
   \return #Container<#TimeEvent> */
    ptr_container<time_event>& timeEvents()
    {
      return m_timeEvents;
    }

    /*! get the #TimeEvents of the #TimeNode
   \return #Container<#TimeEvent> */
    const ptr_container<time_event>& timeEvents() const
    {
      return m_timeEvents;
    }

    // Interface to be used for set-up by other time processes
    /* process all TimeEvents to propagate execution */
    void process(ptr_container<time_event>& statusChangedEvents);

    /* is the TimeNode observing its Expression ? */
    bool isObservingExpression();

    /* enable observation of the Expression */
    void observeExpressionResult(bool);

    void resultCallback(bool result);

  private:
    ptr_container<time_event> m_timeEvents;
};
}
