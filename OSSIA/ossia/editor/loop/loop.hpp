#pragma once

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia_export.h>

namespace ossia
{

struct time_value;
/**
 * @brief The loop class
 *
 * A time process that allows looping around a time_constraint.
 * First the start time_node is checked.
 * Then the main constraint executes.
 * Then the end time_node is checked.
 */
class OSSIA_EXPORT loop final :
    public time_process
{
public:
  /*! factory
   \param const #TimeValue& duration of the pattern #time_constraint
   \param #time_constraint::ExecutionCallback to be notified at each step of the
   loop
   \param #time_event::ExecutionCallback to get start pattern #time_event's
   status
   back
   \param #time_event::ExecutionCallback to get end pattern #time_event's status
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

  /*! get the pattern #time_constraint
   \return std::shared_ptr<TimeConstraint> */
  const std::shared_ptr<time_constraint>
  getPatternTimeConstraint() const;

  /*! get the pattern start #time_node
   \return std::shared_ptr<TimeNode> */
  const std::shared_ptr<time_node> getPatternStartTimeNode() const;

  /*! get the pattern end #time_node
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
