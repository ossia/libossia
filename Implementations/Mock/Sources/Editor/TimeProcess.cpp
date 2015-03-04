#include "Editor/TimeProcess.h"

using namespace OSSIA;
using namespace std;

class Scheduler;

class MockTimeProcess : public TimeProcess {

public:

  // Dummy members
  shared_ptr<TimeEvent> evt;
  shared_ptr<State> st;
  shared_ptr<Scheduler> sched;

  // Navigation
  virtual const shared_ptr<TimeEvent> & getStartEvent() const override {
    return evt;
  }
  virtual const shared_ptr<TimeEvent> & getEndEvent() const override {
    return evt;
  }

  // Accessors
  virtual TimeValue getLength() const override {return TimeValue();}
  virtual void setLength(TimeValue) override {};
  virtual const shared_ptr<State> & getStartState() const override {return st;}
  virtual void setStartState(shared_ptr<State>) override {}
  virtual const shared_ptr<State> & getEndState() const override {return st;}
  virtual void setEndState(shared_ptr<State>) override {}
  virtual const shared_ptr<Scheduler> & getScheduler() const override {
    return sched;
  }
  virtual void setScheduler(shared_ptr<Scheduler>) override {}

};
