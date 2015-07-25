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
