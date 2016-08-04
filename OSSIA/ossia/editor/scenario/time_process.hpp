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

public:
  /*! destructor */
  virtual ~time_process();

  /*! get a #StateElement from the process on its parent #TimeConstraint offset
   \details don't call offset when the parent #TimeConstraint is running
   \param const #TimeValue offset date
   \return std::shared_ptr<#StateElement> */
  virtual state_element offset(time_value) = 0;

  /*! get a #StateElement from the process depending on its parent
   #TimeConstraint date
   \details don't call state when the parent #TimeConstraint is not running
   \return std::shared_ptr<#StateElement> */
  virtual state_element state() = 0;

  virtual void start()
  {
  }
  virtual void stop()
  {
  }
  virtual void pause()
  {
  }
  virtual void resume()
  {
  }

  time_constraint* parent{};

  protected:
    ossia::time_value mLastDate{ossia::Infinite};
    // used to filter multiple state calls at the
    // same time (use date as position can be always
    // 0 in infinite duration case)
};
}
