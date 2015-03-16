#include "TimeProcess.cpp"
#include "Editor/Scenario.h"

using namespace OSSIA;
using namespace std;

class MockScenario : public Scenario, public MockTimeProcess {

public:

  // Dummy members
  shared_ptr<TimeNode> tnode;

  // Constructors, destructor, cloning
  MockScenario() {}
  MockScenario(const MockScenario * other) {}
  virtual ~MockScenario() {}
  virtual shared_ptr<Scenario> clone() const override {
	return shared_ptr<Scenario>(new MockScenario(this));
  }

  // Lecture
  virtual void play(bool log = false, string name = "") const override {}

  // Edition
  virtual void addConstraint(
	  const Constraint&,
	  const TimeNode & startNode) override {}
  virtual void addConstraint(
	  const Constraint&,
	  const TimeNode & startNode,
	  const TimeNode & endNode) override {}

  // Accessors
  virtual const bool isKiller() const override {return true;}
  virtual void setKiller(bool) override {}
  // internal TimeNodes
  virtual const shared_ptr<TimeNode> & getStartNode() const override {
	return tnode;
  }
  virtual void setStartNode(shared_ptr<TimeNode>) override {}
  virtual const shared_ptr<TimeNode> & getEndNode() const override {
	return tnode;
  }
  virtual void setEndNode(shared_ptr<TimeNode>) override {}

};

shared_ptr<Scenario> Scenario::create() {
  return shared_ptr<Scenario>(new MockScenario());
}
