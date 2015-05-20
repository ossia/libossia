#include "Editor/TimeConstraint.h"
#include "Editor/TimeValue.h"
#include "../Misc/Container.cpp"

using namespace OSSIA;
using namespace std;

class MockTimeConstraint :
    public TimeConstraint, public MockContainer<TimeProcess> {

public:

  // Dummy members
  shared_ptr<TimeNode> tnode;
  shared_ptr<State> st;

  // Constructors, destructor, cloning
  MockTimeConstraint(TimeValue nominal,
			 TimeValue min,
             TimeValue max) {}
  MockTimeConstraint(const MockTimeConstraint * other){}
  virtual ~MockTimeConstraint() {}
  virtual shared_ptr<TimeConstraint> clone() const override {
	return shared_ptr<TimeConstraint>(new MockTimeConstraint(this));
  }

  // Lecture
  virtual void play(bool log = false, string name = "") const override {}

  // Navigation
  virtual const shared_ptr<TimeNode> & getStartNode() const override{
	return tnode;
  }
  virtual const shared_ptr<TimeNode> & getEndNode() const override {
	return tnode;
  }

  // Accessors
  virtual const shared_ptr<State> & getStartState() const override {
	return st;
  }
  virtual void setStartState(shared_ptr<State>) override {}
  virtual const shared_ptr<State> & getEndState() const override {
	return st;
  }
  virtual void setEndState(shared_ptr<State>) override {}

};

shared_ptr<TimeConstraint> TimeConstraint::create(TimeValue nominal,
						  TimeValue min,
						  TimeValue max) {
  return shared_ptr<TimeConstraint>(new MockTimeConstraint(nominal, min, max));
}
