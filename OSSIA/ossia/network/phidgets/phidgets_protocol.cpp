// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgets_protocol.hpp"
#include "phidgets_parameter.hpp"
#include "phidgets_device.hpp"
#include "phidgets_node.hpp"
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/value/value_traits.hpp>
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
    case PHIDCLASS_NOTHING: return "PHIDCLASS_NOTHING";
  }
  return "none";
}

const char* phidget_name(Phidget_ChannelClass cls)
{
  switch(cls)
  {
    case PHIDCHCLASS_NOTHING: return "PHIDCHCLASS_NOTHING";
    case PHIDCHCLASS_ACCELEROMETER: return "PHIDCHCLASS_ACCELEROMETER";
    case PHIDCHCLASS_CURRENTINPUT: return "PHIDCHCLASS_CURRENTINPUT";
    case PHIDCHCLASS_DATAADAPTER: return "PHIDCHCLASS_DATAADAPTER";
    case PHIDCHCLASS_DCMOTOR: return "PHIDCHCLASS_DCMOTOR";
    case PHIDCHCLASS_DIGITALINPUT: return "PHIDCHCLASS_DIGITALINPUT";
    case PHIDCHCLASS_DIGITALOUTPUT: return "PHIDCHCLASS_DIGITALOUTPUT";
    case PHIDCHCLASS_DISTANCESENSOR: return "PHIDCHCLASS_DISTANCESENSOR";
    case PHIDCHCLASS_ENCODER: return "PHIDCHCLASS_ENCODER";
    case PHIDCHCLASS_FREQUENCYCOUNTER: return "PHIDCHCLASS_FREQUENCYCOUNTER";
    case PHIDCHCLASS_GPS: return "PHIDCHCLASS_GPS";
    case PHIDCHCLASS_LCD: return "PHIDCHCLASS_LCD";
    case PHIDCHCLASS_GYROSCOPE: return "PHIDCHCLASS_GYROSCOPE";
    case PHIDCHCLASS_HUB: return "PHIDCHCLASS_HUB";
    case PHIDCHCLASS_CAPACITIVETOUCH: return "PHIDCHCLASS_CAPACITIVETOUCH";
    case PHIDCHCLASS_HUMIDITYSENSOR: return "PHIDCHCLASS_HUMIDITYSENSOR";
    case PHIDCHCLASS_IR: return "PHIDCHCLASS_IR";
    case PHIDCHCLASS_LIGHTSENSOR: return "PHIDCHCLASS_LIGHTSENSOR";
    case PHIDCHCLASS_MAGNETOMETER: return "PHIDCHCLASS_MAGNETOMETER";
    case PHIDCHCLASS_MESHDONGLE: return "PHIDCHCLASS_MESHDONGLE";
    case PHIDCHCLASS_PHSENSOR: return "PHIDCHCLASS_PHSENSOR";
    case PHIDCHCLASS_POWERGUARD: return "PHIDCHCLASS_POWERGUARD";
    case PHIDCHCLASS_PRESSURESENSOR: return "PHIDCHCLASS_PRESSURESENSOR";
    case PHIDCHCLASS_RCSERVO: return "PHIDCHCLASS_RCSERVO";
    case PHIDCHCLASS_RESISTANCEINPUT: return "PHIDCHCLASS_RESISTANCEINPUT";
    case PHIDCHCLASS_RFID: return "PHIDCHCLASS_RFID";
    case PHIDCHCLASS_SOUNDSENSOR: return "PHIDCHCLASS_SOUNDSENSOR";
    case PHIDCHCLASS_SPATIAL: return "PHIDCHCLASS_SPATIAL";
    case PHIDCHCLASS_STEPPER: return "PHIDCHCLASS_STEPPER";
    case PHIDCHCLASS_TEMPERATURESENSOR: return "PHIDCHCLASS_TEMPERATURESENSOR";
    case PHIDCHCLASS_VOLTAGEINPUT: return "PHIDCHCLASS_VOLTAGEINPUT";
    case PHIDCHCLASS_VOLTAGEOUTPUT: return "PHIDCHCLASS_VOLTAGEOUTPUT";
    case PHIDCHCLASS_VOLTAGERATIOINPUT: return "PHIDCHCLASS_VOLTAGERATIOINPUT";
    case PHIDCHCLASS_FIRMWAREUPGRADE: return "PHIDCHCLASS_FIRMWAREUPGRADE";
    case PHIDCHCLASS_GENERIC: return "PHIDCHCLASS_GENERIC";
    case PHIDCHCLASS_MOTORPOSITIONCONTROLLER: return "PHIDCHCLASS_MOTORPOSITIONCONTROLLER";
    case PHIDCHCLASS_BLDCMOTOR: return "PHIDCHCLASS_BLDCMOTOR";
    case PHIDCHCLASS_DICTIONARY: return "PHIDCHCLASS_DICTIONARY";
  }
  return "none";
}

template<typename Impl>
class phidget_generic_parameter : public ossia::net::parameter_base
{
  PhidgetHandle m_phidget{};
  ossia::domain m_domain;
  ossia::value m_value, m_previousValue;
  std::mutex m_valueMutex;

  auto get_phidget() const
  {
    return reinterpret_cast<decltype(Impl::phidget)>(m_phidget);
  }
  auto get_impl() const
  {
    return Impl{get_phidget()};
  }

public:
  phidget_generic_parameter(
      PhidgetHandle p, net::node_base& par)
      : ossia::net::parameter_base{par}
      , m_phidget{p}
  {
    Phidget_openWaitForAttachment(m_phidget, 1000);
    m_domain = get_impl().get_domain();
    m_value = init_value(ossia::value_trait<decltype(Impl{get_phidget()}.get_value())>::ossia_enum);
  }

  void on_first_callback_added() override
  {
    get_impl().enable_callbacks(*this);
  }
  void on_removing_last_callback() override
  {
    get_impl().disable_callbacks();
  }

  ~phidget_generic_parameter()
  {
    Phidget_close(m_phidget);
  }

  void pull_value() override
  {

  }

  ossia::value value() const override
  {
    return get_impl().get_value();
  }

  net::parameter_base& push_value(const ossia::value&) override
  {
    return *this;
  }

  net::parameter_base& push_value(ossia::value&&) override
  {
    return *this;
  }

  net::parameter_base& push_value() override
  {
    return *this;
  }

  net::parameter_base& set_value(const ossia::value& val) override
  {
    if (!val.valid())
      return *this;

    ossia::value copy;
    {
      lock_t lock(m_valueMutex);
      if (m_value.v.which() == val.v.which())
      {
        m_previousValue = std::move(m_value); // TODO also implement me for MIDI
        m_value = val;
        copy = m_value;
      }
      else
      {
        m_previousValue = std::move(m_value);
        m_value = ossia::convert(val, m_previousValue);
        copy = m_value;
      }
    }

    send(copy);
    return *this;
  }

  net::parameter_base& set_value(ossia::value&& v) override
  {
    return set_value(v);
  }

  val_type get_value_type() const override
  {
    return ossia::value_trait<decltype(Impl{get_phidget()}.get_value())>::ossia_enum;
  }

  net::parameter_base& set_value_type(val_type) override
  {
    return *this;
  }

  access_mode get_access() const override
  {
    return {};
  }

  net::parameter_base& set_access(access_mode) override
  {
    return *this;
  }

  const domain& get_domain() const override
  {
    static const domain d;
    return d;
  }

  net::parameter_base& set_domain(const domain&) override
  {
    return *this;
  }

  bounding_mode get_bounding() const override
  {
    return {};
  }

  net::parameter_base& set_bounding(bounding_mode) override
  {
    return *this;
  }

  double (*get_value)(PhidgetHandle) = {};
};
template <typename Impl>
auto make_parameter(ossia::phidget_node& parent)
{
  return std::make_unique< phidget_generic_parameter<Impl> >(parent.phidget(), parent);
}

class phidget_voltage_ratio_node : public ossia::phidget_node
{
public:
  struct Parameter
  {
    PhidgetVoltageRatioInputHandle phidget;

    float get_value()
    {
      double val = 0;
      PhidgetVoltageRatioInput_getVoltageRatio(phidget, &val);
      return val;
    }

    void set_value(float)
    {

    }

    ossia::domain get_domain()
    {
      double min, max;
      PhidgetVoltageRatioInput_getMinVoltageRatio(phidget, &min);
      PhidgetVoltageRatioInput_getMaxVoltageRatio(phidget, &max);
      return ossia::make_domain(min, max);
    }

    void enable_callbacks(phidget_generic_parameter<Parameter>& p)
    {
      PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(
            phidget,
            [] (PhidgetVoltageRatioInputHandle ch, void *ctx, const double ratio) {
          auto& p = *static_cast<phidget_generic_parameter<Parameter>*>(ctx);
          p.set_value(ratio);
      }, &p);
    }

    void disable_callbacks()
    {
      PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler(phidget, nullptr, nullptr);
    }
  };

  phidget_voltage_ratio_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
    phidget_node{hdl, dev, parent}
  {
    m_parameter = make_parameter<Parameter>(*this);
  }
};


class phidget_voltage_input_node : public ossia::phidget_node
{
public:
  struct Parameter
  {
    PhidgetVoltageInputHandle phidget;

    float get_value()
    {
      double val = 0;
      PhidgetVoltageInput_getVoltage(phidget, &val);
      return val;
    }

    void set_value(float)
    {

    }

    ossia::domain get_domain()
    {
      double min, max;
      PhidgetVoltageInput_getMinVoltage(phidget, &min);
      PhidgetVoltageInput_getMaxVoltage(phidget, &max);
      return ossia::make_domain(min, max);
    }

    void enable_callbacks(phidget_generic_parameter<Parameter>& p)
    {
      PhidgetVoltageInput_setOnVoltageChangeHandler(
            phidget,
            [] (PhidgetVoltageInputHandle ch, void *ctx, const double ratio) {
          auto& p = *static_cast<phidget_generic_parameter<Parameter>*>(ctx);
          p.set_value(ratio);
      }, &p);
    }

    void disable_callbacks()
    {
      PhidgetVoltageInput_setOnVoltageChangeHandler(phidget, nullptr, nullptr);
    }
  };

  phidget_voltage_input_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
    phidget_node{hdl, dev, parent}
  {
    m_parameter = make_parameter<Parameter>(*this);
  }
};

class phidget_digital_input_node : public ossia::phidget_node
{
public:
  struct Parameter
  {
    PhidgetDigitalInputHandle phidget;

    bool get_value()
    {
      int val = 0;
      PhidgetDigitalInput_getState(phidget, &val);
      return val == 1;
    }

    void set_value(bool)
    {

    }

    ossia::domain get_domain()
    {
      return {};
    }

    void enable_callbacks(phidget_generic_parameter<Parameter>& p)
    {
      PhidgetDigitalInput_setOnStateChangeHandler(
            phidget,
            [] (PhidgetDigitalInputHandle ch, void *ctx, const int ratio) {
          auto& p = *static_cast<phidget_generic_parameter<Parameter>*>(ctx);
          p.set_value(ratio);
      }, &p);
    }

    void disable_callbacks()
    {
      PhidgetDigitalInput_setOnStateChangeHandler(phidget, nullptr, nullptr);
    }
  };

  phidget_digital_input_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
    phidget_node{hdl, dev, parent}
  {
    m_parameter = make_parameter<Parameter>(*this);
  }
};

class phidget_digital_output_node : public ossia::phidget_node
{
public:
  struct Parameter
  {
    PhidgetDigitalOutputHandle phidget;

    bool get_value()
    {
      int val = 0;
      PhidgetDigitalOutput_getState(phidget, &val);
      return val == 1;
    }

    void set_value(bool v)
    {
      PhidgetDigitalOutput_setState(phidget, (int) v);
    }

    ossia::domain get_domain()
    {
      return {};
    }

    void enable_callbacks(phidget_generic_parameter<Parameter>& p)
    {
    }

    void disable_callbacks()
    {
    }
  };

  phidget_digital_output_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
    phidget_node{hdl, dev, parent}
  {
    m_parameter = make_parameter<Parameter>(*this);
  }
};

class phidget_hub_node : public ossia::phidget_node
{
public:
  phidget_hub_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
    phidget_node{hdl, dev, parent}
  {
  }
};

class phidget_accelerometer_node : public ossia::phidget_node
{
public:
  struct Parameter
  {
    PhidgetAccelerometerHandle phidget;

    ossia::vec3f get_value()
    {
      double val[3]{};
      PhidgetAccelerometer_getAcceleration(phidget, &val);
      return ossia::make_vec(val[0], val[1], val[2]);
    }

    void set_value(ossia::vec3f)
    {

    }

    ossia::domain get_domain()
    {
      double min[3];
      double max[3];
      PhidgetAccelerometer_getMinAcceleration(phidget, &min);
      PhidgetAccelerometer_getMaxAcceleration(phidget, &max);
      return ossia::make_domain(
            ossia::make_vec(min[0], min[1], min[2]),
            ossia::make_vec(max[0], max[1], max[2]));
    }

    void enable_callbacks(phidget_generic_parameter<Parameter>& p)
    {
      PhidgetAccelerometer_setOnAccelerationChangeHandler(
            phidget,
            [] (PhidgetAccelerometerHandle ch, void *ctx, const double val[3], double ts) {
          auto& p = *static_cast<phidget_generic_parameter<Parameter>*>(ctx);
          p.set_value(ossia::make_vec(val[0], val[1], val[2]));
      }, &p);
    }

    void disable_callbacks()
    {
      PhidgetAccelerometer_setOnAccelerationChangeHandler(phidget, nullptr, nullptr);
    }
  };

  phidget_accelerometer_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
    phidget_node{hdl, dev, parent}
  {
    m_parameter = make_parameter<Parameter>(*this);
  }
};

class phidget_gyroscope_node : public ossia::phidget_node
{
public:
  struct Parameter
  {
    PhidgetGyroscopeHandle phidget;

    std::vector<ossia::value> get_value()
    {
      int count = 0;

      double val[3];
      PhidgetGyroscope_getAngularRate(phidget, &val);
      std::vector<ossia::value> v(count);
      for(int i = 0; i < count; i++)
      {
        v[i] = (float)val[i];
      }
      return v;
    }

    void set_value(ossia::vec3f)
    {

    }

    ossia::domain get_domain()
    {
      int count = 0;
      PhidgetGyroscope_getAxisCount(phidget, &count);

      double min[3];
      double max[3];

      PhidgetGyroscope_getMinAngularRate(phidget, &min);
      PhidgetGyroscope_getMaxAngularRate(phidget, &max);

      std::vector<ossia::value> min_v(count), max_v(count);
      for(int i = 0; i < 3; i++)
      {
        min_v[i] = (float)min[i];
        max_v[i] = (float)max[i];
      }
      return ossia::make_domain(min_v, max_v);
    }

    void enable_callbacks(phidget_generic_parameter<Parameter>& p)
    {
      PhidgetGyroscope_setOnAngularRateUpdateHandler(
            phidget,
            [] (PhidgetGyroscopeHandle ch, void *ctx, const double val[3], double ts) {
          auto& p = *static_cast<phidget_generic_parameter<Parameter>*>(ctx);
          p.set_value(ossia::make_vec(val[0], val[1], val[2]));
      }, &p);
    }

    void disable_callbacks()
    {
      PhidgetGyroscope_setOnAngularRateUpdateHandler(phidget, nullptr, nullptr);
    }
  };

  phidget_gyroscope_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
    phidget_node{hdl, dev, parent}
  {
    m_parameter = make_parameter<Parameter>(*this);
  }
};


phidget_protocol::phidget_protocol()
{
  onDeviceCreated = [=](PhidgetHandle phid) {
    m_functionQueue.enqueue([=] {
      std::cerr << "EXECUTING\n";
      if(m_phidgetMap.find(phid) != m_phidgetMap.end())
        return;

      Phidget_ChannelClass cls;
      Phidget_getChannelClass(phid, &cls);

      auto par_node = get_parent(phid);
      ossia::phidget_node* phid_node{};

      switch(cls)
      {
        case PHIDCHCLASS_ACCELEROMETER:
        {
          phid_node = new phidget_accelerometer_node{phid, *m_dev, *par_node};

          /*
          uint32_t i = 1;
          PhidgetAccelerometer_getMinDataInterval(hdl, &i);
          std::cerr  << "min: " << i << std::endl;
          PhidgetAccelerometer_getMaxDataInterval(hdl, &i);
          std::cerr  << "max: " << i << std::endl;
          PhidgetAccelerometer_setDataInterval(hdl, 4);
          */
          break;
        }

        case PHIDCHCLASS_GYROSCOPE:
        {
          phid_node = new phidget_gyroscope_node{phid, *m_dev, *par_node};
          break;
        }

        case PHIDCHCLASS_VOLTAGEINPUT:
        {
          phid_node = new phidget_voltage_input_node{phid, *m_dev, *par_node};
          break;
        }
        case PHIDCHCLASS_DIGITALINPUT:
        {
          phid_node = new phidget_digital_input_node{phid, *m_dev, *par_node};
          break;
        }
        case PHIDCHCLASS_DIGITALOUTPUT:
        {
          phid_node = new phidget_digital_output_node{phid, *m_dev, *par_node};
          break;
        }
        case PHIDCHCLASS_HUB:
        {
          phid_node = new phidget_hub_node{phid, *m_dev, *par_node};
          break;
        }
        case PHIDCHCLASS_VOLTAGERATIOINPUT:
        {
          phid_node = new phidget_voltage_ratio_node{phid, *m_dev, *par_node};
          break;
        }
      }

      if(!phid_node)
        return;

      m_phidgetMap.insert({phid, phid_node});

      //phid_node->set_parameter(
      //    std::make_unique<phidget_parameter>(phid, *this, *phid_node));


      par_node->add_child(std::unique_ptr<phidget_node>(phid_node));
    });

    if (m_commandCb)
      m_commandCb();
  };

  onDeviceDestroyed = [=](PhidgetHandle phid) {
    m_functionQueue.enqueue([=] {
      auto& root = m_dev->get_root_node();
      /*
      for (auto& cld : root.children_copy())
      {
        auto phid_node = dynamic_cast<phidget_node*>(cld);
        if (phid_node)
        {
          auto addr = dynamic_cast<phidget_parameter*>(phid_node->get_parameter());
          if (addr && addr->phidget() == phid)
          {
            root.remove_child(*phid_node);
            break;
          }
        }
      }
        */
    });

    if (m_commandCb)
      m_commandCb();

  };

  open();
}

phidget_protocol::~phidget_protocol()
{
  PhidgetManager_close(m_hdl);
  PhidgetManager_delete(&m_hdl);
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
  m_dev = &dev;
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

ossia::net::node_base* phidget_protocol::get_parent(PhidgetHandle phid)
{
  PhidgetHandle parent{};
  auto err = Phidget_getParent(phid, &parent);

  int x = 0;
  auto hub_err = Phidget_getIsHubPortDevice(phid, &x);
  std::cerr << "hub port : " << x << " - err: " << hub_err << std::endl;

  if(err == EPHIDGET_OK)
  {
    if(parent)
    {
      auto par_it = m_phidgetMap.find(parent);
      if(par_it == m_phidgetMap.end())
      {
        // List all parent phidgets
        std::vector<PhidgetHandle> parents;
        parents.push_back(parent);

        err = Phidget_getParent(parent, &parent);

        while(err == EPHIDGET_OK && parent)
        {
          par_it = m_phidgetMap.find(parent);
          if(par_it == m_phidgetMap.end())
          {
            parents.push_back(parent);
            err = Phidget_getParent(parent, &parent);
          }
          else
          {
            break;
          }
        }

        // Choose the starting parent ossia node
        ossia::net::node_base* par_node = &m_dev->get_root_node();
        if(par_it != m_phidgetMap.end())
        {
          par_node = par_it->second;
        }

        // Create all child nodes
        for(auto it = parents.rbegin(); it != parents.rend(); ++it)
        {
          auto node = new ossia::phidget_node{*it, *m_dev, *par_node};
          m_phidgetMap.insert({*it, node});
          par_node->add_child(std::unique_ptr<ossia::net::node_base>(node));
          par_node = node;
        }

        /*
        fmt::MemoryWriter wr;
        ossia::net::debug_recursively(wr, m_dev->get_root_node());
        std::cerr << wr.str() << std::endl;
        */
        return par_node;
      }
      else
      {
        return par_it->second;
      }
    }
  }
  return &m_dev->get_root_node();
}

void phidget_protocol::open()
{
  PhidgetLog_enable(PHIDGET_LOG_VERBOSE, NULL);

  // Tree :
  // Phidgets:/device/...

  // For each device, create the relevant keys.
  PhidgetManager_create(&m_hdl);
  PhidgetManager_setOnAttachHandler(
        m_hdl,
        [] (PhidgetManagerHandle phidm, void *ptr, PhidgetHandle phid) {
    auto& self = *(phidget_protocol*)ptr;

    Phidget_retain(phid);
    self.m_phidgets.push_back(phid);

    if (self.onDeviceCreated)
      self.onDeviceCreated(phid);
  },
  this);

  PhidgetManager_setOnDetachHandler(
        m_hdl,
        [](PhidgetManagerHandle phidm, void *ptr, PhidgetHandle phid) {
    auto& self = *(phidget_protocol*)ptr;
    {
      auto it = ossia::find(self.m_phidgets, phid);
      if (it != self.m_phidgets.end())
      {
        if (self.onDeviceDestroyed)
          self.onDeviceDestroyed(*it);
        self.m_phidgets.erase(it);
        self.m_phidgetMap.erase(phid);
      }
    }

    {
      auto it = ossia::find(self.m_phidgetQuarantine, phid);
      if(it != self.m_phidgetQuarantine.end())
      {
        self.m_phidgetQuarantine.erase(it);
      }
    }
  },
  this);

  PhidgetManager_open(m_hdl);
}
}
