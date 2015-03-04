#include "Editor/State.h"

using namespace OSSIA;

class MockStateElement;

class MockState : public State {

public:

  //todo dummy
  std::shared_ptr<MockStateElement> element;

  // Constructors, destructor, cloning
  MockState(){}
  MockState(const State * other){}
  virtual ~MockState() {}
  virtual std::shared_ptr<State> clone() const override {
    return std::shared_ptr<State>(new MockState(this));
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
  virtual std::shared_ptr<StateElement> & front() override {
    return element;
  }
  virtual const std::shared_ptr<StateElement> & front() const override {
    return element;
  }
  virtual std::shared_ptr<StateElement> & back() override {
    return element;
  }
  virtual const std::shared_ptr<StateElement> & back() const override {
    return element;
  }
  virtual iterator insert(const_iterator where,
				 std::shared_ptr<StateElement> what) override {
    return iterator();
  }
  virtual iterator erase(const_iterator which) override {return iterator();}
  virtual iterator erase(const_iterator first,
				const_iterator last) override {return iterator();}
};

std::shared_ptr<State> State::create() {
  return std::shared_ptr<State>(new MockState);
}
