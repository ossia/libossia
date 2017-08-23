#pragma once

#include <functional>
#include <memory>
#include <string>

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/state/state_element_fwd.hpp>
#include <ossia_export.h>

/**
 * \file time_constraint.hpp
 */
namespace ossia
{
class time_event;
class time_process;
struct time_value;

/**
 * @brief The time_constraint class
 *
 * \brief #time_constraint is use to describe temporal structure precising
 * which
 * amount of time (the duration) are between two #time_events.
 *
 * \details The duration can be fixed or between a minimal and a maximal
 * duration. \n
 * #time_constraint is also a #Clock and a #TimeProcess container.
 */
class OSSIA_EXPORT time_constraint
{

public:
  auto get_date() const
  {
    return m_date;
  }
  auto get_position() const
  {
    return m_position;
  }
  auto get_offset() const
  {
    return m_offset;
  }
  auto get_speed() const
  {
    return m_speed;
  }
  void set_offset(ossia::time_value g)
  {
    m_offset = g;
  }
  void set_speed(double g)
  {
    m_speed = g;
  }
  void tick(ossia::time_value usec);
  void tick(ossia::time_value usec, double ratio);

  /*! to get the constraint execution back
   \param const #TimeValue process clock position
   \param const #TimeValue process clock date
   \param std::shared_ptr<#State> */
  using exec_callback = std::function<void(
      double, ossia::time_value, const ossia::state_element&)>;

  /*! constructor
   \details by default a #time_constraint has an infinite duration with no
   minimal duration and an infinite maximal duration.
   \param #time_constraint::ExecutionCallback to use to be notified at each
   step
   \param #std::shared_ptr<TimeEvent> event where the #time_constraint starts
   \param #std::shared_ptr<TimeEvent> event where the #time_constraint ends
   \param const #TimeValue& nominal duration of the #time_constraint
   \param const #TimeValue& minimal duration of the #time_constraint
   \param const #TimeValue& maximal duration of the #time_constraint
   \return std::shared_ptr<#time_constraint> */
  static std::shared_ptr<time_constraint> create(
      time_constraint::exec_callback, time_event&, time_event&,
      time_value = Infinite, time_value = Zero, time_value = Infinite);

  time_constraint(
      time_constraint::exec_callback, time_event&, time_event&,
      time_value = Infinite, time_value = Zero, time_value = Infinite);

  /*! desctructor */
  ~time_constraint();

  /*! start #time_constraint's #Clock */
  void start();
  void start(ossia::state&);

  /*! stop #time_constraint's #Clock */
  void stop();

  /*! start #time_constraint's #Clock */
  void pause();

  /*! stop #time_constraint's #Clock */
  void resume();

  /*! set #time_constraint's #Clock offset and process a state at offset date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #time_constraint manages
   \details don't call offset when the #time_constraint is running
   \param const #TimeValue offset date
   \return std::shared_ptr<#State> */
  ossia::state_element offset(ossia::time_value);

  /*! get a #State from the constraint depending on its #Clock date
   \details the returned #State is made of as many as sub States for each
   TimeProcess the #time_constraint manages
   \details don't call state when the #time_constraint is not running
   \return std::shared_ptr<#State> */
  ossia::state_element state();

  /*! sets a new callback for the constraint
    \param #time_constraint::ExecutionCallback to use to be notified at each
    step
    */
  void set_callback(exec_callback);

  //! This callback won't compute the state.
  void set_stateless_callback(exec_callback);

  /*! get the #time_constraint nominal duration
   \return const #TimeValue& nominal duration */
  const time_value& get_nominal_duration() const;

  /*! set the #time_constraint duration
   \param const #TimeValue& duration
   \return #time_constraint the constraint */
  time_constraint& set_nominal_duration(ossia::time_value);

  /*! get the #time_constraint minimal duration
   \return const #TimeValue& minimal duration */
  const time_value& get_min_duration() const;

  /*! set the #time_constraint minimal duration
   \param const #TimeValue& minimal duration
   \return #time_constraint the constraint */
  time_constraint& set_min_duration(ossia::time_value);

  /*! get the #time_constraint maximal duration
   \return const #TimeValue& maximal duration */
  const time_value& get_max_duration() const;

  /*! set the #time_constraint maximal duration
   \param const #TimeValue& maximal duration
   \return #time_constraint the constraint */
  time_constraint& set_max_duration(ossia::time_value);

  /*! get the event from where the #time_constraint starts
   \return std::shared_ptr<#time_event> start event */
  time_event& get_start_event() const;

  /*! get the event from where the #time_constraint starts
   \return std::shared_ptr<#time_event> start event */
  time_event& get_end_event() const;

  /*! add a #TimeProcess
   \details it also stores the #TimeProcess's start and end #States into the
   #time_constraint's start and end #time_events
   \param std::shared_ptr<#TimeProcess> to insert */
  void add_time_process(std::shared_ptr<time_process>);

  /*! remove a #TimeProcess
   \details it also removes the #TimeProcess's start and end #States from the
   #time_constraint's start and end #time_events
   \param std::shared_ptr<#TimeProcess> to insert */
  void remove_time_process(time_process*);

  /*! get time processes attached to the #time_constraint
   \return #Container<#TimeProcess> */
  const std::vector<std::shared_ptr<time_process>>& get_time_processes() const
  {
    return m_processes;
  }

private:
  ossia::state make_state();

  std::vector<std::shared_ptr<time_process>> m_processes;
  time_constraint::exec_callback m_callback;

  time_event& m_start;
  time_event& m_end;

  time_value m_nominal{};
  time_value m_min{};
  time_value m_max{};

  /// the progression of the clock between the beginning
  /// and the end [0. :: 1.]
  double m_position{};

  /// how many time the clock is running (without no speed
  /// factor consideration)
  time_value m_date{};

  time_value m_offset{}; /// the date (in ms) the clock will run from
  double m_speed{1.};    /// the speed factor of the clock
  bool m_running{};
};
}
