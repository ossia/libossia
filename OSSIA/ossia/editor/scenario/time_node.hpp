#pragma once

#include <memory>
#include <string>

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <nano_signal_slot.hpp>
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
    using iterator = ptr_container<time_event>::iterator;
    using const_iterator = ptr_container<time_event>::const_iterator;

    /*
    \param #TimeNode::ExecutionCallback to be be notified when the #TimeNode is
    triggered
    \return std::shared_ptr<#TimeNode> */
    time_node();

    /*! destructor */
    ~time_node();

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

    /*! create and store a #TimeEvent
   \param #Container<#TimeEvent>::const_iterator where to store the #TimeEvent
   \param #TimeEvent::ExecutionCallback to get #TimeEvent's status back
   \param std::shared<#Expression> an optionnal #Expression to apply to the
   #TimeEvent
   \return std::shared_ptr<#TimeEvent> */
    iterator emplace(
        const_iterator, time_event::ExecutionCallback,
        expression_ptr = expressions::make_expression_true());
    iterator insert(
        const_iterator, std::shared_ptr<time_event>);
    void remove(const std::shared_ptr<time_event>&);

    /*! get the #TimeEvents of the #TimeNode
   \return #Container<#TimeEvent> */
    ptr_container<time_event>& timeEvents()
    {
      return mTimeEvents;
    }

    /*! get the #TimeEvents of the #TimeNode
   \return #Container<#TimeEvent> */
    const ptr_container<time_event>& timeEvents() const
    {
      return mTimeEvents;
    }

    // Interface to be used for set-up by other time processes
    /* process all TimeEvents to propagate execution */
    void process(ptr_container<time_event>& statusChangedEvents);

    /* is the TimeNode observing its Expression ? */
    bool isObservingExpression();

    /* enable observation of the Expression */
    void observeExpressionResult(bool);

    void resultCallback(bool result);

    void reset();

    /* To be called before deletion, to break the shared_ptr cycle */
    void cleanup();

    /*! Execution callbacks
     *
     * Used to be notified when the #TimeNode is triggered.
     *
     * \details This is not thread-safe
     */
    callback_container<std::function<void()>> triggered;


  private:
    ossia::expression_ptr mExpression;

    ptr_container<time_event> mTimeEvents;
    ptr_container<time_event> mPendingEvents;


    expressions::expression_callback_iterator mResultCallbackIndex;

    bool mObserveExpression = false;
    bool mCallbackSet = false;

};
}
