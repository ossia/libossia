#include "Editor/JamomaTimeEvent.h"
#include "Editor/State.h"

# pragma mark -
# pragma mark Life cycle

JamomaTimeEvent::JamomaTimeEvent(shared_ptr<State> aState, shared_ptr<Expression> anExpression, shared_ptr<JamomaTimeNode> aTimeNode) :
mState(aState),
mExpression(anExpression),
mTimeNode(aTimeNode)
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
  
const shared_ptr<State> & JamomaTimeEvent::getState() const
{
  return mState;
}

const shared_ptr<Expression> & JamomaTimeEvent::getExpression() const
{
  return mExpression;
}

void JamomaTimeEvent::setExpression(shared_ptr<Expression> expression)
{
  mExpression = expression;
}
