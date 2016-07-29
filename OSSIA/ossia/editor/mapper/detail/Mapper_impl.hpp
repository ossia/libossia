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
class JamomaMapper : public ossia::mapper, public JamomaTimeProcess
{

private:

# pragma mark -
# pragma mark Implementation specific

  ossia::net::address&   mDriverAddress;
  ossia::net::address&   mDrivenAddress;
  ossia::value                 mDrive;

  ossia::Message               mLastMessage;
  ossia::value                 mValueToMap;
  mutable std::mutex    mValueToMapMutex;

  bool                  mDriverValueObserved{};
  ossia::net::address::callback_index     mDriverValueCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaMapper(ossia::net::address&,
               ossia::net::address&,
               const ossia::value&);

  ~JamomaMapper();

# pragma mark -
# pragma mark Execution

  ossia::StateElement offset(ossia::time_value) override;

  ossia::StateElement state() override;

# pragma mark -
# pragma mark Execution - Implementation specific

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

# pragma mark -
# pragma mark Accessors

  const ossia::net::address& getDriverAddress() const override;

  const ossia::net::address& getDrivenAddress() const override;

  const ossia::value& getDriving() const override;

private:

# pragma mark -
# pragma mark Implementation specific

  ossia::value computeValue(const ossia::value&, const ossia::value&);

  void driverValueCallback(const ossia::value& value);
};
}
