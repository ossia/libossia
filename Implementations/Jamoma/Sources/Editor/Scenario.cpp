#include "Editor/Scenario.h"
#include "Editor/TimeNode.h"

#include "TimeProcess.cpp"
#include "../Misc/Container.cpp"

using namespace OSSIA;
using namespace std;

class JamomaScenario : virtual Scenario, virtual JamomaTimeProcess
{

private:
  
  // Implementation specific
  Container<TimeConstraint> mTimeContraints;
  Container<TimeNode>       mTimeNodes;
  
  bool                      mIsKiller;
  
public:
  
  // Life cycle
  JamomaScenario()
  {}
  
  JamomaScenario(const JamomaScenario * other)
  {}
  
  virtual ~JamomaScenario()
  {}
  
  virtual shared_ptr<Scenario> clone() const override
  {
    return nullptr;//shared_ptr<Scenario>(new JamomaScenario(this));
  }

  // Execution
  virtual void play(bool log = false, string name = "") const override
  {
    // DEBUG
    cout << "Scenario::play : working progress ...\n";
  }

  // Edition
  virtual void addConstraint(const shared_ptr<TimeConstraint> constraint, const shared_ptr<TimeNode> startNode) override
  {
    
  }
  
  virtual void addConstraint(const shared_ptr<TimeConstraint> constraint, const shared_ptr<TimeNode> startNode, const shared_ptr<TimeNode> endNode) override
  {
    mTimeContraints.push_back(constraint);
    mTimeNodes.push_back(startNode);
    mTimeNodes.push_back(endNode);
  }

  // Accessors
  virtual const bool isKiller() const override
  {
    return mIsKiller;
  }
  
  virtual void setKiller(bool isKiller) override
  {
    mIsKiller = isKiller;
  }
  
  virtual const shared_ptr<TimeNode> & getStartNode() const override
  {
    return mTimeNodes[0];
  }
  
  virtual void setStartNode(shared_ptr<TimeNode> startNode) override
  {
    mTimeNodes[0] = startNode;
  }
  
  virtual const shared_ptr<TimeNode> & getEndNode() const override
  {
    return mTimeNodes[1];
  }
  
  virtual void setEndNode(shared_ptr<TimeNode> endNode) override
  {
    mTimeNodes[1] = endNode;
  }

};

shared_ptr<Scenario> Scenario::create()
{
  return nullptr;//shared_ptr<Scenario>(new JamomaScenario());
}
