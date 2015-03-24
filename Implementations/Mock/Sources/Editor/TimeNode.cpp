#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"
#include "../Misc/Container.cpp"

using namespace OSSIA;
using namespace std;

class MockTimeNode : public TimeNode, public MockContainer<TimeEvent> {

public:

  // Constructors, destructor, cloning
  MockTimeNode() {}
  MockTimeNode(const MockTimeNode * other) {}
  virtual ~MockTimeNode() {}
  virtual shared_ptr<TimeNode> clone() const override {
	return shared_ptr<TimeNode>(new MockTimeNode(this));
  }

  // Lecture
  virtual void play(bool log = false, string name = "") const override {}

  // Accessors
  virtual TimeValue getDate() const override {return TimeValue();}
  virtual TimeValue getSimultaneityMargin() const override {return TimeValue();}
  virtual void setSimultaneityMargin(TimeValue) override {}

  // Std container
  virtual iterator emplace(const_iterator where,
                           shared_ptr<State> st,
                           shared_ptr<Expression> exp) override {
    return iterator();
  }

};

shared_ptr<TimeNode> TimeNode::create() {
  return shared_ptr<TimeNode>(new MockTimeNode());
}
