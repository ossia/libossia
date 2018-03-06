// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgetspp.hpp"

namespace ppp
{

phidget_device::phidget_device(PhidgetHandle hdl) : m_handle{hdl}
{
  Phidget_retain(hdl);
  Phidget_getDeviceClass(m_handle, &m_class);
  Phidget_getDeviceID(m_handle, &m_id);
  Phidget_getDeviceSerialNumber(m_handle, &m_serial);
  //Phidget_getDeviceStatus(m_handle, &m_status);
  // Phidget_getSerialNumber(m_handle, &m_serial);

  const char* name;
  if (!Phidget_getDeviceName(m_handle, &name))
    m_name = name;

  const char* type;
  if (!Phidget_getDeviceClassName(m_handle, &type))
    m_type = type;

  const char* label;
  if (!Phidget_getDeviceLabel(m_handle, &label))
    m_label = label;

  switch (m_class)
  {
    case PHIDCLASS_INTERFACEKIT:
    {
      /*
      m_ik = std::make_unique<interface_kit>(m_serial);

      Phidget_setOnErrorHandler(
          m_ik->get_base_handle(),
          [](PhidgetHandle phid, void* ptr, int errorCode,
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
      */
      break;
    }
    default:
      break;
  }
}

phidget_device::~phidget_device()
{
  Phidget_release(&m_handle);
}

PhidgetHandle phidget_device::handle() const
{
  return m_handle;
}
const std::string& phidget_device::name() const
{
  return m_name;
}
const std::string& phidget_device::type() const
{
  return m_type;
}
const std::string& phidget_device::label() const
{
  return m_label;
}
int phidget_device::serial() const
{
  return m_serial;
}

void phidget_device::set_label(const std::string& n)
{
  Phidget_setDeviceLabel(m_handle, n.data());
  m_label = n;
}




phidget_channel::phidget_channel(
    PhidgetHandle device
    , PhidgetHandle hdl)
  : m_device{device}
  , m_handle{hdl}
{
  Phidget_getChannelClass(m_handle, &m_class);
  Phidget_getChannelSubclass(m_handle, &m_subclass);
  Phidget_getChannel(m_handle, &m_channel_id);


  const char* name;
  if (!Phidget_getChannelName(m_handle, &name))
    m_name = name;

  const char* type;
  if (!Phidget_getChannelClassName(m_handle, &type))
    m_className = type;

  switch (m_class)
  {
    default:
      break;
  }
}

phidget_channel::~phidget_channel()
{
}

PhidgetHandle phidget_channel::handle() const
{
  return m_handle;
}
const std::string& phidget_channel::name() const
{
  return m_name;
}
const std::string& phidget_channel::className() const
{
  return m_className;
}

phidgets_manager::phidgets_manager()
{
}

phidgets_manager::~phidgets_manager()
{
  m_phidgets.clear();
  PhidgetManager_close(m_hdl);
  PhidgetManager_delete(&m_hdl);
}

void phidgets_manager::open()
{
  PhidgetLog_enable(PHIDGET_LOG_VERBOSE, NULL);

  // Tree :
  // Phidgets:/device/...

  // For each device, create the relevant keys.
  PhidgetManager_create(&m_hdl);
  PhidgetManager_setOnAttachHandler(
      m_hdl,
      [] (PhidgetManagerHandle phidm, void *ptr, PhidgetHandle phid) {
        auto& self = *(phidgets_manager*)ptr;
        int ok;
        Phidget_getIsChannel(phid, &ok);
        if(ok)
        {
          std::cerr << "phidget is channel !!! \n";
          PhidgetHandle hub;
          if(Phidget_getHub(phid, &hub) == EPHIDGET_OK)
          {
            std::cerr << "\thub ok: " << hub << std::endl;
          }

          PhidgetHandle parent;
          if(Phidget_getParent(phid, &parent) == EPHIDGET_OK)
          {
            std::cerr << "\tparent ok: " << parent << std::endl;
            auto oldpar = parent;
            while(parent)
            {
              if(Phidget_getParent(parent, &parent) != EPHIDGET_OK)
              {
                std::cerr << "no parent anymore\n" ;
                break;
              }
              else if(parent == oldpar)
              {
                std::cerr << "parent == oldpar\n";
                break;
              }
              else
              {
                std::cerr << "\t new parent: " << parent << "\n";
              }
            }

          }
        }

        std::shared_ptr<phidget_device> p{new phidget_device{phid}};
        self.m_phidgets.push_back(p);

        if (self.onDeviceCreated)
          self.onDeviceCreated(p);
      },
      this);

  PhidgetManager_setOnDetachHandler(
      m_hdl,
      [](PhidgetManagerHandle phidm, void *ptr, PhidgetHandle phid) {
        auto& self = *(phidgets_manager*)ptr;
        auto it = std::find_if(
            self.m_phidgets.begin(), self.m_phidgets.end(),
            [=](auto& e) { return e->handle() == phid; });

        if (it != self.m_phidgets.end())
        {
          if (self.onDeviceDestroyed)
            self.onDeviceDestroyed(*it);
          self.m_phidgets.erase(it);
        }
      },
      this);

  PhidgetManager_open(m_hdl);
}
}
