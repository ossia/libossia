#include "TimeProcessList.cpp"

#include "Editor/TimeEvent.h"

using namespace OSSIA;
using namespace std;

class MockTimeEvent : public TimeEvent {

public:

  // Dummy members
  shared_ptr<State> st;
  shared_ptr<Expression> exp;

  // Constructor, destructor
  MockTimeEvent(shared_ptr<State> st, shared_ptr<Expression> exp) {}
  virtual ~MockTimeEvent() {}

  // Lecture
  virtual void play(bool log = false, string name = "") const override {}

  // Accessors
  virtual const shared_ptr<State> & getState() const override {return st;}
  virtual const shared_ptr<Expression> & getExpression() const override {
    return exp;
  }
  virtual TimeProcessList & getPreviousProcesses() override {
    return MockTimeProcessList();
  }
  virtual TimeProcessList & getNextProcesses() override {
    return MockTimeProcessList();
  }

};
