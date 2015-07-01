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
#include "Editor/TimeConstraint.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"
#include "Editor/Value.h"
#include "Network/Address.h"

#include "JamomaTimeProcess.h"

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

template <typename T>
class JamomaAutomation : public Automation<T>, public JamomaTimeProcess
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  TimeProcess::ExecutionCallback  mCallback;
  
  shared_ptr<State>           mStartState;
  shared_ptr<State>           mEndState;
  shared_ptr<State>           mCurrentState;
  shared_ptr<Clock>           mClock;
  
  shared_ptr<Curve<T>>        curve;
  shared_ptr<Address>         addr;
  shared_ptr<Address>         element;
  Value*                      value;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaAutomation(TimeProcess::ExecutionCallback,
                   shared_ptr<State>,
                   shared_ptr<State>,
                   shared_ptr<Clock>);
  
  JamomaAutomation(const JamomaAutomation * other);
  
  ~JamomaAutomation();
  
  shared_ptr<Automation<T>> clone() const override;

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;

  shared_ptr<State> state() const override;
  
# pragma mark -
# pragma mark Accessors
  
  const Value * getStartValue() const override;
  
  void setStartValue(const Value*) override;
  
  const Value * getEndValue() const override;
  
  void setEndValue(const Value*) override;
  
  const shared_ptr<Curve<T>> & getCurve() const override;
  
  void setCurve(shared_ptr<Curve<T>>) override;
  
  const shared_ptr<Address> & getInputAddress() const override;

  void setInputAddress(shared_ptr<Address>) override;
  
  const shared_ptr<State> & getStartState() const override;
  
  const shared_ptr<State> & getEndState() const override;
  
  const shared_ptr<Clock> & getClock() const override;
  
  const shared_ptr<TimeConstraint> & getParentTimeConstraint() const override;
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  void ClockCallback(const TimeValue&, const TimeValue&);
};
