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

#include "Editor/Automation.h"
#include "Editor/Clock.h"
#include "Editor/Curve.h"
#include "Editor/Message.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"
#include "Editor/Value.h"
#include "Network/Address.h"

#include "JamomaTimeProcess.h"

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaAutomation : public Automation, public JamomaTimeProcess
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<Address>   mDrivenAddress;
  Value *               mDrive = nullptr;
  
  shared_ptr<Message>   mMessageToSend;
  Value*                mValueToSend = nullptr;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaAutomation(TimeProcess::ExecutionCallback,
                   shared_ptr<Address>,
                   const Value*);
  
  JamomaAutomation(const JamomaAutomation *);
  
  shared_ptr<Automation> clone() const override;
  
  ~JamomaAutomation();

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;
  
  void stop() const override;
  
  void pause() const override;
  
  void resume() const override;

  shared_ptr<State> state() const override;
  
# pragma mark -
# pragma mark Accessors
  
  const shared_ptr<Address> getDrivenAddress() const override;
  
  const Value * getDriving() const override;
  
  const shared_ptr<State> & getStartState() const override;
  
  const shared_ptr<State> & getEndState() const override;
  
  const shared_ptr<Clock> & getClock() const override;
  
  const shared_ptr<TimeConstraint> & getParentTimeConstraint() const override;
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  void ClockCallback(const TimeValue&, const TimeValue&, unsigned char);
  
  Value* computeValueAtPosition(const Value* drive, const TimeValue& position);
};
