#pragma once

#include <servus/servus.h>
#include <memory>

#include <ossia/network/generic/generic_device.hpp>

namespace ossia
{
namespace max
{
class ZeroconfOscqueryListener final : servus::Listener
{
  public:
    ZeroconfOscqueryListener();
    ~ZeroconfOscqueryListener() override;

    static std::shared_ptr<ossia::net::generic_device> find_device(const std::string& name);
    static std::vector<std::shared_ptr<ossia::net::generic_device>> get_devices() { return s_devices; }

    void browse();

    enum ConnectionEvent
    {
      ADDED = 0,
      REMOVED
    };

  private:
    // since Max is not thread safe, we put connection events (from Servus thread) in a vector
    // and process it at scheduled interval triggered by Max clock
    void instanceAdded(const std::string& instance) final override;
    void instanceRemoved(const std::string& instance) final override;
    void addInstance(const std::string& instance);
    void removeInstance(const std::string& instance);

    static std::vector<std::shared_ptr<ossia::net::generic_device>> s_devices;
    static std::vector<std::pair<ConnectionEvent, std::string>> s_connection_events;

    servus::Servus service;
    static std::mutex s_mutex;
};

} // namespace pd
} // namespace ossia
