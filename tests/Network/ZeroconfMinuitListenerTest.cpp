// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include_catch.hpp"

// Everything the listener includes comes first: only its own class is opened
// by the macro below.
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/safe_vec.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/resolve.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

#include <boost/asio/ip/tcp.hpp>

#include <boost/lexical_cast.hpp>

#include <servus/servus.h>

#include <chrono>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

/**
 * The Minuit listener of ossia-max, compiled as it is. CMake copies its sources
 * next to each other, so that its #include "ossia-max.hpp" finds the stand-in
 * of max_listener_stub instead of the header that needs the Max SDK.
 *
 * The test needs to fill s_devices and to raise the events of Servus, which
 * would otherwise take a real announcement on the network.
 */
#define private public
#include "ZeroconfMinuitListener.cpp"
#undef private

using ossia::max_binding::client;
using ossia::max_binding::ossia_max;
using ossia::max_binding::ZeroconfMinuitListener;

namespace
{
// A deadlock must fail the test, not hang the suite: all that takes the lock
// runs on a thread that can be abandoned. What it uses is held by the thread.
bool returns_in_time(std::function<void()> f)
{
  auto done = std::make_shared<std::promise<void>>();
  auto result = done->get_future();
  std::thread{[f = std::move(f), done] {
    try
    {
      f();
      done->set_value();
    }
    catch(...)
    {
      done->set_exception(std::current_exception());
    }
  }}.detach();

  if(result.wait_for(std::chrono::seconds(10)) != std::future_status::ready)
    return false;
  result.get();
  return true;
}
}

TEST_CASE("Minuit listener: a withdrawn announcement", "[minuit_listener]")
{
  auto listener = std::make_shared<ZeroconfMinuitListener>();

  CHECK(returns_in_time([listener] {
    listener->instanceRemoved("not_known");
    listener->browse();
  }));
}

TEST_CASE("Minuit listener: the clients of a withdrawn device", "[minuit_listener]")
{
  const std::string name = "withdrawn";
  auto device = std::make_shared<ossia::net::generic_device>(name);
  auto other = std::make_shared<ossia::net::generic_device>("still_here");

  ZeroconfMinuitListener::s_devices = {device, other};

  auto follower = std::make_shared<client>();
  follower->m_zeroconf = true;
  follower->m_device = device;

  // In Max, a client disconnected connects again and looks up its device
  follower->m_on_disconnect = [=] { ZeroconfMinuitListener::find_device(name); };

  auto by_url = std::make_shared<client>();
  by_url->m_device = device;

  auto of_other = std::make_shared<client>();
  of_other->m_zeroconf = true;
  of_other->m_device = other;

  auto no_device = std::make_shared<client>();
  no_device->m_zeroconf = true;

  ossia_max::instance().clients.reference()
      = {follower.get(), by_url.get(), of_other.get(), no_device.get()};

  auto listener = std::make_shared<ZeroconfMinuitListener>();
  const bool returned = returns_in_time(
      [listener, name, follower, by_url, of_other, no_device, device, other] {
    listener->instanceRemoved(name);
    listener->browse();
  });

  REQUIRE(returned);
  CHECK(follower->m_disconnected);
  CHECK(!by_url->m_disconnected);
  CHECK(!of_other->m_disconnected);
  CHECK(!no_device->m_disconnected);

  ossia_max::instance().clients.reference().clear();
  ZeroconfMinuitListener::s_devices.clear();
}
