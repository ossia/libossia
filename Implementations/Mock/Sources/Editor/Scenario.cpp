#include "TimeProcess.cpp"
#include "Editor/Scenario.h"

using namespace OSSIA;
using namespace std;

class MockScenario : public Scenario, public MockTimeProcess {

public:

  // Dummy members
  shared_ptr<TimeNode> stnode;
  shared_ptr<TimeNode> etnode;

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
  // TODO : passer des shared ptr ?
  virtual void addConstraint(
      const TimeConstraint&,
      const TimeNode & startNode) override {}
  virtual void addConstraint(
      const TimeConstraint&,
      const TimeNode & startNode,
      const TimeNode & endNode) override {}

  // Accessors
  virtual const bool isKiller() const override {return true;}
  virtual void setKiller(bool) override {}
  // internal TimeNodes
  virtual const shared_ptr<TimeNode> & getStartNode() const override {
    return stnode;
  }
  virtual void setStartNode(shared_ptr<TimeNode> node) override {stnode = node;}
  virtual const shared_ptr<TimeNode> & getEndNode() const override {
    return etnode;
  }
  virtual void setEndNode(shared_ptr<TimeNode> node) override {etnode = node;}

};

shared_ptr<Scenario> Scenario::create() {
  return shared_ptr<Scenario>(new MockScenario());
}
