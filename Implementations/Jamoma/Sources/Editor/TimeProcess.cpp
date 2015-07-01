#include "Editor/JamomaTimeProcess.h"

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

JamomaTimeProcess::JamomaTimeProcess(TimeProcess::ExecutionCallback callback,
                                     shared_ptr<State> startState,
                                     shared_ptr<State> endState) :
mCallback(callback),
mStartState(startState),
mEndState(endState)
{
  mCurrentState = State::create();
  mClock = Clock::create();
}
