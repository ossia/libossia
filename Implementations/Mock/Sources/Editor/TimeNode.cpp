#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class MockTimeNode : public TimeNode {

public:

  // Dummy members
  shared_ptr<TimeEvent> element;

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
  virtual iterator begin() override {return iterator();}
  virtual iterator end() override {return iterator();}
  virtual const_iterator cbegin() const override {return const_iterator();}
  virtual const_iterator cend() const override {return const_iterator();}
  virtual size_type size() const override {return size_type();}
  virtual bool empty() const override {return true;}
  virtual shared_ptr<TimeEvent> & front() override {
	return element;
  }
  virtual const shared_ptr<TimeEvent> & front() const override {
	return element;
  }
  virtual shared_ptr<TimeEvent> & back() override {
	return element;
  }
  virtual const shared_ptr<TimeEvent> & back() const override {
	return element;
  }
  virtual iterator emplace(const_iterator where,
			  shared_ptr<State> st,
			  shared_ptr<Expression> exp) override {
	return iterator();
  }
  virtual iterator erase(const_iterator which) override {return iterator();}
  virtual iterator erase(const_iterator first,
			 const_iterator last) override {return iterator();}

};

shared_ptr<TimeNode> TimeNode::create() {
  return shared_ptr<TimeNode>(new MockTimeNode());
}
