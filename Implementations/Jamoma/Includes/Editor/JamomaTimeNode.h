/*!
 * \file JamomaTimeNode.h
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
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeNode : public TimeNode
{
  
private:
  
  // Implementation specific
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeNode();
  
  JamomaTimeNode(const JamomaTimeNode * other);
  
  virtual ~JamomaTimeNode();
  
  virtual shared_ptr<TimeNode> clone() const override;

# pragma mark -
# pragma mark Execution
  
  virtual void play(bool log = false, string name = "") const override;

# pragma mark -
# pragma mark Accessors
  
  virtual TimeValue getDate() const override;
  
  virtual TimeValue getSimultaneityMargin() const override;
  
  virtual void setSimultaneityMargin(TimeValue) override;
};
