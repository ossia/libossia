#pragma once

#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <memory>
#include <ossia_export.h>
#include <string>

namespace ossia
{
class time_constraint;
class time_value;
/**
 * @brief The time_process class
 *
 * \brief #TimeProcess is the parent class used to describe temporal content.
 */
class OSSIA_EXPORT time_process
{
  friend class time_constraint;
public:
  /*! destructor */
  virtual ~time_process();

  /*! get a #StateElement from the process on its parent #TimeConstraint offset
   \details don't call offset when the parent #TimeConstraint is running
   \param const #time_value offset date
   \return state_element */
  virtual state_element offset(time_value) = 0;

  /*! get a #StateElement from the process depending on its parent
   #TimeConstraint date
   \details don't call state when the parent #TimeConstraint is not running
   \return state_element */
  virtual state_element state() = 0;

  /**
  * @brief start
  *
  * Will be called when
  * the parent time constraint is started.
  */
  virtual void start()
  {
  }

  /**
  * @brief stop
  *
  * Will be called when
  * the parent time constraint is stopped.
  */
  virtual void stop()
  {
  }

  /**
  * @brief stop
  *
  * Will be called when
  * the parent time constraint is paused.
  */
  virtual void pause()
  {
  }

  /**
  * @brief resume
  *
  * Will be called when
  * the parent time constraint is resumed.
  */
  virtual void resume()
  {
  }

  /**
   * @brief parent The parent time_constraint of the process
   *
   * The time_constraint sets this pointer when a process is added to it.
   * This has the obvious consequence that a time_process cannot be part
   * of two different time_constraint.
   *
   * @return A pointer to the parent.
   */
  time_constraint* parent() const
  { return mParent; }

  int32_t getPriority() const { return mPriority; }
  void setPriority(int32_t i) { mPriority = i; }

  bool getPriorityOverride() const { return mPriorityOverride; }
  void setPriorityOverride(bool o) { mPriorityOverride = o;}
  protected:
    ossia::time_value mLastDate{ossia::Infinite};
    // used to filter multiple state calls at the
    // same time (use date as position can be always
    // 0 in infinite duration case)

  private:
    time_constraint* mParent{};
    int32_t mPriority = 0;
    bool mPriorityOverride = false;
};
}
