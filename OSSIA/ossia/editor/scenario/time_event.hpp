#pragma once

#include <functional>
#include <memory>
#include <string>

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia_export.h>

namespace ossia
{
class state;
class time_constraint;
class time_node;
/**
 * @brief The time_event class
 *
 * \brief #time_event is use to describe temporal structure to launch the start
 * or the end of each attached #time_constraint.
 *
 * \details #time_event has a #State and can also be submitted to an
 * expression.
 *
 */
class OSSIA_EXPORT time_event
{
  public:
    /*! event status */
    enum class Status
    {
      NONE,
      PENDING,
      HAPPENED,
      DISPOSED
    };

    /**
     * @brief The OffsetBehavior enum
     * Describes what happens when a parent scenario
     * does an offset beyond this event. This is useful to
     * make default cases for the scenario.
     */
    enum class OffsetBehavior
    {
        EXPRESSION_TRUE, //! The condition is considered True
        EXPRESSION_FALSE, //! The condition is considered False
        EXPRESSION //! The condition will be evaluated
    };

    /*! to get the event status back
     \param #Status new status */
    using ExecutionCallback = std::function<void(Status)>;

  private:
    time_event::ExecutionCallback mCallback;

    time_node& mTimeNode;
    state mState;
    Status mStatus;
    OffsetBehavior mOffsetBehavior{OffsetBehavior::EXPRESSION_TRUE};

    expression_ptr mExpression;

public:
    time_event(
        time_event::ExecutionCallback,
        time_node& aTimeNode,
        expression_ptr anExpression);

  /*! destructor */
  ~time_event();


  /*! changes the callback in the event
   \param #time_event::ExecutionCallback to get #time_event's status back
   \details this may be unsafe to do during execution */
  void setCallback(time_event::ExecutionCallback) ;

  /*! make the event happen to propagate the execution to next TimeConstraints
   \details the event have to be in PENDING status to call this method
   otherwise
   it will raise a runtime_error
   \details turning the event' status into HAPPENED will call the callback to
   notify its owner */
  void happen() ;

  /*! dispose the event to not propagate the execution to next TimeConstraints
   \details the event have to be in PENDING status to call this method
   otherwise
   it will raise a runtime_error
   \details turning the event' status into DISPOSED will call the callback to
   notify its owner */
  void dispose() ;

  /*! add a sub state into the state of the event
   \param std::shared_ptr<#State> to add */
  void addState(state_element&&) ;

  /*! remove a sub state from the state of the event
   \param std::shared_ptr<#State> to remove */
  void removeState(const state_element&) ;

  /*! get the #time_node where the event is
   \return std::shared_ptr<#time_node> */
  time_node& getTimeNode() const ;

  /*! get the state of the event
  \return std::shared_ptr<#State> */
  const state& getState() const ;

  /*! get the expression of the event
  \return std::shared_ptr<expression> */
  const expression& getExpression() const ;

  /*! set the expression of the event
   \param std::shared_ptr<expression>
   \return #time_event the event */
  time_event& setExpression(expression_ptr) ;

  /*! get the status of the event
   \return #Status */
  Status getStatus() const ;

  /**
   * @brief getOffsetValue Returns the value of the condition if
   * we are offseting past this time event.
   */
  OffsetBehavior getOffsetBehavior() const;

  /**
   * @brief setOffsetValue Sets the value of the condition if we are offseting
   * past this time event.
   */
  time_event& setOffsetBehavior(OffsetBehavior);

  /*! get previous time contraints attached to the event
   \return #Container<#time_constraint> */
  ptr_container<time_constraint>& previousTimeConstraints()
  {
    return m_previousTimeConstraints;
  }

  /*! get previous time contraints attached to the event
   \return #Container<#TimeProcess> */
  const ptr_container<time_constraint>& previousTimeConstraints() const
  {
    return m_previousTimeConstraints;
  }

  /*! get next time contraints attached to the event
   \return #Container<#time_constraint> */
  ptr_container<time_constraint>& nextTimeConstraints()
  {
    return m_nextTimeConstraints;
  }

  /*! get next time contraints attached to the event
   \return #Container<#TimeProcess> */
  const ptr_container<time_constraint>& nextTimeConstraints() const
  {
    return m_nextTimeConstraints;
  }

  void setStatus(Status status);

  void reset();

  /* To be called before deletion, to break the shared_ptr cycle */
  void cleanup();
private:
  ptr_container<time_constraint> m_previousTimeConstraints;
  ptr_container<time_constraint> m_nextTimeConstraints;
};
}
