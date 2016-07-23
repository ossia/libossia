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
#include <Editor/Value/SafeValue.h>
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
  SafeValue mDrive;

  shared_ptr<Message>    mMessageToSend;
  SafeValue mValueToSend;

public:
#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  JamomaAutomation(shared_ptr<Address>, const SafeValue&);

  JamomaAutomation(const JamomaAutomation&);

  shared_ptr<Automation> clone() const override;

  ~JamomaAutomation();
#if 0
# pragma mark -
# pragma mark Execution
#endif

  shared_ptr<StateElement> offset(TimeValue) override;

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

  const SafeValue& getDriving() const override;

#if 0
# pragma mark -
# pragma mark Implementation specific
#endif
  static SafeValue computeValue(double, const SafeValue&);
};
