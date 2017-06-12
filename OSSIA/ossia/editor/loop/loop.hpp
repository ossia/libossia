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
        time_value, time_constraint::exec_callback,
        time_event::exec_callback, time_event::exec_callback);

  /*! destructor */
  ~loop();

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  state_element offset(ossia::time_value, double pos) override;
  state_element state(ossia::time_value date, double pos) override;

  /*! get the pattern #time_constraint
   \return std::shared_ptr<TimeConstraint> */
  const std::shared_ptr<time_constraint> get_time_constraint() const;

  /*! get the pattern start #time_node
   \return std::shared_ptr<TimeNode> */
  const std::shared_ptr<time_node> get_start_timenode() const;

  /*! get the pattern end #time_node
   \return std::shared_ptr<TimeNode> */
  const std::shared_ptr<time_node> get_end_timenode() const;

  private:
  void constraint_callback(ossia::time_value, time_value, const ossia::state&);

  void start_event_callback(time_event::status);

  void end_event_callback(time_event::status);

  std::shared_ptr<time_node> m_startNode;
  time_event::exec_callback m_startCallback;

  std::shared_ptr<time_node> m_endNode;
  time_event::exec_callback m_endCallback;

  std::shared_ptr<time_constraint> m_constraint;
  time_constraint::exec_callback m_constraintCallback;

  ossia::state m_currentState; // an internal State to return on state call
  ossia::state m_offsetState;  // an internal State built when offset is called

};
}
