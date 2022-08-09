#pragma once

#include <ossia/network/generic/generic_device.hpp>

#include <servus/servus.h>

#include <m_pd.h>

#include <memory>

namespace ossia::pd
{
class ZeroconfOscqueryListener final : servus::Listener
{
public:
  ZeroconfOscqueryListener();
  ~ZeroconfOscqueryListener() override;

  static ossia::net::generic_device* find_device(const std::string& name);

  void browse();

private:
  void instanceAdded(const std::string& instance) final override;
  void instanceRemoved(const std::string& instance) final override;

  static std::vector<std::shared_ptr<ossia::net::generic_device>> m_devices;
  static std::vector<std::string> m_zombie_devices;

  servus::Servus service;
  static std::mutex m_mutex;
};

} // namespace ossia
