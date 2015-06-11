/*!
 * \file JamomaScenario.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include "Editor/Scenario.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeNode.h"

//#include "../Sources/Editor/TimeProcess.cpp"

using namespace OSSIA;
using namespace std;

class JamomaScenario : public Scenario//, public JamomaTimeProcess
{

private:
  
  // Implementation specific
  Container<TimeConstraint> mTimeContraints;
  Container<TimeNode>       mTimeNodes;
  
  bool                      mIsKiller;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaScenario();
  
  JamomaScenario(const JamomaScenario * other);
  
  virtual ~JamomaScenario();
  
  virtual shared_ptr<Scenario> clone() const override;

# pragma mark -
# pragma mark Execution
  
  virtual void play(bool log = false, string name = "") const override;

# pragma mark -
# pragma mark Edition
  
  virtual void addConstraint(const shared_ptr<TimeConstraint> constraint, const shared_ptr<TimeNode> startNode) override;
  
  virtual void addConstraint(const shared_ptr<TimeConstraint> constraint, const shared_ptr<TimeNode> startNode, const shared_ptr<TimeNode> endNode) override;
  
  virtual void removeConstraint(const shared_ptr<TimeConstraint> constraint) override;

# pragma mark -
# pragma mark Accessors
  
  virtual bool isKiller() const override;
  
  virtual void setKiller(bool isKiller) override;
  
  virtual const shared_ptr<TimeNode> & getStartNode() const override;
  
  virtual void setStartNode(shared_ptr<TimeNode> startNode) override;
  
  virtual const shared_ptr<TimeNode> & getEndNode() const override;
  
  virtual void setEndNode(shared_ptr<TimeNode> endNode) override;
};
