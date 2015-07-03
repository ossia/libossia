#include "Editor/JamomaAutomation.h"

#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Automation> Automation::create(TimeProcess::ExecutionCallback callback,
                                            shared_ptr<Address> address,
                                            const Value* drive)
  {
    return make_shared<JamomaAutomation>(callback, address, drive);
  }
}

JamomaAutomation::JamomaAutomation(TimeProcess::ExecutionCallback callback,
                                   shared_ptr<Address> address,
                                   const Value* drive) :
JamomaTimeProcess(callback, State::create(), State::create()),
mDrivenAddress(address),
mDrive(drive->clone())
{
  // pass callback to the Clock
  Clock::ExecutionCallback clockCallback = std::bind(&JamomaAutomation::ClockCallback, this, _1, _2);
  mClock->setExecutionCallback(clockCallback);
}

JamomaAutomation::JamomaAutomation(const JamomaAutomation * other) :
JamomaTimeProcess(other->mCallback, other->mStartState, other->mEndState)
{}

JamomaAutomation::~JamomaAutomation()
{}

shared_ptr<Automation> JamomaAutomation::clone() const
{
  return make_shared<JamomaAutomation>(this);
}

# pragma mark -
# pragma mark Execution

void JamomaAutomation::play(bool log, string name) const
{
  // setup clock duration with parent constraint duration
  mClock->setDuration(mParent->getDuration());
  
  mClock->go();
}

void JamomaAutomation::stop() const
{
  mClock->stop();
}

void JamomaAutomation::pause() const
{
  mClock->pause();
}

void JamomaAutomation::resume() const
{
  mClock->resume();
}

shared_ptr<State> JamomaAutomation::state() const
{
  return mCurrentState;
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<Address> JamomaAutomation::getDrivenAddress() const
{
  
}

const Value * JamomaAutomation::getDriving() const
{
  
}

const shared_ptr<State> & JamomaAutomation::getStartState() const
{
  return mStartState;
}

const shared_ptr<State> & JamomaAutomation::getEndState() const
{
  return mEndState;
}

const shared_ptr<Clock> & JamomaAutomation::getClock() const
{
  return mClock;
}

const shared_ptr<TimeConstraint> & JamomaAutomation::getParentTimeConstraint() const
{
  return mParent;
}

void JamomaAutomation::ClockCallback(const TimeValue& position, const TimeValue& date)
{
  (mCallback)(position, date, mCurrentState);
}
