/*!
 * \file Mapper_impl.h
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

#include "Editor/Mapper.h"
#include "Editor/Curve.h"
#include "Editor/Message.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"
#include <Editor/Value/Value.h>
#include "Network/Address.h"

#include "TimeProcess_impl.h"

#include <thread>
#include <mutex>

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;
namespace impl
{
class JamomaMapper : public Mapper, public JamomaTimeProcess
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<Address>   mDriverAddress;
  shared_ptr<Address>   mDrivenAddress;
  Value             mDrive;

  shared_ptr<Message>   mMessageToSend;
  Value             mValueToMap;
  mutable std::mutex    mValueToMapMutex;

  bool                  mDriverValueObserved;
  Address::iterator     mDriverValueCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaMapper(shared_ptr<Address>,
               shared_ptr<Address>,
               const Value&);

  JamomaMapper(const JamomaMapper &);

  shared_ptr<Mapper> clone() const override;

  ~JamomaMapper();

# pragma mark -
# pragma mark Execution

  shared_ptr<StateElement> offset(TimeValue) override;

  shared_ptr<StateElement> state() override;

# pragma mark -
# pragma mark Execution - Implementation specific

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

# pragma mark -
# pragma mark Accessors

  const shared_ptr<Address> getDriverAddress() const override;

  const shared_ptr<Address> getDrivenAddress() const override;

  const Value& getDriving() const override;

private:

# pragma mark -
# pragma mark Implementation specific

  Value computeValue(const Value&, const Value&);

  void driverValueCallback(const Value& value);
};
}
