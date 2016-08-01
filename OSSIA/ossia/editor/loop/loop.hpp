#pragma once

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia_export.h>

namespace ossia
{

class time_value;

class OSSIA_EXPORT loop : public time_process
{
public:
  /*! factory
   \param const #TimeValue& duration of the pattern #TimeConstraint
   \param #TimeConstraint::ExecutionCallback to be notified at each step of the
   loop
   \param #TimeEvent::ExecutionCallback to get start pattern #TimeEvent's
   status
   back
   \param #TimeEvent::ExecutionCallback to get end pattern #TimeEvent's status
   back
   \return a new loop */
  static std::shared_ptr<loop> create(
      time_value, time_constraint::ExecutionCallback,
      time_event::ExecutionCallback, time_event::ExecutionCallback);

  /*! destructor */
  virtual ~loop();

  /*! get the pattern #TimeConstraint
   \return std::shared_ptr<TimeConstraint> */
  virtual const std::shared_ptr<time_constraint>
  getPatternTimeConstraint() const = 0;

  /*! get the pattern start #TimeNode
   \return std::shared_ptr<TimeNode> */
  virtual const std::shared_ptr<time_node> getPatternStartTimeNode() const = 0;

  /*! get the pattern end #TimeNode
   \return std::shared_ptr<TimeNode> */
  virtual const std::shared_ptr<time_node> getPatternEndTimeNode() const = 0;
};
}
