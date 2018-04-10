#pragma once
#include <ossia/network/phidgets/phidgets_node.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/detail/logger.hpp>
namespace ossia
{

template<typename Impl>
class phidget_generic_parameter : public ossia::net::parameter_base
{
    PhidgetHandle m_phidget{};
    ossia::domain m_domain;
    //ossia::value m_value, m_previousValue;
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
      m_domain = get_impl().get_domain();
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
    }

    void pull_value() override
    {

    }

    ossia::value value() const override
    {
      return get_impl().get_value();
    }

    net::parameter_base& push_value(const ossia::value& val) override { set_value(val); return *this; }
    net::parameter_base& push_value(ossia::value&& val) override { set_value(val); return *this; }
    net::parameter_base& push_value() override { return *this; }

    net::parameter_base& set_value(const ossia::value& val) override
    {
      if (!val.valid())
        return *this;

      get_impl().set_value(ossia::convert<decltype(Impl{get_phidget()}.get_value())>(val));
      send(val);
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

    net::parameter_base& set_value_type(val_type) override { return *this; }

    access_mode get_access() const override { return ossia::access_mode::BI; }
    net::parameter_base& set_access(access_mode) override { return *this; }

    const domain& get_domain() const override { return m_domain; }
    net::parameter_base& set_domain(const domain&) override { return *this; }

    bounding_mode get_bounding() const override { return ossia::bounding_mode::FREE; }
    net::parameter_base& set_bounding(bounding_mode) override { return *this; }
};

template<typename Impl>
class phidget_control_parameter : public ossia::net::parameter_base
{
    auto get_phidget() const
    {
      return reinterpret_cast<decltype(Impl::phidget)>(m_phidget);
    }
    auto get_impl() const
    {
      return Impl{get_phidget()};
    }

    PhidgetHandle m_phidget{};
    ossia::domain m_domain;

    using value_type = decltype(std::declval<Impl>().get_value());
    value_type m_value{};


  public:
    phidget_control_parameter(
        PhidgetHandle p, net::node_base& par)
      : ossia::net::parameter_base{par}
      , m_phidget{p}
    {
      m_domain = get_impl().get_domain();
    }

    ~phidget_control_parameter() override { }

    ossia::value value() const override
    {
      return m_value;
    }

    void pull_value() override
    {

    }
    net::parameter_base& push_value(const ossia::value& val) override { set_value(val); return *this; }
    net::parameter_base& push_value(ossia::value&& val) override { set_value(val); return *this; }
    net::parameter_base& push_value() override { set_value(value()); return *this; }

    net::parameter_base& set_value(const ossia::value& val) override
    {
      if (!val.valid())
        return *this;

      auto v = ossia::convert<value_type>(val);
      m_value = v;
      get_impl().set_value(v);
      send(val);
      return *this;
    }


    net::parameter_base& set_value(ossia::value&& v) override { return set_value(v); }

    val_type get_value_type() const override
    {
      return ossia::value_trait<decltype(Impl{get_phidget()}.get_value())>::ossia_enum;
    }

    net::parameter_base& set_value_type(val_type) override { return *this; }

    access_mode get_access() const override { return ossia::access_mode::BI; }
    net::parameter_base& set_access(access_mode) override { return *this; }

    const domain& get_domain() const override { return m_domain; }
    net::parameter_base& set_domain(const domain&) override { return *this; }

    bounding_mode get_bounding() const override { return ossia::bounding_mode::FREE; }
    net::parameter_base& set_bounding(bounding_mode) override { return *this; }
};


class phidget_open_parameter : public ossia::net::parameter_base
{
    PhidgetHandle m_phidget{};
    bool m_open{false};

  public:
    phidget_open_parameter(
        PhidgetHandle p, net::node_base& par)
      : ossia::net::parameter_base{par}
      , m_phidget{p}
    {
    }

    ~phidget_open_parameter() override
    {
      Phidget_close(m_phidget);
    }

    ossia::value value() const override { return m_open; }

    void pull_value() override { }

    net::parameter_base& push_value(const ossia::value& val) override { set_value(val); return *this; }
    net::parameter_base& push_value(ossia::value&& val) override { set_value(val); return *this; }
    net::parameter_base& push_value() override { return *this; }

    net::parameter_base& set_value(const ossia::value& val) override
    {
      if (!val.valid())
        return *this;

      auto res = ossia::convert<bool>(val);
      if(res != value())
      {
        if(res)
        {
          auto err = Phidget_openWaitForAttachment(m_phidget, 1000);
          if(err != EPHIDGET_OK)
          {
            const char* err_txt{};
            Phidget_getErrorDescription(err, &err_txt);
            if(err_txt)
              ossia::logger().error("Phidget Open error: {} ({})", this->get_node().osc_address(), err_txt);
            else
              ossia::logger().error("Phidget Open error: {} (timeout)", this->get_node().osc_address());

            res = false;
          }
        }
        else
        {
          Phidget_close(m_phidget);
        }
        m_open = res;
        send(res);
      }
      return *this;
    }

    net::parameter_base& set_value(ossia::value&& v) override { return set_value(v); }

    val_type get_value_type() const override { return ossia::val_type::BOOL; }
    net::parameter_base& set_value_type(val_type) override { return *this; }

    access_mode get_access() const override { return ossia::access_mode::BI; }
    net::parameter_base& set_access(access_mode) override { return *this; }

    const domain& get_domain() const override { static const domain d = ossia::make_domain(false, true); return d; }
    net::parameter_base& set_domain(const domain&) override { return *this; }

    bounding_mode get_bounding() const override { return ossia::bounding_mode::CLIP; }
    net::parameter_base& set_bounding(bounding_mode) override { return *this; }
};

class phidget_channel_parameter : public ossia::net::parameter_base
{
    PhidgetHandle m_phidget{};

  public:
    phidget_channel_parameter(
        PhidgetHandle p, net::node_base& par)
      : ossia::net::parameter_base{par}
      , m_phidget{p}
    {
    }

    ~phidget_channel_parameter() override { }

    ossia::value value() const override
    {
      int c{};
      Phidget_getChannel(m_phidget, &c);
      return c;
    }

    void pull_value() override { }

    net::parameter_base& push_value(const ossia::value& val) override { return *this; }
    net::parameter_base& push_value(ossia::value&& val) override { return *this; }
    net::parameter_base& push_value() override { return *this; }
    net::parameter_base& set_value(const ossia::value& val) override { return *this; }
    net::parameter_base& set_value(ossia::value&& v) override { return *this; }

    val_type get_value_type() const override { return ossia::val_type::INT; }
    net::parameter_base& set_value_type(val_type) override { return *this; }

    access_mode get_access() const override { return ossia::access_mode::GET; }
    net::parameter_base& set_access(access_mode) override { return *this; }

    const domain& get_domain() const override { static const domain d = ossia::make_domain(0, 1024); return d; }
    net::parameter_base& set_domain(const domain&) override { return *this; }

    bounding_mode get_bounding() const override { return ossia::bounding_mode::CLIP; }
    net::parameter_base& set_bounding(bounding_mode) override { return *this; }
};

template <typename Impl, typename Parent>
auto make_parameter(Parent& parent)
{
  return std::make_unique< phidget_generic_parameter<Impl> >(parent.phidget(), parent);
}


template<typename Impl>
struct Control_Double_parameter
{
    typename Impl::handle_type phidget;

    float get_value()
    {
      double val;
      if(Impl::Get(phidget, &val) != EPHIDGET_OK)
        val = 0.;
      return (float)val;
    }

    void set_value(float v)
    {
      Impl::Set(phidget, v);
    }

    ossia::domain get_domain()
    {
      double min, max;
      if(Impl::Min(phidget, &min) != EPHIDGET_OK)
        min = 0.;
      if(Impl::Max(phidget, &max) != EPHIDGET_OK)
        max = 0.;
      return ossia::make_domain(min, max);
    }

    void enable_callbacks(phidget_generic_parameter<Control_Double_parameter>& p)
    {
    }

    void disable_callbacks()
    {
    }
};

template<typename Impl>
struct Control_UInt_parameter
{
    typename Impl::handle_type phidget;

    int get_value()
    {
      uint32_t val;
      if(Impl::Get(phidget, &val) != EPHIDGET_OK)
        val = 0;
      return (int)val;
    }

    void set_value(int v)
    {
      Impl::Set(phidget, v);
    }

    ossia::domain get_domain()
    {
      uint32_t min, max;
      if(Impl::Min(phidget, &min) != EPHIDGET_OK)
        min = 0;
      if(Impl::Max(phidget, &max) != EPHIDGET_OK)
        max = 0;
      return ossia::make_domain((int)min, (int)max);
    }

    void enable_callbacks(phidget_generic_parameter<Control_UInt_parameter>& p)
    {
    }

    void disable_callbacks()
    {
    }
};

template<typename Impl>
struct Double_parameter
{
    typename Impl::handle_type phidget;

    float get_value()
    {
      double val;
      if(Impl::Get(phidget, &val) != EPHIDGET_OK)
        val = 0.;
      return (float)val;
    }

    void set_value(float)
    {

    }

    ossia::domain get_domain()
    {
      double min, max;
      if(Impl::Min(phidget, &min) != EPHIDGET_OK)
        min = 0.;
      if(Impl::Max(phidget, &max) != EPHIDGET_OK)
        max = 0.;
      return ossia::make_domain(min, max);
    }

    void enable_callbacks(phidget_generic_parameter<Double_parameter>& p)
    {
      Impl::Change(
            phidget,
            [] (typename Impl::handle_type ch, void *ctx, const double ratio) {
        auto& p = *static_cast<phidget_generic_parameter<Double_parameter>*>(ctx);
        p.set_value(ratio);
      }, &p);
    }

    void disable_callbacks()
    {
      Impl::Change(phidget, nullptr, nullptr);
    }
};

template<typename Impl>
struct UInt_parameter
{
    typename Impl::handle_type phidget;

    int get_value()
    {
      uint32_t val;
      if(Impl::Get(phidget, &val) != EPHIDGET_OK)
        val = 0;
      return (int)val;
    }

    void set_value(int)
    {

    }

    ossia::domain get_domain()
    {
      uint32_t min, max;
      if(Impl::Min(phidget, &min) != EPHIDGET_OK)
        min = 0;
      if(Impl::Max(phidget, &max) != EPHIDGET_OK)
        max = 0;
      return ossia::make_domain((int)min, (int)max);
    }

    void enable_callbacks(phidget_generic_parameter<UInt_parameter>& p)
    {
      Impl::Change(
            phidget,
            [] (typename Impl::handle_type ch, void *ctx, const uint32_t ratio) {
        auto& p = *static_cast<phidget_generic_parameter<UInt_parameter>*>(ctx);
        p.set_value((int)ratio);
      }, &p);
    }

    void disable_callbacks()
    {
      Impl::Change(phidget, nullptr, nullptr);
    }
};

template<typename Impl>
struct Vec3_parameter
{
    typename Impl::handle_type phidget;

    ossia::vec3f get_value()
    {
      double val[3]{};
      if(Impl::Min(phidget, &val) != EPHIDGET_OK)
      { val[0] = 0; val[1] = 0; val[2] = 0; }
      return ossia::make_vec(val[0], val[1], val[2]);
    }

    void set_value(ossia::vec3f)
    {

    }

    ossia::domain get_domain()
    {
      double min[3];
      double max[3];
      if(Impl::Min(phidget, &min) != EPHIDGET_OK)
      { min[0] = 0; min[1] = 0; min[2] = 0; }
      if(Impl::Max(phidget, &max) != EPHIDGET_OK)
      { max[0] = 0; max[1] = 0; max[2] = 0; }
      return ossia::make_domain(
            ossia::make_vec(min[0], min[1], min[2]),
            ossia::make_vec(max[0], max[1], max[2]));
    }

    void enable_callbacks(phidget_generic_parameter<Vec3_parameter>& p)
    {
      Impl::Change(
            phidget,
            [] (typename Impl::handle_type ch, void *ctx, const double val[3], double ts) {
        auto& p = *static_cast<phidget_generic_parameter<Vec3_parameter>*>(ctx);
        p.set_value(ossia::make_vec(val[0], val[1], val[2]));
      }, &p);
    }

    void disable_callbacks()
    {
      Impl::Change(phidget, nullptr, nullptr);
    }
};

class phidget_control_node : public ossia::net::generic_node
{
  public:
    template<typename Desc>
    phidget_control_node(Desc desc, std::string name, PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      generic_node{name, dev, parent}
    {
      m_parameter = std::make_unique< phidget_control_parameter<Desc> >(hdl, *this);
      m_device.on_parameter_created(*m_parameter);
    }
};

class phidget_open_node : public ossia::net::generic_node
{
  public:
    phidget_open_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      generic_node{"open", dev, parent}
    {
      m_parameter = std::make_unique< phidget_open_parameter >(hdl, *this);
      m_device.on_parameter_created(*m_parameter);
    }
};

class phidget_channel_node : public ossia::net::generic_node
{
  public:
    phidget_channel_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      generic_node{"channel", dev, parent}
    {
      m_parameter = std::make_unique< phidget_channel_parameter >(hdl, *this);
      m_device.on_parameter_created(*m_parameter);
    }
};


class phidget_voltage_ratio_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetVoltageRatioInputHandle;
        static const constexpr auto Get = PhidgetVoltageRatioInput_getVoltageRatio;
        static const constexpr auto Change = PhidgetVoltageRatioInput_setOnVoltageRatioChangeHandler;
        static const constexpr auto Min = PhidgetVoltageRatioInput_getMinVoltageRatio;
        static const constexpr auto Max = PhidgetVoltageRatioInput_getMaxVoltageRatio;
    };
    struct rate_desc
    {
        using handle_type = PhidgetVoltageRatioInputHandle;
        static const constexpr auto Get = PhidgetVoltageRatioInput_getDataInterval;
        static const constexpr auto Set = PhidgetVoltageRatioInput_setDataInterval;
        static const constexpr auto Min = PhidgetVoltageRatioInput_getMinDataInterval;
        static const constexpr auto Max = PhidgetVoltageRatioInput_getMaxDataInterval;
    };
    struct trigger_desc
    {
        using handle_type = PhidgetVoltageRatioInputHandle;
        static const constexpr auto Get = PhidgetVoltageRatioInput_getVoltageRatioChangeTrigger;
        static const constexpr auto Set = PhidgetVoltageRatioInput_setVoltageRatioChangeTrigger;
        static const constexpr auto Min = PhidgetVoltageRatioInput_getMinVoltageRatioChangeTrigger;
        static const constexpr auto Max = PhidgetVoltageRatioInput_getMaxVoltageRatioChangeTrigger;
    };

    phidget_voltage_ratio_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Double_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);

      add_child_simple(std::make_unique<ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_Double_parameter<trigger_desc>{}, "trigger", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
          m_children[3]->get_parameter()->push_value();
        }
      });
    }
};


class phidget_current_input_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetCurrentInputHandle;
        static const constexpr auto Get = PhidgetCurrentInput_getCurrent;
        static const constexpr auto Change = PhidgetCurrentInput_setOnCurrentChangeHandler;
        static const constexpr auto Min = PhidgetCurrentInput_getMinCurrent;
        static const constexpr auto Max = PhidgetCurrentInput_getMaxCurrent;
    };

    struct rate_desc
    {
        using handle_type = PhidgetCurrentInputHandle;
        static const constexpr auto Get = PhidgetCurrentInput_getDataInterval;
        static const constexpr auto Set = PhidgetCurrentInput_setDataInterval;
        static const constexpr auto Min = PhidgetCurrentInput_getMinDataInterval;
        static const constexpr auto Max = PhidgetCurrentInput_getMaxDataInterval;
    };

    phidget_current_input_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Double_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
    }
};


class phidget_voltage_input_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetVoltageInputHandle;
        static const constexpr auto Get = PhidgetVoltageInput_getVoltage;
        static const constexpr auto Change = PhidgetVoltageInput_setOnVoltageChangeHandler;
        static const constexpr auto Min = PhidgetVoltageInput_getMinVoltage;
        static const constexpr auto Max = PhidgetVoltageInput_getMaxVoltage;
    };

    struct rate_desc
    {
        using handle_type = PhidgetVoltageInputHandle;
        static const constexpr auto Get = PhidgetVoltageInput_getDataInterval;
        static const constexpr auto Set = PhidgetVoltageInput_setDataInterval;
        static const constexpr auto Min = PhidgetVoltageInput_getMinDataInterval;
        static const constexpr auto Max = PhidgetVoltageInput_getMaxDataInterval;
    };
    struct trigger_desc
    {
        using handle_type = PhidgetVoltageInputHandle;
        static const constexpr auto Get = PhidgetVoltageInput_getVoltageChangeTrigger;
        static const constexpr auto Set = PhidgetVoltageInput_setVoltageChangeTrigger;
        static const constexpr auto Min = PhidgetVoltageInput_getMinVoltageChangeTrigger;
        static const constexpr auto Max = PhidgetVoltageInput_getMaxVoltageChangeTrigger;
    };

    phidget_voltage_input_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Double_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_Double_parameter<trigger_desc>{}, "trigger", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
          m_children[3]->get_parameter()->push_value();
        }
      });
    }
};

class phidget_capacitive_touch_input_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetCapacitiveTouchHandle;
        static const constexpr auto Get = PhidgetCapacitiveTouch_getTouchValue;
        static const constexpr auto Change = PhidgetCapacitiveTouch_setOnTouchHandler;
        static const constexpr auto Min = PhidgetCapacitiveTouch_getMinTouchValue;
        static const constexpr auto Max = PhidgetCapacitiveTouch_getMaxTouchValue;
    };

    struct rate_desc
    {
        using handle_type = PhidgetCapacitiveTouchHandle;
        static const constexpr auto Get = PhidgetCapacitiveTouch_getDataInterval;
        static const constexpr auto Set = PhidgetCapacitiveTouch_setDataInterval;
        static const constexpr auto Min = PhidgetCapacitiveTouch_getMinDataInterval;
        static const constexpr auto Max = PhidgetCapacitiveTouch_getMaxDataInterval;
    };

    phidget_capacitive_touch_input_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Double_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
    }
};

class phidget_distance_sensor_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetDistanceSensorHandle;
        static const constexpr auto Get = PhidgetDistanceSensor_getDistance;
        static const constexpr auto Change = PhidgetDistanceSensor_setOnDistanceChangeHandler;
        static const constexpr auto Min = PhidgetDistanceSensor_getMinDistance;
        static const constexpr auto Max = PhidgetDistanceSensor_getMaxDistance;
    };

    struct rate_desc
    {
        using handle_type = PhidgetDistanceSensorHandle;
        static const constexpr auto Get = PhidgetDistanceSensor_getDataInterval;
        static const constexpr auto Set = PhidgetDistanceSensor_setDataInterval;
        static const constexpr auto Min = PhidgetDistanceSensor_getMinDataInterval;
        static const constexpr auto Max = PhidgetDistanceSensor_getMaxDataInterval;
    };

    phidget_distance_sensor_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<UInt_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
    }
};

class phidget_humidity_sensor_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetHumiditySensorHandle;
        static const constexpr auto Get = PhidgetHumiditySensor_getHumidity;
        static const constexpr auto Change = PhidgetHumiditySensor_setOnHumidityChangeHandler;
        static const constexpr auto Min = PhidgetHumiditySensor_getMinHumidity;
        static const constexpr auto Max = PhidgetHumiditySensor_getMaxHumidity;
    };

    struct rate_desc
    {
        using handle_type = PhidgetHumiditySensorHandle;
        static const constexpr auto Get = PhidgetHumiditySensor_getDataInterval;
        static const constexpr auto Set = PhidgetHumiditySensor_setDataInterval;
        static const constexpr auto Min = PhidgetHumiditySensor_getMinDataInterval;
        static const constexpr auto Max = PhidgetHumiditySensor_getMaxDataInterval;
    };

    phidget_humidity_sensor_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Double_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
    }
};


class phidget_pressure_sensor_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetPressureSensorHandle;
        static const constexpr auto Get = PhidgetPressureSensor_getPressure;
        static const constexpr auto Change = PhidgetPressureSensor_setOnPressureChangeHandler;
        static const constexpr auto Min = PhidgetPressureSensor_getMinPressure;
        static const constexpr auto Max = PhidgetPressureSensor_getMaxPressure;
    };

    struct rate_desc
    {
        using handle_type = PhidgetPressureSensorHandle;
        static const constexpr auto Get = PhidgetPressureSensor_getDataInterval;
        static const constexpr auto Set = PhidgetPressureSensor_setDataInterval;
        static const constexpr auto Min = PhidgetPressureSensor_getMinDataInterval;
        static const constexpr auto Max = PhidgetPressureSensor_getMaxDataInterval;
    };

    phidget_pressure_sensor_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Double_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
    }
};
class phidget_resistance_input_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetResistanceInputHandle;
        static const constexpr auto Get = PhidgetResistanceInput_getResistance;
        static const constexpr auto Change = PhidgetResistanceInput_setOnResistanceChangeHandler;
        static const constexpr auto Min = PhidgetResistanceInput_getMinResistance;
        static const constexpr auto Max = PhidgetResistanceInput_getMaxResistance;
    };

    struct rate_desc
    {
        using handle_type = PhidgetResistanceInputHandle;
        static const constexpr auto Get = PhidgetResistanceInput_getDataInterval;
        static const constexpr auto Set = PhidgetResistanceInput_setDataInterval;
        static const constexpr auto Min = PhidgetResistanceInput_getMinDataInterval;
        static const constexpr auto Max = PhidgetResistanceInput_getMaxDataInterval;
    };

    phidget_resistance_input_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Double_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
    }
};

class phidget_light_sensor_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetLightSensorHandle;
        static const constexpr auto Get = PhidgetLightSensor_getIlluminance;
        static const constexpr auto Change = PhidgetLightSensor_setOnIlluminanceChangeHandler;
        static const constexpr auto Min = PhidgetLightSensor_getMinIlluminance;
        static const constexpr auto Max = PhidgetLightSensor_getMaxIlluminance;
    };

    struct rate_desc
    {
        using handle_type = PhidgetLightSensorHandle;
        static const constexpr auto Get = PhidgetLightSensor_getDataInterval;
        static const constexpr auto Set = PhidgetLightSensor_setDataInterval;
        static const constexpr auto Min = PhidgetLightSensor_getMinDataInterval;
        static const constexpr auto Max = PhidgetLightSensor_getMaxDataInterval;
    };

    phidget_light_sensor_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Double_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
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
    }

    void init()
    {
      m_parameter = make_parameter<Parameter>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
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
    }

    void init()
    {
      m_parameter = make_parameter<Parameter>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
    }
};

class phidget_magnetometer_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetMagnetometerHandle;
        static const constexpr auto Get = PhidgetMagnetometer_getMagneticField;
        static const constexpr auto Change = PhidgetMagnetometer_setOnMagneticFieldChangeHandler;
        static const constexpr auto Min = PhidgetMagnetometer_getMinMagneticField;
        static const constexpr auto Max = PhidgetMagnetometer_getMaxMagneticField;
    };
    struct rate_desc
    {
        using handle_type = PhidgetMagnetometerHandle;
        static const constexpr auto Get = PhidgetMagnetometer_getDataInterval;
        static const constexpr auto Set = PhidgetMagnetometer_setDataInterval;
        static const constexpr auto Min = PhidgetMagnetometer_getMinDataInterval;
        static const constexpr auto Max = PhidgetMagnetometer_getMaxDataInterval;
    };

    phidget_magnetometer_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Vec3_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
    }
};

class phidget_accelerometer_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetAccelerometerHandle;
        static const constexpr auto Get = PhidgetAccelerometer_getAcceleration;
        static const constexpr auto Change = PhidgetAccelerometer_setOnAccelerationChangeHandler;
        static const constexpr auto Min = PhidgetAccelerometer_getMinAcceleration;
        static const constexpr auto Max = PhidgetAccelerometer_getMaxAcceleration;
    };

    struct rate_desc
    {
        using handle_type = PhidgetAccelerometerHandle;
        static const constexpr auto Get = PhidgetAccelerometer_getDataInterval;
        static const constexpr auto Set = PhidgetAccelerometer_setDataInterval;
        static const constexpr auto Min = PhidgetAccelerometer_getMinDataInterval;
        static const constexpr auto Max = PhidgetAccelerometer_getMaxDataInterval;
    };

    phidget_accelerometer_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Vec3_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);
      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
    }
};

class phidget_gyroscope_node : public ossia::phidget_node
{
  public:
    struct param_desc
    {
        using handle_type = PhidgetGyroscopeHandle;
        static const constexpr auto Get = PhidgetGyroscope_getAngularRate;
        static const constexpr auto Change = PhidgetGyroscope_setOnAngularRateUpdateHandler;
        static const constexpr auto Min = PhidgetGyroscope_getMinAngularRate;
        static const constexpr auto Max = PhidgetGyroscope_getMaxAngularRate;
    };

    struct rate_desc
    {
        using handle_type = PhidgetGyroscopeHandle;
        static const constexpr auto Get = PhidgetGyroscope_getDataInterval;
        static const constexpr auto Set = PhidgetGyroscope_setDataInterval;
        static const constexpr auto Min = PhidgetGyroscope_getMinDataInterval;
        static const constexpr auto Max = PhidgetGyroscope_getMaxDataInterval;
    };

    phidget_gyroscope_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }

    void init()
    {
      m_parameter = make_parameter<Vec3_parameter<param_desc>>(*this);
      m_device.on_parameter_created(*m_parameter);

      add_child_simple(std::make_unique< ossia::phidget_open_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique< ossia::phidget_channel_node>(m_hdl, m_device, *this));
      add_child_simple(std::make_unique<ossia::phidget_control_node>(Control_UInt_parameter<rate_desc>{}, "rate", m_hdl, m_device, *this));
      m_children[0]->get_parameter()->add_callback([=] (const ossia::value& val) {
        bool b = ossia::convert<bool>(val);
        if(b) {
          m_children[2]->get_parameter()->push_value();
        }
      });
    }
};


class phidget_hub_node : public ossia::phidget_node
{
  public:
    phidget_hub_node(PhidgetHandle hdl, ossia::net::device_base& dev, ossia::net::node_base& parent):
      phidget_node{hdl, dev, parent}
    {
    }
    void init()
    {

    }
};

template<typename T, typename... Args>
auto make(PhidgetHandle phid, ossia::net::device_base& dev, ossia::net::node_base& par_node)
{
  static std::vector<std::future<void>> vec;
  auto node = new T{phid, dev, par_node};
  par_node.add_child(std::unique_ptr<phidget_node>(node));

    /*
    Phidget_close(phid);
    for(int i = 0; i < 10; i++)
    {
      if(Phidget_openWaitForAttachment(phid, 100) == EPHIDGET_OK)
        break;
    }
    */

    node->init();
    //Phidget_close(phid);

  return node;
}
}
