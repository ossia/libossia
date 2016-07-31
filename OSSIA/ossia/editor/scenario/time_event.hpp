#pragma once

#include <functional>
#include <memory>
#include <string>

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia_export.h>

namespace ossia
{
class state;
class time_constraint;
class time_node;

/**
 * @brief The time_event class
 *
 * \brief #TimeEvent is use to describe temporal structure to launch the start
 * or the end of each attached #TimeConstraint.
 *
 * \details #TimeEvent has a #State and can also be submitted to an
 * #Expression.
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

  /*! destructor */
  virtual ~time_event();

  /*! to get the event status back
   \param #Status new status */
  using ExecutionCallback = std::function<void(Status)>;

  /*! changes the callback in the event
   \param #TimeEvent::ExecutionCallback to get #TimeEvent's status back
   \details this may be unsafe to do during execution */
  virtual void setCallback(time_event::ExecutionCallback) = 0;

  /*! make the event happen to propagate the execution to next TimeConstraints
   \details the event have to be in PENDING status to call this method
   otherwise
   it will raise a runtime_error
   \details turning the event' status into HAPPENED will call the callback to
   notify its owner */
  virtual void happen() = 0;

  /*! dispose the event to not propagate the execution to next TimeConstraints
   \details the event have to be in PENDING status to call this method
   otherwise
   it will raise a runtime_error
   \details turning the event' status into DISPOSED will call the callback to
   notify its owner */
  virtual void dispose() = 0;

  /*! add a sub state into the state of the event
   \param std::shared_ptr<#State> to add */
  virtual void addState(state&&) = 0;

  /*! remove a sub state from the state of the event
   \param std::shared_ptr<#State> to remove */
  virtual void removeState(const state&) = 0;

  /*! get the #TimeNode where the event is
   \return std::shared_ptr<#TimeNode> */
  virtual const std::shared_ptr<time_node>& getTimeNode() const = 0;

  /*! get the state of the event
  \return std::shared_ptr<#State> */
  virtual const state& getState() const = 0;

  /*! get the expression of the event
  \return std::shared_ptr<#Expression> */
  virtual const expression& getExpression() const = 0;

  /*! set the expression of the event
   \param std::shared_ptr<#Expression>
   \return #TimeEvent the event */
  virtual time_event& setExpression(expression_ptr) = 0;

  /*! get the status of the event
   \return #Status */
  virtual Status getStatus() const = 0;

  /*! get previous time contraints attached to the event
   \return #Container<#TimeConstraint> */
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
   \return #Container<#TimeConstraint> */
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

private:
  ptr_container<time_constraint> m_previousTimeConstraints;
  ptr_container<time_constraint> m_nextTimeConstraints;
};
}
