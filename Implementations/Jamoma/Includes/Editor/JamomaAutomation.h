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
#include <Editor/Value/Value.h>
#include "Network/Address.h"

#include "JamomaTimeProcess.h"

using namespace OSSIA;
using namespace std;

class JamomaAutomation final : public Automation, public JamomaTimeProcess
{

private:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif

  shared_ptr<Address>    mDrivenAddress;
  std::unique_ptr<Value> mDrive;

  shared_ptr<Message>    mMessageToSend;
  std::unique_ptr<Value> mValueToSend;

public:
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaAutomation(shared_ptr<Address>,
                   const Value*);

  JamomaAutomation(const JamomaAutomation *);

  shared_ptr<Automation> clone() const override;

  ~JamomaAutomation();
#if 0
# pragma mark -
# pragma mark Execution
#endif

  shared_ptr<StateElement> offset(const TimeValue&) override;

  shared_ptr<StateElement> state() override;
#if 0
# pragma mark -
# pragma mark Execution - Implementation specific
#endif

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;
#if 0
# pragma mark -
# pragma mark Accessors
#endif

  const shared_ptr<Address> getDrivenAddress() const override;

  const Value * getDriving() const override;

private:
#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  std::unique_ptr<OSSIA::Value> computeValue(double, const Value&);
};
