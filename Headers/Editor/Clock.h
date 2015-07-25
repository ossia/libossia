/*!
 * \file Clock.h
 *
 * \defgroup Editor
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <memory>
#include <functional>

#include "Editor/TimeValue.h"

namespace OSSIA
{

class Clock
{

public:
  
# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param duration
   \param granularity
   \param offset
   \param speed
   \param external drive
   \return std::shared_ptr<#Clock> */
  static std::shared_ptr<Clock> create(const TimeValue& = Infinite,
                                       const TimeValue& = 1.,
                                       const TimeValue& = 0.,
                                       float = 1.,
                                       bool = false);
  
  /*! clone */
  virtual std::shared_ptr<Clock> clone() const = 0;
  
  /*! destructor */
  virtual ~Clock() = default;

# pragma mark -
# pragma mark Execution
  
  /*! start the clock */
  virtual void go() = 0;
  
  /*! halt the clock */
  virtual void stop() = 0;
  
  /*! pause the clock progression */
  virtual void pause() = 0;
  
  /*! resume the clock progression */
  virtual void resume() = 0;
  
  /*! called every time a new step should be processed.
   \details can be use to force step processing in case of external drive but the callback will not be called
   \return bool true if the clock ticks */
  virtual bool tick() = 0;
  
  /*! to get the clock execution back
   \param clock position
   \param clock date 
   \param dropped ticks */
  using ExecutionCallback = std::function<void(const TimeValue&, const TimeValue&, unsigned char)>;
  
  /*! get the clock execution callback function
   \return #ValueCallback function */
  virtual ExecutionCallback getExecutionCallback() const = 0;
  
  /*! set the clock execution callback function
   \param #ValueCallback function */
  virtual void setExecutionCallback(ExecutionCallback) = 0;
  
# pragma mark -
# pragma mark Accessors
  
  /*! get the duration of the clock
   \return const #TimeValue duration */
  virtual const TimeValue & getDuration() const = 0;
  
  /*! set the duration of the clock execution
   \param const #TimeValue duration
   \return #Clock the clock */
  virtual Clock & setDuration(const TimeValue&) = 0;
  
  /*! get the granularity of the clock
   \return const #TimeValue granularity */
  virtual const TimeValue & getGranularity() const = 0;
  
  /*! set the granularity of the clock execution
   \param const #TimeValue granularity
   \return #Clock the clock */
  virtual Clock & setGranularity(const TimeValue&) = 0;
  
  /*! get the offset of the clock
   \return const #TimeValue offset */
  virtual const TimeValue & getOffset() const = 0;
  
  /** set the offset of the clock
   \param const #TimeValue offset
   \return #Clock the clock */
  virtual Clock & setOffset(const TimeValue&) = 0;
  
  /*! get the speed of the clock
   \return const #TimeValue speed */
  virtual float getSpeed() const = 0;
  
  /** set the speed factor attribute
   \param float speed factor
   \return #Clock the clock */
  virtual Clock & setSpeed(float) = 0;
  
  /*! is the clock in external drive mode
   \return bool true if the clock is in external drive mode */
  virtual bool getExternal() const = 0;
  
  /** set is the clock in external drive mode
   \param bool
   \return #Clock the clock */
  virtual Clock & setExternal(bool) = 0;
  
  /*! get the running status of the clock
   \return bool true if is running */
  virtual bool getRunning() const = 0;
  
  /*! get the position of the clock
   \return const #TimeValue position */
  virtual const TimeValue & getPosition() const = 0;
  
  /*! get the date of the clock
   \return const #TimeValue date */
  virtual const TimeValue & getDate() const = 0;
};
}
