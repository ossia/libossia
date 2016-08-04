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
    loop(
        time_value, time_constraint::ExecutionCallback,
        time_event::ExecutionCallback, time_event::ExecutionCallback);

  /*! destructor */
  ~loop();

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  state_element offset(time_value) override;
  state_element state() override;

  /*! get the pattern #TimeConstraint
   \return std::shared_ptr<TimeConstraint> */
  const std::shared_ptr<time_constraint>
  getPatternTimeConstraint() const;

  /*! get the pattern start #TimeNode
   \return std::shared_ptr<TimeNode> */
  const std::shared_ptr<time_node> getPatternStartTimeNode() const;

  /*! get the pattern end #TimeNode
   \return std::shared_ptr<TimeNode> */
  const std::shared_ptr<time_node> getPatternEndTimeNode() const;

  private:
  void PatternConstraintCallback(time_value, time_value, const ossia::state&);

  void PatternStartEventCallback(time_event::Status);

  void PatternEndEventCallback(time_event::Status);

  std::shared_ptr<time_node> mPatternStartNode;
  time_event::ExecutionCallback mPatternStartEventCallback;

  std::shared_ptr<time_node> mPatternEndNode;
  time_event::ExecutionCallback mPatternEndEventCallback;

  std::shared_ptr<time_constraint> mPatternConstraint;
  time_constraint::ExecutionCallback mPatternConstraintCallback;

  ossia::state mCurrentState; // an internal State to return on state call
  ossia::state mOffsetState;  // an internal State built when offset is called

};
}
