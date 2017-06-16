#pragma once

#include <ossia/editor/scenario/time_value.hpp>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>
#include <ossia_export.h>

namespace ossia
{
class time_constraint;
using clock_type = std::chrono::steady_clock;
class OSSIA_EXPORT clock
{
  friend class time_constraint;
public:
  enum exec_status
  {
    RUNNING,
    STOPPED
  };
  using exec_status_callback = std::function<void(exec_status)>;

  //! Ratio : the number of time units in one millisecond
  clock(
      ossia::time_constraint& cst,
      double time_ratio = 1000.);

  /*! destructor */
  ~clock();

  /*! start the clock */
  void start();

  /*! halt the clock */
  void stop();

  /*! pause the clock progression */
  void pause();

  /*! true if the constraint is running and paused, else false */
  bool paused() const;

  /*! resume the clock progression */
  void resume();

  /*! called every time a new step should be processed.
   \details can be use to force step processing in case of external drive but
   the callback will not be called
   \return bool true if the clock ticks */
  bool tick();

  /*! get the duration of the clock
   \return const #TimeValue duration */
  time_value get_duration() const;

  /*! set the duration of the clock execution
   \param const #TimeValue duration
   \return #Clock the clock */
  clock& set_duration(ossia::time_value);

  /*! get the granularity of the clock
   \return const #TimeValue granularity */
  time_value get_granularity() const;

  /*! set the granularity of the clock execution
   \param const #TimeValue granularity
   \return #Clock the clock */
  clock& set_granularity(ossia::time_value);

  /*! get the running status of the clock
   \return bool true if is running */
  bool running() const;

  /*! get the position of the clock
   \return const #TimeValue position */
  time_value get_position() const;

  /*! get the date of the clock
   \return const #TimeValue date */
  time_value get_date() const;

  // Execution status will be called when the clock starts and stops.
  void set_exec_status_callback(exec_status_callback);
  exec_status_callback get_exec_status_callback() const;

  private:
  /*! to allow TimeConstraint to override setDuration accessor */
  void do_set_duration(ossia::time_value);

  /*! to avoid dead lock in EXTERNAL drive mode if a TimeProcess wants to end
   * its ParentTimeConstraint's clock */
  void request_stop();

  time_constraint& m_constraint;

  double m_ratio{};

  time_value m_duration{};    /// the time (in ms) the clock will run at normal

  /// speed factor
  time_value m_granularity{}; /// the minimum time between each tick (in ms)

  /// the progression of the clock between the beginning
  /// and the end [0. :: 1.]
  time_value m_position{};

  /// how many time the clock is running (without no speed
  /// factor consideration)
  time_value m_date{};

  std::thread m_thread; /// a thread to launch the clock execution

  /// a time reference used to compute time tick
  clock_type::time_point m_lastTime{};

  exec_status_callback m_statusCallback;

  /// a time reference used to know elapsed time in a microsecond
  int64_t m_elapsedTime{};

  std::atomic_bool m_running{}; /// is the clock running right now ?
  std::atomic_bool m_paused{}; /// is the clock paused right now ?
  std::atomic_bool m_shouldStop{};

  /*! called back by the internal thread */
  void threadCallback();

};
}
