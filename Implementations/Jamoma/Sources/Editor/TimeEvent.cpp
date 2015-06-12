#include "Editor/TimeEvent.h"
#include "Editor/State.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeEvent : public TimeEvent
{
  
private:
  
  // Implementation specific
  shared_ptr<State>       mState;
  shared_ptr<Expression>  mExpression;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeEvent(shared_ptr<State> aState, shared_ptr<Expression> anExpression) :
  mState(aState),
  mExpression(anExpression)
  {}
  
  virtual ~JamomaTimeEvent()
  {}

# pragma mark -
# pragma mark Execution
  
  virtual void play(bool log = false, string name = "") const override
  {
    mState->launch();
    // note : what else ?
  }

# pragma mark -
# pragma mark Accessors
  
  virtual const shared_ptr<State> & getState() const override
  {
    return mState;
  }
  
  virtual const shared_ptr<Expression> & getExpression() const override
  {
    return mExpression;
  }
};
