#pragma once
#include <ossia/network/phidgets/detail/sensors.hpp>
#include <algorithm>
#include <eggs/variant.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
namespace ppp
{
class phidgets_manager;

struct phidget
{
  friend class phidgets_manager;

public:
  ~phidget();

  std::function<void(int, const char*)> onError;

  CPhidgetHandle handle() const;
  const std::string& name() const;
  const std::string& type() const;
  const std::string& label() const;
  int serial() const;
  int status() const;

  void set_label(const std::string& n);
  interface_kit* impl() const
  {
    return m_ik.get();
  }

private:
  phidget(CPhidgetHandle hdl);
  phidget(const phidget&) = delete;
  phidget(phidget&&) = delete;
  phidget& operator=(const phidget&) = delete;
  phidget& operator=(phidget&&) = delete;

  CPhidgetHandle m_handle{};

  CPhidget_DeviceClass m_class{};
  CPhidget_DeviceID m_id{};

  std::string m_name;
  std::string m_type;
  std::string m_label;
  int m_serial{};
  int m_status{};

  std::unique_ptr<interface_kit> m_ik;
};
using phidget_ptr = std::shared_ptr<phidget>;
class phidgets_manager
{
public:
  phidgets_manager();
  phidgets_manager(const phidgets_manager&) = delete;
  phidgets_manager(phidgets_manager&&) = delete;
  phidgets_manager& operator=(const phidgets_manager&) = delete;
  phidgets_manager& operator=(phidgets_manager&&) = delete;
  ~phidgets_manager();

  std::function<void(phidget_ptr)> onPhidgetCreated;
  std::function<void(phidget_ptr)> onPhidgetDestroyed;

  void open();

  const std::vector<phidget_ptr>& phidgets() const
  {
    return m_phidgets;
  }

private:
  CPhidgetManagerHandle m_hdl{};

  std::vector<phidget_ptr> m_phidgets;
};
}
