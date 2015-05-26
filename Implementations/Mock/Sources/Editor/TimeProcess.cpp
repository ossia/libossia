#pragma once
#include "Editor/TimeProcess.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class MockTimeProcess : public virtual TimeProcess {

public:

  // Dummy members
  shared_ptr<TimeEvent> startevt;
  shared_ptr<TimeEvent> endevt;
  shared_ptr<State> startst;
  shared_ptr<State> endst;
  shared_ptr<Clock> sched;

  // Navigation
  virtual const shared_ptr<TimeEvent> & getStartEvent() const override {
    return startevt;
  }
  virtual const shared_ptr<TimeEvent> & getEndEvent() const override {
    return endevt;
  }

  // Accessors
  virtual TimeValue getLength() const override {return TimeValue();}
  virtual void setLength(TimeValue) override {}

  virtual const shared_ptr<State> & getStartState() const override {return startst;}
  virtual void setStartState(shared_ptr<State> state) override {startst = state;}
  virtual const shared_ptr<State> & getEndState() const override {return endst;}
  virtual void setEndState(shared_ptr<State> state) override {endst = state;}

  virtual const shared_ptr<Clock> & getClock() const override {
    return sched;
  }
  virtual void setClock(shared_ptr<Clock>) override {}

};
