#include "Editor/JamomaTimeProcess.h"

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

JamomaTimeProcess::JamomaTimeProcess(TimeProcess::ExecutionCallback callback,
                                     shared_ptr<State> startState,
                                     shared_ptr<State> endState,
                                     shared_ptr<Clock> clock) :
mCallback(callback),
mStartState(startState),
mEndState(endState),
mClock(clock)
{
  mCurrentState = State::create();
}
