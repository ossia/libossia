#include "Editor/JamomaTimeProcess.h"

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

JamomaTimeProcess::JamomaTimeProcess(shared_ptr<State> startState,
                                     shared_ptr<State> endState) :
mStartState(startState),
mEndState(endState)
{
  mCurrentState = State::create();
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<State> & JamomaTimeProcess::getStartState() const
{
  return mStartState;
}

const shared_ptr<State> & JamomaTimeProcess::getEndState() const
{
  return mEndState;
}

const shared_ptr<TimeConstraint> & JamomaTimeProcess::getParentTimeConstraint() const
{
  return mParent;
}