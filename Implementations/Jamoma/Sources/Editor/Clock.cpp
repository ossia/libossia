#include "Editor/JamomaClock.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life cycle

std::shared_ptr<Clock> Clock::create(const TimeValue duration,
                                     const TimeValue offset,
                                     const float speed)
{
  return nullptr;
}

JamomaClock::JamomaClock(const TimeValue duration,
                         const TimeValue offset,
                         const float speed)
{
  
}

JamomaClock::~JamomaClock()
{
  
}

# pragma mark -
# pragma mark Execution

void JamomaClock::go()
{
  
}

void JamomaClock::stop()
{
  
}

void JamomaClock::pause()
{
  
}

void JamomaClock::resume()
{
  
}

void JamomaClock::tick()
{
  
}

# pragma mark -
# pragma mark Accessors

const TimeValue JamomaClock::getDuration() const
{
  
}

Clock & JamomaClock::setDuration(const TimeValue)
{
  return *this;
}

const TimeValue JamomaClock::getOffset() const
{
  
}

Clock & JamomaClock::setOffset(const TimeValue)
{
  return *this;
}

const float JamomaClock::getSpeed() const
{
  
}

Clock & JamomaClock::setSpeed(const float)
{
  return *this;
}

# pragma mark -
# pragma mark Callback

JamomaClock::PositionCallback JamomaClock::getPositionCallback() const
{
  
}

void JamomaClock::setPositionCallback(PositionCallback)
{
  
}