#include "Editor/State.h"

namespace Generic {

class State : public OSSIA::State {

public:

  //todo dummy
  std::shared_ptr<StateElement> element;

  // Constructors, destructor, cloning
  State(){}
  State(const State * other){}
  virtual ~State() {}
  virtual std::shared_ptr<OSSIA::State> clone() const override {
    return std::shared_ptr<OSSIA::State>(new State(this));
  }

  // Lecture
  virtual void launch() const override {}

  // Std container
  virtual OSSIA::iterator begin() override {return OSSIA::iterator();}
  virtual OSSIA::iterator end() override {return OSSIA::iterator();}
  virtual OSSIA::const_iterator cbegin() const override {return OSSIA::iterator();}
  virtual OSSIA::const_iterator cend() const override {return OSSIA::iterator();}
  virtual OSSIA::size_type size() const override {return OSSIA::size_type();}
  virtual bool empty() const override {return true;}
  virtual std::shared_ptr<OSSIA::StateElement> & front() override {
    return element;
  }
  virtual const std::shared_ptr<OSSIA::StateElement> & front() const override {
    return element;
  }
  virtual std::shared_ptr<OSSIA::StateElement> & back() override {
    return element;
  }
  virtual const std::shared_ptr<OSSIA::StateElement> & back() const override {
    return element;
  }
  virtual OSSIA::iterator insert(OSSIA::const_iterator where,
				 std::shared_ptr<OSSIA::StateElement> what) override {
    return OSSIA::iterator();
  }
  virtual OSSIA::iterator erase(OSSIA::const_iterator which) override {return OSSIA::iterator();}
  virtual OSSIA::iterator erase(OSSIA::const_iterator first,
				OSSIA::const_iterator last) override {return OSSIA::iterator();}
};

}

std::shared_ptr<OSSIA::State> OSSIA::State::create() {
  return std::shared_ptr<OSSIA::State>(new Generic::State);
}
