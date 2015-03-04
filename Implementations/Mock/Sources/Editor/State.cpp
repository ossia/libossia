#include "Editor/State.h"

using namespace OSSIA;
using namespace std;

class MockState : public State {

public:

  // Dummy members
  shared_ptr<StateElement> element;

  // Constructors, destructor, cloning
  MockState(){}
  MockState(const MockState * other){}
  virtual ~MockState() {}
  virtual shared_ptr<State> clone() const override {
    return shared_ptr<State>(new MockState(this));
  }

  // Lecture
  virtual void launch() const override {}

  // Std container
  virtual iterator begin() override {return iterator();}
  virtual iterator end() override {return iterator();}
  virtual const_iterator cbegin() const override {return iterator();}
  virtual const_iterator cend() const override {return iterator();}
  virtual size_type size() const override {return size_type();}
  virtual bool empty() const override {return true;}
  virtual shared_ptr<StateElement> & front() override {
    return element;
  }
  virtual const shared_ptr<StateElement> & front() const override {
    return element;
  }
  virtual shared_ptr<StateElement> & back() override {
    return element;
  }
  virtual const shared_ptr<StateElement> & back() const override {
    return element;
  }
  virtual iterator insert(const_iterator where,
			  shared_ptr<StateElement> what) override {
    return iterator();
  }
  virtual iterator erase(const_iterator which) override {return iterator();}
  virtual iterator erase(const_iterator first,
			 const_iterator last) override {return iterator();}
};

shared_ptr<State> State::create() {
  return shared_ptr<State>(new MockState);
}
