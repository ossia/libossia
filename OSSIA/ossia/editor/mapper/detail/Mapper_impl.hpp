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
#include <ossia/network/base/address.hpp>

#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

#include <thread>
#include <mutex>

using namespace std::placeholders;
namespace impl
{
class JamomaMapper : public OSSIA::Mapper, public JamomaTimeProcess
{

private:

# pragma mark -
# pragma mark Implementation specific

  OSSIA::net::Address&   mDriverAddress;
  OSSIA::net::Address&   mDrivenAddress;
  OSSIA::Value                 mDrive;

  OSSIA::Message               mLastMessage;
  OSSIA::Value                 mValueToMap;
  mutable std::mutex    mValueToMapMutex;

  bool                  mDriverValueObserved{};
  OSSIA::net::Address::callback_index     mDriverValueCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaMapper(OSSIA::net::Address&,
               OSSIA::net::Address&,
               const OSSIA::Value&);

  ~JamomaMapper();

# pragma mark -
# pragma mark Execution

  OSSIA::StateElement offset(OSSIA::TimeValue) override;

  OSSIA::StateElement state() override;

# pragma mark -
# pragma mark Execution - Implementation specific

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

# pragma mark -
# pragma mark Accessors

  const OSSIA::net::Address& getDriverAddress() const override;

  const OSSIA::net::Address& getDrivenAddress() const override;

  const OSSIA::Value& getDriving() const override;

private:

# pragma mark -
# pragma mark Implementation specific

  OSSIA::Value computeValue(const OSSIA::Value&, const OSSIA::Value&);

  void driverValueCallback(const OSSIA::Value& value);
};
}
