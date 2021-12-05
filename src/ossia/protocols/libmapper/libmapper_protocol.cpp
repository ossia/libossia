#include "libmapper_protocol.hpp"
#include <ossia/network/common/device_parameter.hpp>
#include <ossia/network/common/node_visitor.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <thread>
#include <iostream>
#include <mapper/mapper_cpp.h>
namespace ossia::net
{


template<typename T>
constexpr std::nullptr_t libmapper_type{};
template<>
constexpr char libmapper_type<int> = 'i';
template<>
constexpr char libmapper_type<float> = 'f';

struct libmapper_send
{
  mpr_signal signal{};
  mpr_time timetag{};

  void operator()(impulse v) const noexcept { }
  void operator()(int v) const noexcept { mpr_sig_set_value(signal, 0, 1, MPR_INT32, &v); }
  void operator()(float v) const noexcept { mpr_sig_set_value(signal, 0, 1, MPR_FLT, &v); }
  void operator()(bool v) const noexcept { mpr_sig_set_value(signal, 0, 1, MPR_BOOL, &v); }
  void operator()(char v) const noexcept { (*this)(int(v)); }
  void operator()(vec2f v) const noexcept { mpr_sig_set_value(signal, 0, 2, MPR_FLT, v.data()); }
  void operator()(vec3f v) const noexcept { mpr_sig_set_value(signal, 0, 3, MPR_FLT, v.data()); }
  void operator()(vec4f v) const noexcept { mpr_sig_set_value(signal, 0, 4, MPR_FLT, v.data()); }
  void operator()(const std::string& v) const noexcept { (*this)(std::stof(v)); }
  void operator()(const std::vector<ossia::value>& v) const noexcept {
    //float* sub = (float*)alloca(sizeof(float) * v.size());
    //for(auto i = 0u; i < v.size(); i++)
    //  sub[i] = ossia::convert<float>(v[i]);
    //mapper_signal_update(signal, sub, v.size(), timetag);
  }
  void operator()() const noexcept { }
};

template<typename T>
struct libmapper_receive
{
  ossia::net::parameter_base& param;
  const T* value;
  int count;

  void operator()(impulse v) const { param.set_value(ossia::impulse{}); }
  void operator()(int v) const { param.set_value((int)value[0]); }
  void operator()(float v) const { param.set_value((float)value[0]); }
  void operator()(bool v) const { param.set_value((bool)value[0]); }
  void operator()(char v) const { param.set_value((char)value[0]); }
  void operator()(vec2f v) const { if(count >= 2) param.set_value(vec2f{(float)value[0], (float)value[1]}); }
  void operator()(vec3f v) const { if(count >= 3) param.set_value(vec3f{(float)value[0], (float)value[1], (float)value[2]}); }
  void operator()(vec4f v) const { if(count >= 4) param.set_value(vec4f{(float)value[0], (float)value[1], (float)value[2], (float)value[3]}); }
  void operator()(const std::string& v) const { param.set_value(std::to_string(value[0])); }
  void operator()(const std::vector<ossia::value>& v) const { param.set_value(std::vector<ossia::value>{value[0]}); }
  void operator()() const noexcept { }
};

struct libmapper_create_param
{
  libmapper_server_protocol& proto;
  const ossia::net::parameter_base& param;
  void operator()(impulse v) const noexcept { }
  void operator()(int v) const noexcept { proto.create_1d_parameter<int, int>(param); }
  void operator()(float v) const noexcept { proto.create_1d_parameter<float, float>(param); }
  void operator()(bool v) const noexcept { proto.create_1d_parameter<bool, int>(param); }
  void operator()(char v) const noexcept { proto.create_1d_parameter<char, int>(param); }
  void operator()(vec2f v) const noexcept { proto.create_array_parameter<2>(param); }
  void operator()(vec3f v) const noexcept { proto.create_array_parameter<3>(param); }
  void operator()(vec4f v) const noexcept { proto.create_array_parameter<4>(param); }
  void operator()(const std::string& v) const noexcept { }
  void operator()(const std::vector<ossia::value>& v) const noexcept {
    proto.create_vector_parameter(param);
  }
  void operator()() const noexcept { }
};

struct libmapper_apply_control
{
  libmapper_server_protocol& protocol;
  mpr_device dev;
  void operator()(libmapper_server_protocol::CreateSignal cmd) const noexcept
  {
    cmd.param->value().apply(libmapper_create_param{protocol, *cmd.param});
  }

  void operator()(libmapper_server_protocol::RemoveSignal cmd) const noexcept
  {
    //mapper_device_remove_signal(dev, cmd.sig);
  }

};

/*
template<typename Type>
static void on_libmapper_input(mapper_signal sig, mapper_id instance, const void* value, int count, mapper_timetag_t* tt)
{
  ossia::net::parameter_base* param = (ossia::net::parameter_base*) value;
  const void* arr = mapper_signal_instance_value(sig, instance, tt);
  if(arr)
  {
    ossia::apply(libmapper_receive<Type>{*param, (Type*) arr, count}, param->value());
  }
}
*/


libmapper_server_protocol::libmapper_server_protocol()
{

}

libmapper_server_protocol::~libmapper_server_protocol()
{
  stop();
}

void libmapper_server_protocol::set_device(ossia::net::device_base& dev)
{
  if (m_device)
  {
    auto& old = *m_device;
    old.on_node_created
        .disconnect<&libmapper_server_protocol::on_nodeCreated>(this);
    old.on_node_removing
        .disconnect<&libmapper_server_protocol::on_nodeRemoved>(this);
    dev.on_parameter_created
        .disconnect<&libmapper_server_protocol::on_parameterCreated>(this);
    dev.on_parameter_removing
        .disconnect<&libmapper_server_protocol::on_parameterRemoved>(this);
    old.on_attribute_modified
        .disconnect<&libmapper_server_protocol::on_attributeChanged>(this);
    old.on_node_renamed
        .disconnect<&libmapper_server_protocol::on_nodeRenamed>(this);

    ossia::net::visit_parameters(
          old.get_root_node(),
          [&](ossia::net::node_base& n, ossia::net::parameter_base& p) {
      if (p.callback_count() > 0)
        observe(p, false);
    });

    m_running = false;
    m_thread.join();
    mpr_dev_free(m_mapper_dev);
    m_mapper_dev = nullptr;
  }

  m_device = &dev;

  dev.on_node_created
      .connect<&libmapper_server_protocol::on_nodeCreated>(this);
  dev.on_node_removing
      .connect<&libmapper_server_protocol::on_nodeRemoved>(this);
  dev.on_parameter_created
      .connect<&libmapper_server_protocol::on_parameterCreated>(this);
  dev.on_parameter_removing
      .connect<&libmapper_server_protocol::on_parameterRemoved>(this);
  dev.on_attribute_modified
      .connect<&libmapper_server_protocol::on_attributeChanged>(this);
  dev.on_node_renamed
      .connect<&libmapper_server_protocol::on_nodeRenamed>(this);

  m_thread = std::thread{[this] { execThread(); }};

  while(!m_running) std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

bool libmapper_server_protocol::pull(net::parameter_base& param)
{
  return true;
}

bool libmapper_server_protocol::push(const net::parameter_base& param, const ossia::value& v)
{
  auto it = m_outputMap.find(&param);
  if(it != m_outputMap.end())
  {
    m_sendQueue.enqueue({it->second, v});
  }
  return true;
}

bool libmapper_server_protocol::observe(net::parameter_base& param, bool enable)
{
  return false;
}

bool libmapper_server_protocol::push_raw(const ossia::net::full_parameter_data& data)
{
  return false;
}

bool libmapper_server_protocol::update(ossia::net::node_base&)
{
  return true;
}

void libmapper_server_protocol::stop()
{
}

void libmapper_server_protocol::on_nodeCreated(const node_base& n)
{
}

void libmapper_server_protocol::on_nodeRemoved(const node_base&)
{

}

template<typename OssiaType, typename LibmapperType>
void libmapper_server_protocol::create_1d_parameter(const parameter_base& p)
{
  auto& n = p.get_node();
  LibmapperType min = p.get_domain().convert_min<OssiaType>();
  LibmapperType max = p.get_domain().convert_max<OssiaType>();

  const auto unit = p.get_unit();
  const auto unit_text = ossia::get_unit_text(unit);

  constexpr char t = libmapper_type<LibmapperType>;
  switch(p.get_access())
  {
    case ossia::access_mode::BI:
    {
      std::string addr = n.osc_address();

      std::string in_name = addr + ".in";
      std::string out_name = addr + ".out";
      mpr_sig_handler* in_hdl{};
      mpr_sig_handler* out_hdl{};
      auto in_sig = mpr_sig_new(m_mapper_dev, mpr_dir::MPR_DIR_IN, in_name.c_str(), 1, MPR_FLT, "", nullptr, nullptr, nullptr, in_hdl, 0);
      auto out_sig = mpr_sig_new(m_mapper_dev, mpr_dir::MPR_DIR_OUT, out_name.c_str(), 1, MPR_FLT, "", nullptr, nullptr, nullptr, out_hdl, 0);
      ///*auto i = */m_inputMap[&p] = m_mapper_dev->add_input_signal(addr + ".in", 1, t, unit_text.data(), &min, &max, on_libmapper_input<LibmapperType>, (void*) &p);
      //auto o = m_outputMap[&p] = m_mapper_dev->add_output_signal(addr + ".out", 1, t, unit_text.data(), &min, &max);

      p.value().apply(libmapper_send{out_sig});

      break;
    }

    case ossia::access_mode::GET:
    {
      // const auto& addr = n.osc_address();
      //
      // /* auto i = */m_inputMap[&p] = m_mapper_dev->add_signal(mapper::Direction::INCOMING, addr, 1, t, unit_text.data(), &min, &max, on_libmapper_input<LibmapperType>, (void*) &p);
      break;
    }

    case ossia::access_mode::SET:
    {
      // const auto& addr = n.osc_address();
      //
      // auto o = m_outputMap[&p] = m_mapper_dev->add_output_signal(addr, 1, t, unit_text.data(), &min, &max);
      // p.value().apply(libmapper_send{o});
      break;
    }
  }
}

template<std::size_t N>
void libmapper_server_protocol::create_array_parameter(const parameter_base& p)
{
  /*
  auto& n = p.get_node();
  auto min = p.get_domain().convert_min<std::array<float, N>>();
  auto max = p.get_domain().convert_max<std::array<float, N>>();

  const auto unit = p.get_unit();
  const auto unit_text = ossia::get_unit_text(unit);

  constexpr char t = 'f';
  switch(p.get_access())
  {
    case ossia::access_mode::BI:
    {
      std::string addr = n.osc_address();

      auto i = m_inputMap[&p] = m_mapper_dev->add_input_signal(addr + ".in", 1, t, unit_text.data(), min.data(), max.data(), on_libmapper_input<float>, (void*) &p);
      auto o = m_outputMap[&p] = m_mapper_dev->add_output_signal(addr + ".out", 1, t, unit_text.data(), min.data(), max.data());
      p.value().apply(libmapper_send{o});

      break;
    }

    case ossia::access_mode::GET:
    {
      const auto& addr = n.osc_address();

      auto i = m_inputMap[&p] = m_mapper_dev->add_input_signal(addr, 1, t, unit_text.data(), min.data(), max.data(), on_libmapper_input<float>, (void*) &p);
      break;
    }

    case ossia::access_mode::SET:
    {
      const auto& addr = n.osc_address();

      auto o = m_outputMap[&p] = m_mapper_dev->add_output_signal(addr, 1, t, unit_text.data(),  min.data(), max.data());
      p.value().apply(libmapper_send{o});
      break;
    }
  }
  */
}

void libmapper_server_protocol::create_vector_parameter(const parameter_base& p)
{
  /*
  // TODO handle various domain cases... ints, floats...
  auto& n = p.get_node();
  auto min = p.get_domain().convert_min<float>();
  auto max = p.get_domain().convert_max<float>();

  const auto unit = p.get_unit();
  const auto unit_text = ossia::get_unit_text(unit);

  constexpr char t = 'f';
  switch(p.get_access())
  {
    case ossia::access_mode::BI:
    {
      std::string addr = n.osc_address();

      auto i = m_inputMap[&p] = m_mapper_dev->add_input_signal(addr + ".in", 1, t, unit_text.data(), &min, &max, on_libmapper_input<float>, (void*) &p);
      auto o = m_outputMap[&p] = m_mapper_dev->add_output_signal(addr + ".out", 1, t, unit_text.data(), &min, &max);
      p.value().apply(libmapper_send{o});

      break;
    }

    case ossia::access_mode::GET:
    {
      const auto& addr = n.osc_address();

      auto i = m_inputMap[&p] = m_mapper_dev->add_input_signal(addr, 1, t, unit_text.data(), &min, &max, on_libmapper_input<float>, (void*) &p);
      break;
    }

    case ossia::access_mode::SET:
    {
      const auto& addr = n.osc_address();

      auto o = m_outputMap[&p] = m_mapper_dev->add_output_signal(addr, 1, t, unit_text.data(),  &min, &max);
      p.value().apply(libmapper_send{o});
      break;
    }
  }
  */
}


void libmapper_server_protocol::on_parameterCreated(const parameter_base& p)
{
  while(m_editSemaphore > 0) std::this_thread::sleep_for(std::chrono::milliseconds(1));

  m_editSemaphore++;
  m_ctlQueue.enqueue(CreateSignal{&p});
}

void libmapper_server_protocol::on_parameterRemoved(const parameter_base& p)
{
  while(m_editSemaphore > 0) std::this_thread::sleep_for(std::chrono::milliseconds(1));

  {
    auto it = m_inputMap.find(&p);
    if(it != m_inputMap.end())
    {
      m_editSemaphore++;
      m_ctlQueue.enqueue(RemoveSignal{it->second});
    }
  }
  {
    auto it = m_outputMap.find(&p);
    if(it != m_outputMap.end())
    {
      m_editSemaphore++;
      m_ctlQueue.enqueue(RemoveSignal{it->second});
    }
  }
}

void libmapper_server_protocol::on_attributeChanged(const node_base&, string_view attr)
{

}

void libmapper_server_protocol::on_nodeRenamed(const node_base& n, std::string oldname)
{

}

void libmapper_server_protocol::execThread()
{
  m_mapper_dev = mpr_dev_new(m_device->get_name().c_str(), nullptr);

  ossia::net::visit_parameters(
        m_device->get_root_node(),
        [&](ossia::net::node_base& n, ossia::net::parameter_base& p) {
    if (p.callback_count() > 0)
      observe(p, true);

    on_parameterCreated(p);
  });

  m_running = true;

  while(m_running)
  {
    Message m;
    while(m_sendQueue.try_dequeue(m))
      m.value.apply(libmapper_send{m.sig});

    mpr_dev_poll(m_mapper_dev, 10);

    ControlMessage ctl;
    while(m_ctlQueue.try_dequeue(ctl))
    {
      std::visit(libmapper_apply_control{*this, m_mapper_dev}, ctl);
      m_editSemaphore--;
    }

    mpr_dev_poll(m_mapper_dev, 10);
  }

}


libmapper_client_protocol::libmapper_client_protocol()
{
  const auto MPR_OBJ_ALL = MPR_OBJ_NEW | MPR_OBJ_MOD | MPR_OBJ_REM | MPR_OBJ_EXP;
  m_db = mpr_graph_new(MPR_OBJ_ALL);
  // void mpr_graph_handler(mpr_graph graph, mpr_obj object, const mpr_graph_evt event, const void *data)
  mpr_graph_add_cb(
        m_db,
        [] (mpr_graph db,
        mpr_obj dev,
        const mpr_graph_evt event,
        const void *user){
    std::cerr << "device callback!\n";
    switch(event)
    {
      case MPR_OBJ_NEW: std::cerr << "New \n"; break;
      case MPR_OBJ_MOD: std::cerr << "Mod \n"; break;
      case MPR_OBJ_REM: std::cerr << "Rem \n"; break;
      case MPR_OBJ_EXP: std::cerr << "Exp \n"; break;
    }

    auto self = (libmapper_client_protocol*) user;

    int N = mpr_obj_get_num_props(dev, 0);
    for (int i = 0; i < N; i++)
    {
      const char* key;
      int len;
      mpr_type type;
      const void* value;
      int publish{};
      mpr_obj_get_prop_by_idx(dev , i, &key, &len, &type, &value, &publish);

      std::cerr<< " Prop: " << key << " => " << int(type) << std::endl;
      switch(type)
      {
        case MPR_STR:

          std::cerr<< "  ==> " << (const char*)(value) << std::endl;
          break;
      }
    }
    self->update();
  }, MPR_OBJ_ALL, this);
  // m_db->subscribe(MAPPER_OBJ_ALL);
  // m_db->request_devices();
}

libmapper_client_protocol::~libmapper_client_protocol()
{
  stop();
}
void libmapper_client_protocol::poll(int v)
{
  mpr_graph_poll(m_db, v);
}

void libmapper_client_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;

  m_mapper_dev = mpr_dev_new(m_device->get_name().c_str(), 0);
}

bool libmapper_client_protocol::pull(net::parameter_base& param)
{
  return true;
}

bool libmapper_client_protocol::push(const net::parameter_base& param, const ossia::value& v)
{
  auto it = m_outputMap.find(&param);
  if(it != m_outputMap.end())
  {
    auto sig = it->second;
    float va = ossia::convert<float>(v);
    mpr_sig_set_value(sig, 0, 1, MPR_FLT, &va);
  }
  return true;
}

bool libmapper_client_protocol::observe(net::parameter_base& param, bool enable)
{
  return false;
}

bool libmapper_client_protocol::push_raw(const ossia::net::full_parameter_data& data)
{
  return false;
}

bool libmapper_client_protocol::update(ossia::net::node_base&)
{
  return update();
}
bool libmapper_client_protocol::update()
{
  std::cerr << "ok\n";
  mpr_graph_poll(m_db, 100);
  auto list = mpr_graph_get_list(m_db, MPR_DEV);
  mpr_graph_poll(m_db, 100);
  int n = mpr_list_get_size(list);
  for(int i = 0; i < n; i++)
  {
    mpr_dev device = mpr_list_get_idx(list, i);

    int len{};
    mpr_type type{};
    const void* value{};
    int publish{};
    mpr_obj_get_prop_by_key(device, "name", &len, &type, &value, &publish);

    //std::cerr << mapper_device_host(dev) << ":" << dev.port() << std::endl;
    // switch(prop.type)
    // {
    //   case 's': std::cerr << (const char*)prop.value  << std::endl;
    // }
    //

    // What we could do is,
    // create a hidden mirror of each remote signal
    // and push to our local signal, which the libmapper algoritmh then
    // will handle
    // Another option is to create an osc device but that will only work
    // for sending, not receiving
    // for(const mapper::Signal& node : dev.signals())
    // {
    //   std::cerr << " --> " << node.name() << std::endl;
    //
    //   mapper_signal_update_float(node, (float)rand() / 1000.);
    //   mapper_signal_push(node);
    // }
   }
  return true;
}

void libmapper_client_protocol::stop()
{
}

}
