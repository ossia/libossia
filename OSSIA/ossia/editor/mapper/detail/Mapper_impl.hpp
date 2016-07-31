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
namespace detail
{
class mapper_impl : public ossia::mapper, public time_process_impl
{

private:

# pragma mark -
# pragma mark Implementation specific

  ossia::net::address_base&   mDriverAddress;
  ossia::net::address_base&   mDrivenAddress;
  ossia::value                 mDrive;

  ossia::message               mLastMessage;
  ossia::value                 mValueToMap;
  mutable std::mutex    mValueToMapMutex;

  bool                  mDriverValueObserved{};
  ossia::net::address_base::callback_index     mDriverValueCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  mapper_impl(ossia::net::address_base&,
               ossia::net::address_base&,
               const ossia::value&);

  ~mapper_impl();

# pragma mark -
# pragma mark Execution

  ossia::state_element offset(ossia::time_value) override;

  ossia::state_element state() override;

# pragma mark -
# pragma mark Execution - Implementation specific

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

# pragma mark -
# pragma mark Accessors

  const ossia::net::address_base& getDriverAddress() const override;

  const ossia::net::address_base& getDrivenAddress() const override;

  const ossia::value& getDriving() const override;

private:

# pragma mark -
# pragma mark Implementation specific

  ossia::value computeValue(const ossia::value&, const ossia::value&);

  void driverValueCallback(const ossia::value& value);
};
}
