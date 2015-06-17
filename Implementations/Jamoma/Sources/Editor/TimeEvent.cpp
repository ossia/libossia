#include "Editor/JamomaTimeEvent.h"
#include "Editor/State.h"

# pragma mark -
# pragma mark Life cycle

JamomaTimeEvent::JamomaTimeEvent(shared_ptr<TimeNode> aTimeNode, shared_ptr<State> aState, shared_ptr<Expression> anExpression) :
mTimeNode(aTimeNode),
mState(aState),
mExpression(anExpression)
{}

JamomaTimeEvent::~JamomaTimeEvent()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeEvent::play(bool log, string name) const
{
  mState->launch();
  // note : what else ?
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeNode> & JamomaTimeEvent::getTimeNode() const
{
  return mTimeNode;
}
  
const shared_ptr<State> & JamomaTimeEvent::getState() const
{
  return mState;
}

const shared_ptr<Expression> & JamomaTimeEvent::getExpression() const
{
  return mExpression;
}
