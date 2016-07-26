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

#include <ossia/editor/mapper/mapper.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/v1/Address.hpp>

#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

#include <thread>
#include <mutex>

using namespace OSSIA;

using namespace std::placeholders;
namespace impl
{
class JamomaMapper : public Mapper, public JamomaTimeProcess
{

private:

# pragma mark -
# pragma mark Implementation specific

  std::shared_ptr<Address>   mDriverAddress;
  std::shared_ptr<Address>   mDrivenAddress;
  Value                 mDrive;

  Message               mLastMessage;
  Value                 mValueToMap;
  mutable std::mutex    mValueToMapMutex;

  bool                  mDriverValueObserved;
  Address::iterator     mDriverValueCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaMapper(std::shared_ptr<Address>,
               std::shared_ptr<Address>,
               const Value&);

  JamomaMapper(const JamomaMapper &);

  std::shared_ptr<Mapper> clone() const override;

  ~JamomaMapper();

# pragma mark -
# pragma mark Execution

  StateElement offset(TimeValue) override;

  StateElement state() override;

# pragma mark -
# pragma mark Execution - Implementation specific

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

# pragma mark -
# pragma mark Accessors

  const std::shared_ptr<Address> getDriverAddress() const override;

  const std::shared_ptr<Address> getDrivenAddress() const override;

  const Value& getDriving() const override;

private:

# pragma mark -
# pragma mark Implementation specific

  Value computeValue(const Value&, const Value&);

  void driverValueCallback(const Value& value);
};
}
