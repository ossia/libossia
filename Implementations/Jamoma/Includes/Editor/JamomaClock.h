/*!
 * \file JamomaClock.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Editor/Clock.h"

using namespace OSSIA;
using namespace std;

class JamomaClock : public Clock
{
  
private:
  
  // Implementation specific
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaClock(const TimeValue,
              const TimeValue = 0.,
              const float = 0.);
  
  ~JamomaClock();
  
# pragma mark -
# pragma mark Execution
  
  void go() override;
  
  void stop() override;
  
  void pause() override;
  
  void resume() override;
  
  void tick() override;
  
# pragma mark -
# pragma mark Accessors
  
  const TimeValue getDuration() const override;
  
  Clock & setDuration(const TimeValue) override;
  
  const TimeValue getOffset() const override;
  
  Clock & setOffset(const TimeValue) override;
  
  const float getSpeed() const override;
  
  Clock & setSpeed(const float) override;
  
# pragma mark -
# pragma mark Callback
  
  PositionCallback getPositionCallback() const override;
  
  void setPositionCallback(PositionCallback) override;
  
};
