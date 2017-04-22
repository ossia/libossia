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
using clock_type = std::chrono::steady_clock;

//! This class allows to provide a custom clock source to ossia::clock.
struct OSSIA_EXPORT clock_source
{
  clock_type::time_point (*now)() = [] { return clock_type::now(); };
};

class OSSIA_EXPORT clock
{
  friend class time_constraint;
public:
  /*! to get the clock execution back
   \param clock position
   \param clock date
   \param dropped ticks */
  using exec_callback
  = std::function<void(ossia::time_value, time_value, unsigned char)>;

  enum exec_status
  {
    RUNNING,
    STOPPED
  };
  using exec_status_callback = std::function<void(exec_status)>;

  /*! how the time flows for the clock */
  enum class drive_mode
  {
    INTERNAL, // the tick method is called by the clock itself
    EXTERNAL  // the tick method is called from outside the clock
  };

  /*! ctor
   \param #ExecutionCallback
   \param duration
   \param granularity
   \param offset
   \param speed
   \param drive mode*/
  clock(
      clock::exec_callback, time_value = Infinite, time_value = time_value{10.},
      time_value = Zero, float = 1.,
      clock::drive_mode = clock::drive_mode::INTERNAL, clock_source = clock_source{});

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

  /*! can be called by hand to advance the clock from a certain number of
   * ticks.
   * \param usec Number of microseconds to move the clock
   * \return bool true if the clock ticks
   */
  bool tick(ossia::time_value usec);

  /*! get the duration of the clock
   \return const #TimeValue duration */
  time_value get_duration() const;

  /*! set the duration of the clock execution
   \param const #TimeValue duration
   \return #Clock the clock */
  clock& set_duration(ossia::time_value);

  /*! get the offset of the clock
   \return const #TimeValue offset */
  time_value get_offset() const;

  /** set the offset of the clock
   \param const #TimeValue offset
   \return #Clock the clock */
  clock& set_offset(ossia::time_value);

  /*! get the granularity of the clock
   \return const #TimeValue granularity */
  time_value get_granularity() const;

  /*! set the granularity of the clock execution
   \param const #TimeValue granularity
   \return #Clock the clock */
  clock& set_granularity(ossia::time_value);

  /*! get the speed of the clock
   \return const #TimeValue speed */
  double get_speed() const;

  /** set the speed factor attribute
   \param double speed factor
   \return #Clock the clock */
  clock& set_speed(double);

  /*! get the clock drive mode
   \return #DriveMode */
  drive_mode get_drive_mode() const;

  /** set is the clock drive mode
   \param #DriveMode
   \return #Clock the clock */
  clock& set_drive_mode(drive_mode);

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


  /**
   * @brief setCallback Replace the execution callback.
   *
   * The callback **shall** be a valid function, e.g. bool(callback) == true.
   */
  void set_callback(exec_callback);

protected:
  /*! to allow TimeConstraint to override start method */
  void do_start();

  /*! to allow TimeConstraint to override stop method */
  void do_stop();

  /*! to allow TimeConstraint to override setDuration accessor */
  void do_set_duration(ossia::time_value);

  /*! to allow TimeConstraint to override setOffset accessor */
  void do_set_offset(ossia::time_value);

  /*! to avoid dead lock in EXTERNAL drive mode if a TimeProcess wants to end
   * its ParentTimeConstraint's clock */
  void request_stop();

  time_value m_duration{};    /// the time (in ms) the clock will run at normal
  /// speed factor
  time_value m_granularity{}; /// the minimum time between each tick (in ms)
  time_value m_offset{};      /// the date (in ms) the clock will run from
  double m_speed{1.};           /// the speed factor of the clock

  /// the progression of the clock between the beginning
  /// and the end [0. :: 1.]
  time_value m_position{};

  /// how many time the clock is running (without no speed
  /// factor consideration)
  time_value m_date{};

  clock_source m_source; //! \todo use me
  std::thread m_thread; /// a thread to launch the clock execution

  /// a time reference used to compute time tick
  clock_type::time_point m_lastTime{};

  /// a time reference used to know elapsed time in a microsecond
  int64_t m_elapsedTime{};

  /// in EXTERNAL drive mode the tick() method is called from outside
  clock::drive_mode m_drive_mode{};

  std::atomic_bool m_running{}; /// is the clock running right now ?
  std::atomic_bool m_paused{}; /// is the clock paused right now ?
  std::atomic_bool m_shouldStop{};

private:
  /*! called back by the internal thread */
  void threadCallback();

  exec_callback m_callback; /// the callback to use for each step
  exec_status_callback m_statusCallback;
};
}
