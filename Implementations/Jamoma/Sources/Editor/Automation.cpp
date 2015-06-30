#include "Editor/JamomaAutomation.h"

#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  // explicit instantiation for double
  template class Automation<double>;
  
  template<>
  shared_ptr<Automation<double>> Automation<double>::create(TimeProcess::ExecutionCallback callback,
                                                           shared_ptr<State> startState,
                                                           shared_ptr<State> endState,
                                                           shared_ptr<Clock> clock)
  {
    return make_shared<JamomaAutomation<double>>(callback, startState, endState, clock);
  }
}

template <typename T>
JamomaAutomation<T>::JamomaAutomation(TimeProcess::ExecutionCallback callback,
                                      shared_ptr<State> startState,
                                      shared_ptr<State> endState,
                                      shared_ptr<Clock> clock) :
mCallback(callback),
mStartState(startState),
mEndState(endState),
mClock(clock)
{
  // pass callback to the Clock
  Clock::ExecutionCallback clockCallback = std::bind(&JamomaAutomation<T>::ClockCallback, this, _1, _2);
  mClock->setExecutionCallback(clockCallback);

  // build an internal State to update at each tick of the clock
  mCurrentState = State::create();
}

template <typename T>
JamomaAutomation<T>::JamomaAutomation(const JamomaAutomation * other)
{}

template <typename T>
JamomaAutomation<T>::~JamomaAutomation()
{}

template <typename T>
shared_ptr<Automation<T>> JamomaAutomation<T>::clone() const
{
  return make_shared<JamomaAutomation>(this);
}

# pragma mark -
# pragma mark Execution

template <typename T>
void JamomaAutomation<T>::play(bool log, string name) const
{
  // setup clock duration with parent constraint duration
  mClock->setDuration(mParent->getDuration());
  
  mClock->go();
}

template <typename T>
shared_ptr<State> JamomaAutomation<T>::state() const
{
  return mCurrentState;
}

# pragma mark -
# pragma mark Accessors

template <typename T>
const Value * JamomaAutomation<T>::getStartValue() const
{
  return value;
}

template <typename T>
void JamomaAutomation<T>::setStartValue(const Value*)
{}

template <typename T>
const Value * JamomaAutomation<T>::getEndValue() const
{
  return value;
}

template <typename T>
void JamomaAutomation<T>::setEndValue(const Value*)
{}

template <typename T>
const shared_ptr<Curve<T>> & JamomaAutomation<T>::getCurve() const
{
  return curve;
}

template <typename T>
void JamomaAutomation<T>::setCurve(shared_ptr<Curve<T>>)
{}

template <typename T>
const shared_ptr<Address> & JamomaAutomation<T>::getInputAddress() const
{
  return addr;
}

template <typename T>
void JamomaAutomation<T>::setInputAddress(shared_ptr<Address>)
{}

template <typename T>
const shared_ptr<State> & JamomaAutomation<T>::getStartState() const
{
  return mStartState;
}

template <typename T>
const shared_ptr<State> & JamomaAutomation<T>::getEndState() const
{
  return mEndState;
}

template <typename T>
const shared_ptr<Clock> & JamomaAutomation<T>::getClock() const
{
  return mClock;
}

template <typename T>
const shared_ptr<TimeConstraint> & JamomaAutomation<T>::getParentTimeConstraint() const
{
  return mParent;
}

template <typename T>
void JamomaAutomation<T>::ClockCallback(const TimeValue& position, const TimeValue& date)
{
  (mCallback)(position, date, mCurrentState);
}
