// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgets_protocol.hpp"
#include "phidgets_parameter.hpp"
#include "phidgets_device.hpp"
#include "phidgets_node.hpp"

namespace ossia
{
class hub
{
  hub(PhidgetHandle h)
  {
    //m_hub = h;

  }
  PhidgetHubHandle m_hub;
  PhidgetHandle m_handle;
};
class vint
{
  vint(PhidgetHandle h)
  {
    //m_hub = h;
  }
  PhidgetHandle m_handle;
};
const char* phidget_name(Phidget_DeviceClass cls)
{
  switch(cls)
  {
    case PHIDCLASS_ACCELEROMETER: return "PHIDCLASS_ACCELEROMETER";
    case PHIDCLASS_ADVANCEDSERVO: return "PHIDCLASS_ADVANCEDSERVO";
    case PHIDCLASS_ANALOG: return "PHIDCLASS_ANALOG";
    case PHIDCLASS_BRIDGE: return "PHIDCLASS_BRIDGE";
    case PHIDCLASS_ENCODER: return "PHIDCLASS_ENCODER";
    case PHIDCLASS_FREQUENCYCOUNTER: return "PHIDCLASS_FREQUENCYCOUNTER";
    case PHIDCLASS_GPS: return "PHIDCLASS_GPS";
    case PHIDCLASS_HUB: return "PHIDCLASS_HUB";
    case PHIDCLASS_INTERFACEKIT: return "PHIDCLASS_INTERFACEKIT";
    case PHIDCLASS_IR : return "PHIDCLASS_IR";
    case PHIDCLASS_LED: return "PHIDCLASS_LED";
    case PHIDCLASS_MESHDONGLE: return "PHIDCLASS_MESHDONGLE";
    case PHIDCLASS_MOTORCONTROL: return "PHIDCLASS_MOTORCONTROL";
    case PHIDCLASS_PHSENSOR: return "PHIDCLASS_PHSENSOR";
    case PHIDCLASS_RFID: return "PHIDCLASS_RFID";
    case PHIDCLASS_SERVO: return "PHIDCLASS_SERVO";
    case PHIDCLASS_SPATIAL: return "PHIDCLASS_SPATIAL";
    case PHIDCLASS_STEPPER: return "PHIDCLASS_STEPPER";
    case PHIDCLASS_TEMPERATURESENSOR: return "PHIDCLASS_TEMPERATURESENSOR";
    case PHIDCLASS_TEXTLCD: return "PHIDCLASS_TEXTLCD";
    case PHIDCLASS_VINT: return "PHIDCLASS_VINT";
    case PHIDCLASS_GENERIC: return "PHIDCLASS_GENERIC";
    case PHIDCLASS_FIRMWAREUPGRADE: return "PHIDCLASS_FIRMWAREUPGRADE";
    case PHIDCLASS_DICTIONARY: return "PHIDCLASS_DICTIONARY";
  }
  return "none";
}
phidget_protocol::phidget_protocol()
{
  m_mgr.onDeviceCreated = [=](ppp::phidget_ptr phid) {
    m_functionQueue.enqueue([=] {

      auto phid_node = new phidget_node(*m_dev, *m_dev);
      //phid_node->set_parameter(
      //    std::make_unique<phidget_parameter>(phid, *this, *phid_node));
      m_dev->add_child(std::unique_ptr<phidget_node>(phid_node));
      std::cerr << phid->name() << " "  << phid->label() << " "
                << phid->serial() << " " << phid->status() << " "
                << (int)phid->device_class() << " " << (int)phid->device_id() << " "
                << std::endl;

      std::cerr << phidget_name(phid->device_class());
      {
        const char* name = " NONAME ";
        if(Phidget_getChannelClassName(phid->handle(), &name) == EPHIDGET_OK)
          std::cerr << "Channel Class Name: " << name << std::endl;
      }

      {
        int hubport{-1};
        Phidget_getHubPort(phid->handle(), &hubport);
        std::cerr << "\thub port: " << hubport << std::endl;
      }
      {
        int channel{-1};
        Phidget_getIsChannel(phid->handle(), &channel);
        std::cerr << "\tis channel: " << channel << std::endl;

        if(channel)
        {
          int c{-1};
          Phidget_getChannel(phid->handle(), &c);
          std::cerr << "\tchannel: " << c << std::endl;
        }
      }
      {
        int port{-1};
        Phidget_getIsHubPortDevice(phid->handle(), &port);
        std::cerr << "\tis hubportdevice: " << port << std::endl;
      }


      PhidgetHandle arr[128];
      size_t sz;
      if(auto r = Phidget_getChildDevices(phid->handle(), (PhidgetHandle *)arr, &sz); r == EPHIDGET_OK)
      {
        std::cerr << phid->channel_count() << " == " << sz << std::endl;

        for(int i = 0; i < phid->channel_count(); i++)
        {
          PhidgetHandle hdl;
        }

      }
      else
      {
        const char* res;
        Phidget_getErrorDescription(r, &res);
        std::cerr << "getChildDevices error: " << r << " " << res << std::endl ;

      }
    });

    if (m_commandCb)
      m_commandCb();
  };
  m_mgr.onDeviceDestroyed = [=](ppp::phidget_ptr phid) {
    m_functionQueue.enqueue([=] {
      for (auto& cld : m_dev->children_copy())
      {
        auto phid_node = dynamic_cast<phidget_node*>(cld);
        if (phid_node)
        {
          auto addr = dynamic_cast<phidget_parameter*>(phid_node->get_parameter());
          if (addr && addr->phidget() == phid)
          {
            m_dev->remove_child(*phid_node);
            break;
          }
        }
      }
    });

    if (m_commandCb)
      m_commandCb();

  };

  m_mgr.open();
}

bool phidget_protocol::pull(net::parameter_base&)
{
  return true;
}

std::future<void> phidget_protocol::pull_async(net::parameter_base&)
{
  return {};
}

void phidget_protocol::request(net::parameter_base&)
{
}

bool phidget_protocol::push(const net::parameter_base&)
{
  return true;
}

bool phidget_protocol::observe(net::parameter_base&, bool)
{
  return true;
}

bool phidget_protocol::observe_quietly(net::parameter_base&, bool)
{
  return true;
}

bool phidget_protocol::update(net::node_base& node_base)
{
  return true;
}

void phidget_protocol::set_device(net::device_base& dev)
{
  if (auto d = dynamic_cast<phidget_device*>(&dev))
    m_dev = d;
}

void phidget_protocol::set_command_callback(std::function<void()> c)
{
  m_commandCb = c;
}

void phidget_protocol::run_commands()
{
  bool ok = false;
  std::function<void()> cmd;
  do
  {
    ok = m_functionQueue.try_dequeue(cmd);
    if (ok && cmd)
      cmd();
  } while (ok);
}

bool phidget_protocol::push_raw(const net::full_parameter_data &)
{
    return false;
}
}
