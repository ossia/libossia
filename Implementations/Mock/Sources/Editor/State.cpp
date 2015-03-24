#include "Editor/State.h"
#include "../Misc/Container.cpp"

using namespace OSSIA;
using namespace std;

class MockState : public State, public MockContainer<StateElement> {

public:

  // Constructors, destructor, cloning
  MockState(){}
  MockState(const MockState * other){}
  virtual ~MockState() {}
  virtual shared_ptr<State> clone() const override {
    return shared_ptr<State>(new MockState(this));
  }

  // Lecture
  virtual void launch() const override {}
};

shared_ptr<State> State::create() {
  return shared_ptr<State>(new MockState);
}
