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
# pragma mark Definitions
  
  /*! to get the clock execution back
   \param clock position
   \param clock date
   \param dropped ticks */
  using ExecutionCallback = std::function<void(const TimeValue&, const TimeValue&, unsigned char)>;
  
# pragma mark -
# pragma mark Enumerations
  
  /*! how the time flows for the clock */
  enum class DriveMode
  {
    INTERNAL, // the tick method is called by the clock itself
    EXTERNAL  // the tick method is called from outside the clock
  };

# pragma mark -
# pragma mark Life cycle

  /*! factory
   \param #ExecutionCallback
   \param duration
   \param granularity
   \param offset
   \param speed
   \param drive mode
   \return std::shared_ptr<#Clock> */
  static std::shared_ptr<Clock> create(Clock::ExecutionCallback,
                                       const TimeValue& = Infinite,
                                       const TimeValue& = 1.,
                                       const TimeValue& = 0.,
                                       float = 1.,
                                       DriveMode = Clock::DriveMode::INTERNAL);

  /*! destructor */
  virtual ~Clock();

# pragma mark -
# pragma mark Execution

  /*! start the clock */
  virtual void start() = 0;

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
  
  /*! get the clock drive mode
   \return #DriveMode */
  virtual DriveMode getDriveMode() const = 0;
  
  /** set is the clock drive mode
   \param #DriveMode
   \return #Clock the clock */
  virtual Clock & setDriveMode(DriveMode) = 0;

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
