#include "Editor/JamomaAutomation.h"

#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
template<> shared_ptr<Automation<double>> Automation<double>::create(TimeProcess::ExecutionCallback callback,
                                                                     shared_ptr<State> startState,
                                                                     shared_ptr<State> endState,
                                                                     shared_ptr<Clock> clock)
{
  return make_shared<JamomaAutomation>(callback, startState, endState, clock);
}
}

JamomaAutomation::JamomaAutomation(TimeProcess::ExecutionCallback callback,
                                   shared_ptr<State> startState,
                                   shared_ptr<State> endState,
                                   shared_ptr<Clock> clock) :
mCallback(callback),
mStartState(startState),
mEndState(endState),
mClock(clock)
{
  // pass callback to the Clock
  Clock::ExecutionCallback clockCallback = std::bind(&JamomaAutomation::ClockCallback, this, _1, _2);
  mClock->setExecutionCallback(clockCallback);

  // build an internal State to update at each tick of the clock
  mCurrentState = State::create();
}

JamomaAutomation::JamomaAutomation(const JamomaAutomation * other)
{}

JamomaAutomation::~JamomaAutomation()
{}

shared_ptr<Automation<double>> JamomaAutomation::clone() const
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

shared_ptr<State> JamomaAutomation::state() const
{
  return mCurrentState;
}

# pragma mark -
# pragma mark Accessors

const Value * JamomaAutomation::getStartValue() const
{
  return value;
}

void JamomaAutomation::setStartValue(const Value*)
{}

const Value * JamomaAutomation::getEndValue() const
{
  return value;
}

void JamomaAutomation::setEndValue(const Value*)
{}

const shared_ptr<Curve<double>> & JamomaAutomation::getCurve() const
{
  return curve;
}

void JamomaAutomation::setCurve(shared_ptr<Curve<double>>)
{}

const shared_ptr<Address> & JamomaAutomation::getInputAddress() const
{
  return addr;
}

void JamomaAutomation::setInputAddress(shared_ptr<Address>)
{}

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
