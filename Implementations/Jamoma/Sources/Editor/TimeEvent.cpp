#include "TimeProcessList.cpp"
#include "Editor/TimeEvent.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeEvent : public TimeEvent
{
  
private:
  
  // Implementation specific
  shared_ptr<State>       mState;
  shared_ptr<Expression>  mExpression;
  JamomaTimeProcessList   mProcessList;
  
public:
  // Constructor, destructor
  JamomaTimeEvent(shared_ptr<State> aState, shared_ptr<Expression> anExpression) :
  mState(aState),
  mExpression(anExpression)
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  virtual ~JamomaTimeEvent()
  {}

  // Lecture
  virtual void play(bool log = false, string name = "") const override
  {
    // todo : launch it state
    //mState->launch();
    
    // note : what else ?
  }

  // Accessors
  virtual const shared_ptr<State> & getState() const override
  {
    return mState;
  }
  
  virtual const shared_ptr<Expression> & getExpression() const override
  {
    return mExpression;
  }
  
  virtual TimeProcessList & getPreviousProcesses() override
  {
    // todo : return effective previous processes
    return mProcessList;
  }
  
  virtual TimeProcessList & getNextProcesses() override
  {
    // todo : return effective next processes
    return mProcessList;
  }

};
