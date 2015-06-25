/*!
 * \file JamomaAutomation.h
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

#include "Editor/Clock.h"
#include "Editor/Automation.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"
#include "Editor/Value.h"
#include "Network/Address.h"

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaAutomation : public Automation<double>
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<State>           mStartState;
  shared_ptr<State>           mEndState;
  shared_ptr<Clock>           mClock;
  
  shared_ptr<Curve<double>>   curve;
  shared_ptr<Address>         addr;
  shared_ptr<Address>         element;
  Value*                      value;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaAutomation(shared_ptr<State>,
                   shared_ptr<State>,
                   shared_ptr<Clock>);
  
  JamomaAutomation(const JamomaAutomation * other);
  
  ~JamomaAutomation();
  
  shared_ptr<Automation> clone() const override;

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;
  
  shared_ptr<State> state(const TimeValue&, const TimeValue& = Infinite) const override;

# pragma mark -
# pragma mark Accessors
  
  const Value * getStartValue() const override;
  
  void setStartValue(const Value*) override;
  
  const Value * getEndValue() const override;
  
  void setEndValue(const Value*) override;
  
  const shared_ptr<Curve<double>> & getCurve() const override;
  
  void setCurve(shared_ptr<Curve<double>>) override;
  
  const shared_ptr<Address> & getInputAddress() const override;

  void setInputAddress(shared_ptr<Address>) override;
  
  const shared_ptr<State> & getStartState() const override;
  
  const shared_ptr<State> & getEndState() const override;
  
  const shared_ptr<Clock> & getClock() const override;
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  void ClockCallback(const TimeValue&, const TimeValue&);
};
