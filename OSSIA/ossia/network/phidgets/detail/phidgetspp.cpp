// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgetspp.hpp"

namespace ppp
{

phidget::phidget(CPhidgetHandle hdl) : m_handle{hdl}
{
  CPhidget_getDeviceClass(m_handle, &m_class);
  CPhidget_getDeviceID(m_handle, &m_id);
  CPhidget_getDeviceStatus(m_handle, &m_status);
  CPhidget_getSerialNumber(m_handle, &m_serial);

  const char* name;
  if (!CPhidget_getDeviceName(m_handle, &name))
    m_name = name;

  const char* type;
  if (!CPhidget_getDeviceType(m_handle, &type))
    m_type = type;

  const char* label;
  if (!CPhidget_getDeviceLabel(m_handle, &label))
    m_label = label;

  switch (m_class)
  {
    case PHIDCLASS_INTERFACEKIT:
    {
      m_ik = std::make_unique<interface_kit>(m_serial);

      CPhidget_set_OnError_Handler(
          m_ik->get_base_handle(),
          [](CPhidgetHandle phid, void* ptr, int errorCode,
             const char* errorString) -> int {
            auto& self = *(phidget*)ptr;
            if (self.onError)
              self.onError(errorCode, errorString);
            return EPHIDGET_OK;
          },
          this);

      // m_ik->set_sensor_change([] (int a , int b) {
      // std::cerr << "value: " << a << " : " << b << std::endl;
      // });
      break;
    }
    default:
      break;
  }
}

phidget::~phidget()
{
}

CPhidgetHandle phidget::handle() const
{
  return m_handle;
}
const std::string& phidget::name() const
{
  return m_name;
}
const std::string& phidget::type() const
{
  return m_type;
}
const std::string& phidget::label() const
{
  return m_label;
}
int phidget::serial() const
{
  return m_serial;
}
int phidget::status() const
{
  return m_status;
}

void phidget::set_label(const std::string& n)
{
  CPhidget_setDeviceLabel(m_handle, n.data());
  m_label = n;
}

phidgets_manager::phidgets_manager()
{
}

phidgets_manager::~phidgets_manager()
{
  m_phidgets.clear();
  CPhidgetManager_close(m_hdl);
  CPhidgetManager_delete(m_hdl);
}

void phidgets_manager::open()
{
  CPhidget_enableLogging(PHIDGET_LOG_VERBOSE, NULL);

  // Tree :
  // Phidgets:/device/...

  // For each device, create the relevant keys.
  CPhidgetManager_create(&m_hdl);
  CPhidgetManager_set_OnAttach_Handler(
      m_hdl,
      [](CPhidgetHandle phid, void* ptr) -> int {
        auto& self = *(phidgets_manager*)ptr;
        std::shared_ptr<phidget> p{new phidget{phid}};
        self.m_phidgets.push_back(p);

        if (self.onPhidgetCreated)
          self.onPhidgetCreated(p);

        return EPHIDGET_OK;
      },
      this);

  CPhidgetManager_set_OnDetach_Handler(
      m_hdl,
      [](CPhidgetHandle phid, void* ptr) -> int {
        auto& self = *(phidgets_manager*)ptr;
        auto it = std::find_if(
            self.m_phidgets.begin(), self.m_phidgets.end(),
            [=](auto& e) { return e->handle() == phid; });

        if (it != self.m_phidgets.end())
        {
          if (self.onPhidgetDestroyed)
            self.onPhidgetDestroyed(*it);
          self.m_phidgets.erase(it);
        }

        return EPHIDGET_OK;
      },
      this);

  CPhidgetManager_open(m_hdl);
}
}
