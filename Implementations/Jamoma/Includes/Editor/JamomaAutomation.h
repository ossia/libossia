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
#include "Editor/Automation.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"
#include "Editor/Value.h"
#include "Network/Address.h"

using namespace OSSIA;
using namespace std;

class JamomaAutomation : public Automation<double>
{
  
private:
  
  // Implementation Specific
  shared_ptr<Curve<double>> curve;
  shared_ptr<Address> addr;
  shared_ptr<Address> element;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaAutomation();
  
  JamomaAutomation(const JamomaAutomation * other);
  
  ~JamomaAutomation();
  
  shared_ptr<Automation> clone() const override;

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;

# pragma mark -
# pragma mark Accessors
  
  Value getStartValue() const override;
  
  void setStartValue(Value) override;
  
  Value getEndValue() const override;
  
  void setEndValue(Value) override;
  
  const shared_ptr<Curve<double>> & getCurve() const override;
  
  void setCurve(shared_ptr<Curve<double>>) override;
  
  const shared_ptr<Address> & getInputAddress() const override;

  void setInputAddress(shared_ptr<Address>) override;
  
  const std::shared_ptr<TimeConstraint> & getParentTimeConstraint() const override;
  
  const std::shared_ptr<State> & getStartState() const override;
  
  const std::shared_ptr<State> & getEndState() const override;
};
