/*!
 * \file Clock.h
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

namespace OSSIA
{

class Clock
{

public:
  
# pragma mark -
# pragma mark Life cycle
  
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
  
  /*! called every time a new step should be processed */
  virtual void tick() = 0;
  
# pragma mark -
# pragma mark Accessors
  
  /*! set the duration of the clock execution
   \param const #TimeValue duration
   \return #Clock the clock */
  virtual Clock & setDuration(const TimeValue);
  
  /** set the offset of the clock
   \param const #TimeValue offset
   \return #Clock the clock */
  virtual Clock & setOffset(const TimeValue);
  
  /** set the speed factor attribute
   \param const float speed factor
   \return #Clock the clock */
  virtual Clock & setSpeed(const float);
};
}
