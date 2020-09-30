// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#pragma once

#include <servus/servus.h>
#include <memory>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>

namespace ossia
{
namespace max
{
class ZeroconfMinuitListener final : servus::Listener
{
  public:
    ZeroconfMinuitListener();
    ~ZeroconfMinuitListener() override;

    static std::shared_ptr<ossia::net::generic_device> find_device(const std::string& name);

    void browse();

  private:
    // since Max is not thread safe, we put connection events (from Servus thread) in a vector
    // and process it at scheduled interval triggered by Max clock
    void instanceAdded(const std::string& instance) final override;
    void instanceRemoved(const std::string& instance) final override;
    void addDevice(const std::string& instance);

    enum ConnectionEvent
    {
      ADDED = 0,
      REMOVED
    };

    static std::vector<std::shared_ptr<ossia::net::generic_device>> m_devices;
    static std::vector<std::pair<ConnectionEvent, std::string>> m_connection_events;

    servus::Servus service;
    static std::mutex m_mutex;
};

} // namespace pd
} // namespace ossia
