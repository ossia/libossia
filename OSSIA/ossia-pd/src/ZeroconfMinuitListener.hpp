#pragma once

#include <m_pd.h>
#include <servus/servus.h>
#include <memory>

#include <ossia/network/generic/generic_device.hpp>

namespace ossia
{
namespace pd
{
class ZeroconfMinuitListener final : servus::Listener
{
  public:
    ZeroconfMinuitListener();
    ~ZeroconfMinuitListener() override;

    static ossia::net::generic_device* find_device(const std::string& name);

    void browse();

  private:
    void instanceAdded(const std::string& instance) final override;
    void instanceRemoved(const std::string& instance) final override;

    static std::vector<std::shared_ptr<ossia::net::generic_device>> m_devices;
    static std::vector<std::vector<std::shared_ptr<ossia::net::generic_device>>::iterator> m_zombie_devices;

    servus::Servus service;
    std::mutex m_mutex;
};

} // namespace pd
} // namespace ossia
