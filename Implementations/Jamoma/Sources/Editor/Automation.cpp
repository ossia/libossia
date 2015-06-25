#include "Editor/JamomaAutomation.h"

#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

template<> shared_ptr<Automation<double>> Automation<double>::create(shared_ptr<State> startState,
                                                                     shared_ptr<State> endState,
                                                                     shared_ptr<Clock> clock)
{
  return make_shared<JamomaAutomation>(startState, endState, clock);
}

JamomaAutomation::JamomaAutomation(shared_ptr<State> startState,
                                   shared_ptr<State> endState,
                                   shared_ptr<Clock> clock) :
mStartState(startState),
mEndState(endState),
mClock(clock)
{
  // pass callback to the Clock
  Clock::ExecutionCallback callback = std::bind(&JamomaAutomation::ClockCallback, this, _1, _2);
  mClock->setExecutionCallback(callback);
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
  mClock->go();
}

shared_ptr<State> JamomaAutomation::state(const TimeValue& position, const TimeValue& date) const
{
  // on start
  if (position == Zero)
  {
    ;
  }
  // on end
  else if (position == One)
  {
    ;
  }
  
  //! \todo the algorithme !
  return State::create();
}

# pragma mark -
# pragma mark Accessors

Value JamomaAutomation::getStartValue() const
{
  return Value();
}

void JamomaAutomation::setStartValue(Value)
{}

Value JamomaAutomation::getEndValue() const
{
  return Value();
}

void JamomaAutomation::setEndValue(Value)
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

void JamomaAutomation::ClockCallback(const TimeValue& position, const TimeValue& date)
{
  // cout << "JamomaAutomation::ClockCallback : " << double(position) << ", " << double(date) << "\n";
  
  // on start
  if (position == Zero)
  {
    cout << "JamomaAutomation starts\n";
  }
  // on end
  else if (position == One)
  {
    cout << "JamomaAutomation ends\n";
  }
  
  //state(position, date)->launch();
}