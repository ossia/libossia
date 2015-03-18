#include "TimeProcess.cpp"
#include "Editor/Scenario.h"

#include "TTScore.h"

using namespace OSSIA;
using namespace std;

class JamomaScenario : virtual Scenario, virtual JamomaTimeProcess
{
  
private:
  // Implementation Specific
  shared_ptr<TimeNode> tnode;
  
public:
  // Constructors, destructor, cloning
  JamomaScenario()
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  JamomaScenario(const JamomaScenario * other)
  {}
  
  virtual ~JamomaScenario()
  {}
  
  virtual shared_ptr<Scenario> clone() const override
  {
    return nullptr;//shared_ptr<Scenario>(new JamomaScenario(this));
  }

  // Lecture
  virtual void play(bool log = false, string name = "") const override
  {}

  // Edition
  virtual void addConstraint(const Constraint&, const TimeNode & startNode) override
  {}
  
  virtual void addConstraint(const Constraint&, const TimeNode & startNode, const TimeNode & endNode) override
  {}

  // Accessors
  virtual const bool isKiller() const override
  {
    return true;
  }
  
  virtual void setKiller(bool) override
  {}
  
  // internal TimeNodes
  virtual const shared_ptr<TimeNode> & getStartNode() const override
  {
    return tnode;
  }
  
  virtual void setStartNode(shared_ptr<TimeNode>) override
  {}
  
  virtual const shared_ptr<TimeNode> & getEndNode() const override
  {
    return tnode;
  }
  
  virtual void setEndNode(shared_ptr<TimeNode>) override
  {}

};

shared_ptr<Scenario> Scenario::create()
{
  return nullptr;//shared_ptr<Scenario>(new JamomaScenario());
}