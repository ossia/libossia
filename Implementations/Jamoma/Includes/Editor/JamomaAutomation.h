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
  
  JamomaAutomation(shared_ptr<Address>,
                   const Value*);
  
  JamomaAutomation(const JamomaAutomation *);
  
  shared_ptr<Automation> clone() const override;
  
  ~JamomaAutomation();
  
# pragma mark -
# pragma mark Execution
  
  shared_ptr<StateElement> state(const TimeValue&, const TimeValue&) override;
  
# pragma mark -
# pragma mark Execution - Implementation specific
  
  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;
  
# pragma mark -
# pragma mark Accessors
  
  const shared_ptr<Address> getDrivenAddress() const override;
  
  const Value * getDriving() const override;
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  Value* computeValueAtPosition(const Value* drive, const TimeValue& position);
};
