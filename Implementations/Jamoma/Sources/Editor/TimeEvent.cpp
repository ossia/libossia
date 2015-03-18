#include "TimeProcessList.cpp"
#include "Editor/TimeEvent.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeEvent : public TimeEvent
{
  
private:
  // Implementation Specific
  shared_ptr<State> st;
  shared_ptr<Expression> exp;
  JamomaTimeProcessList proclist;
  
public:
  // Constructor, destructor
  JamomaTimeEvent(shared_ptr<State> st, shared_ptr<Expression> exp)
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
  {}

  // Accessors
  virtual const shared_ptr<State> & getState() const override
  {
    return st;
  }
  
  virtual const shared_ptr<Expression> & getExpression() const override
  {
    return exp;
  }
  
  virtual TimeProcessList & getPreviousProcesses() override
  {
    return proclist;
  }
  
  virtual TimeProcessList & getNextProcesses() override
  {
    return proclist;
  }

};
