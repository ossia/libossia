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
class OSSIA_EXPORT clock
{
public:
  using clock_type = std::chrono::steady_clock;
  /*! to get the clock execution back
   \param clock position
   \param clock date
   \param dropped ticks */
  using ExecutionCallback
      = std::function<void(time_value, time_value, unsigned char)>;

  enum ClockExecutionStatus
  {
    RUNNING,
    STOPPED
  };
  using ExecutionStatusCallback = std::function<void(ClockExecutionStatus)>;

  /*! how the time flows for the clock */
  enum class DriveMode
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
   \param drive mode
   \return std::shared_ptr<#Clock> */
  clock(
      clock::ExecutionCallback, time_value = Infinite, time_value = 10.,
      time_value = 0., float = 1.,
      clock::DriveMode = clock::DriveMode::INTERNAL);

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
  bool tick(time_value usec);

  /*! get the duration of the clock
   \return const #TimeValue duration */
  time_value getDuration() const;

  /*! set the duration of the clock execution
   \param const #TimeValue duration
   \return #Clock the clock */
  clock& setDuration(time_value);

  /*! get the offset of the clock
   \return const #TimeValue offset */
  time_value getOffset() const;

  /** set the offset of the clock
   \param const #TimeValue offset
   \return #Clock the clock */
  clock& setOffset(time_value);

  /*! get the granularity of the clock
   \return const #TimeValue granularity */
  time_value getGranularity() const;

  /*! set the granularity of the clock execution
   \param const #TimeValue granularity
   \return #Clock the clock */
  clock& setGranularity(time_value);

  /*! get the speed of the clock
   \return const #TimeValue speed */
  double getSpeed() const;

  /** set the speed factor attribute
   \param double speed factor
   \return #Clock the clock */
  clock& setSpeed(double);

  /*! get the clock drive mode
   \return #DriveMode */
  DriveMode getDriveMode() const;

  /** set is the clock drive mode
   \param #DriveMode
   \return #Clock the clock */
  clock& setDriveMode(DriveMode);

  /*! get the running status of the clock
   \return bool true if is running */
  bool getRunning() const;

  /*! get the position of the clock
   \return const #TimeValue position */
  time_value getPosition() const;

  /*! get the date of the clock
   \return const #TimeValue date */
  time_value getDate() const;

  // Execution status will be called when the clock starts and stops.
  void setExecutionStatusCallback(ExecutionStatusCallback);
  ExecutionStatusCallback getExecutionStatusCallback() const;


  /*! to allow TimeConstraint to override start method */
  void do_start();

  /*! to allow TimeConstraint to override stop method */
  void do_stop();

  /*! to allow TimeConstraint to override setDuration accessor */
  void do_setDuration(time_value);

  /*! to allow TimeConstraint to override setOffset accessor */
  void do_setOffset(time_value);

  /*! to avoid dead lock in EXTERNAL drive mode if a TimeProcess wants to end
   * its ParentTimeConstraint's clock */
  void request_stop();

  protected:
    time_value mDuration{};    /// the time (in ms) the clock will run at normal
                               /// speed factor
    time_value mGranularity{}; /// the minimum time between each tick (in ms)
    time_value mOffset{};      /// the date (in ms) the clock will run from
    double mSpeed{};           /// the speed factor of the clock

    clock::DriveMode
        mDriveMode{}; /// in EXTERNAL drive mode the tick() method is
                      /// called from outside

    std::atomic_bool mRunning{}; /// is the clock running right now ?
    std::atomic_bool mPaused{};  /// is the clock paused right now ?
    time_value
        mPosition{};    /// the progression of the clock between the beginning
                        /// and the end [0. :: 1.]
    time_value mDate{}; /// how many time the clock is running (without no speed
                        /// factor consideration)

    std::thread mThread; /// a thread to launch the clock execution

    clock_type::time_point
        mLastTime{};          /// a time reference used to compute time tick
    long long mElapsedTime{}; /// a time reference used to know how many time are
                              /// elapsed in microsecond

  private:
    /*! called back by the internal thread */
    void threadCallback();

    ExecutionCallback mCallback; /// the callback to use for each step
    ExecutionStatusCallback mStatusCallback;
};
}
