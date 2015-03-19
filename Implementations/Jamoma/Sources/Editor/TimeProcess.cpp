#include "Editor/TimeProcess.h"
#include "Editor/TimeValue.h"
#include "Editor/Clock.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeProcess : public TimeProcess
{
  
private:
  // Implementation Specific
  shared_ptr<TimeEvent> event;
  shared_ptr<State> state;
  shared_ptr<Clock> clock;
  
public:
  // Navigation
  virtual const shared_ptr<TimeEvent> & getStartEvent() const override
  {
    return event;
  }
  
  virtual const shared_ptr<TimeEvent> & getEndEvent() const override
  {
    return event;
  }

  // Accessors
  virtual TimeValue getLength() const override
  {
    return TimeValue();
  }
  
  virtual void setLength(TimeValue) override
  {}
  
  virtual const shared_ptr<State> & getStartState() const override
  {
    return state;
  }
  
  virtual void setStartState(shared_ptr<State>) override
  {}
  
  virtual const shared_ptr<State> & getEndState() const override
  {
    return state;
  }
  
  virtual void setEndState(shared_ptr<State>) override
  {}
  
  virtual const shared_ptr<Clock> & getClock() const override
  {
    return clock;
  }
  
  virtual void setClock(shared_ptr<Clock>) override
  {}

};
