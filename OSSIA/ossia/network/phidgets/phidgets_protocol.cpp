// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgets_protocol.hpp"
#include "phidgets_parameter.hpp"
#include "phidgets_node.hpp"
#include "phidgets_parameter_data.hpp"
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/value/value_traits.hpp>
#include <ossia/detail/logger.hpp>
namespace ossia
{
template<typename Fun>
ossia::net::node_base* find_tree(ossia::net::node_base& root, const Fun& f)
{
  if(f(root))
    return &root;
  for(auto& node : root.children_copy())
  {
    auto res = find_tree(node, f);
    if(res)
      return res;
  }
  return nullptr;
}

void phidget_protocol::on_deviceCreated(PhidgetHandle phid)
{
  phidget_handle_t hdl{phid};
  const auto sn = hdl.get_serial();

  if(m_phidgetMap.find(hdl) != m_phidgetMap.end())
    return;

  Phidget_DeviceClass dcls;
  Phidget_getDeviceClass(phid, &dcls);

  Phidget_ChannelClass cls;
  Phidget_getChannelClass(phid, &cls);

  auto par_node = get_parent(phid);
  ossia::phidget_node* phid_node{};

  switch(cls)
  {
    case PHIDCHCLASS_ACCELEROMETER:
      phid_node = make<phidget_accelerometer_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_GYROSCOPE:
      phid_node = make<phidget_gyroscope_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_VOLTAGEINPUT:
      phid_node = make<phidget_voltage_input_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_DIGITALINPUT:
      phid_node = make<phidget_digital_input_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_DIGITALOUTPUT:
      phid_node = make<phidget_digital_output_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_HUB:
      phid_node = make<phidget_hub_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_VOLTAGERATIOINPUT:
      phid_node = make<phidget_voltage_ratio_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_MAGNETOMETER:
      phid_node = make<phidget_magnetometer_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_CAPACITIVETOUCH:
      phid_node = make<phidget_capacitive_touch_input_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_DISTANCESENSOR:
      phid_node = make<phidget_distance_sensor_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_HUMIDITYSENSOR:
      phid_node = make<phidget_humidity_sensor_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_PRESSURESENSOR:
      phid_node = make<phidget_pressure_sensor_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_RESISTANCEINPUT:
      phid_node = make<phidget_resistance_input_node>(phid, *m_dev, *par_node);
      break;
    case PHIDCHCLASS_LIGHTSENSOR:
      phid_node = make<phidget_light_sensor_node>(phid, *m_dev, *par_node);
      break;
    default:
      break;
  }

  if(!phid_node)
  {
    Phidget_release(&phid);
    return;
  }

  Phidget_setOnErrorHandler(
        phid,
        [] (PhidgetHandle phid, void *ctx, Phidget_ErrorEventCode errorCode, const char *errorString)
  {
    auto phid_node = static_cast<ossia::phidget_node*>(ctx);
    ossia::logger().error("[Phidget] {} ({}): {}", phid_node->osc_address(), phidget_handle_t{phid}.get_serial(), errorString);
  }, phid_node);


  phid_node->about_to_be_deleted.connect<phidget_protocol, &phidget_protocol::deleting_node>(*this);

  m_phidgetMap.insert({hdl, phid_node});
}

void phidget_protocol::remove_parent_rec(ossia::net::node_base* par)
{
  if(par && par->children().empty())
  {
    if(auto grandpa = par->get_parent())
    {
      remove_node(par);
      remove_parent_rec(grandpa);
    }
  }
}
bool is_child_of(ossia::net::node_base* child, const ossia::net::node_base* par)
{
  while(auto cur = child->get_parent())
  {
    if(cur == par)
      return true;
    child = cur;
  }
  return false;
}
void phidget_protocol::remove_node(ossia::net::node_base* node)
{
  if(auto par = node->get_parent())
  {
    /*
    for(auto it = m_phidgetMap.begin(); it != m_phidgetMap.end(); )
    {
      if(is_child_of(it->second, node))
      {
        it = m_phidgetMap.erase(it);
      }
      else
      {
        ++it;
      }
    }*/
    par->remove_child(*node);
  }
}

void phidget_protocol::deleting_node(const net::node_base& par)
{
  for(auto it = m_phidgetMap.begin(); it != m_phidgetMap.end(); )
  {
    if(it->second == &par)
    {
      it = m_phidgetMap.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void phidget_protocol::on_deviceRemoved(ossia::phidget_id phid)
{
  ossia::net::node_base* to_remove{};
  for(auto it = m_phidgetMap.begin(); it != m_phidgetMap.end(); )
  {
    auto pn = dynamic_cast<phidget_node*>(it->second);
    if(pn && pn->id == phid)
    {
        to_remove = it->second;
        it = m_phidgetMap.erase(it);
        break;
    }
    else
    {
      ++it;
    }
  }

  if(to_remove)
  {
    auto par = to_remove->get_parent();
    remove_node(to_remove);
    remove_parent_rec(par);
  }
}

phidget_protocol::phidget_protocol()
{
  PhidgetLog_enable(PHIDGET_LOG_WARNING, nullptr);

  PhidgetNet_enableServerDiscovery(PHIDGETSERVER_SBC);
  PhidgetNet_enableServerDiscovery(PHIDGETSERVER_DEVICE);
  PhidgetNet_enableServerDiscovery(PHIDGETSERVER_DEVICELISTENER);
  PhidgetNet_enableServerDiscovery(PHIDGETSERVER_DEVICEREMOTE);
  // Tree :
  // Phidgets:/device/...

  // For each device, create the relevant keys.
  PhidgetManager_create(&m_hdl);
  PhidgetManager_setOnAttachHandler(
        m_hdl,
        [] (PhidgetManagerHandle phidm, void *ptr, PhidgetHandle phid) {
    auto self = (phidget_protocol*)ptr;

    Phidget_retain(phid);
    self->m_functionQueue.enqueue([phid,self] { self->on_deviceCreated(phid); });

    if (self->m_commandCb)
      self->m_commandCb();
  },
  this);

  PhidgetManager_setOnDetachHandler(
        m_hdl,
        [](PhidgetManagerHandle phidm, void *ptr, PhidgetHandle phid) {

    phidget_handle_t h{phid};

    auto self = (phidget_protocol*)ptr;
    self->m_functionQueue.enqueue([phid=phidget_id{h},self] { self->on_deviceRemoved(phid); });

    if (self->m_commandCb)
      self->m_commandCb();
  },
  this);

  PhidgetManager_open(m_hdl);

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

void phidget_protocol::run_command()
{
  std::function<void()> cmd;

  if(m_functionQueue.try_dequeue(cmd))
    if (cmd)
      cmd();
}

bool phidget_protocol::push_raw(const net::full_parameter_data &)
{
  return false;
}

auto debug_handle(phidget_handle_t phid, int h)
{
  // Phidget_openWaitForAttachment(phid, 0);
  std::cerr << std::endl;
  auto debug_param = [&] (auto param, const char* name)
  {
    int x = 0;
    auto hub_err = param(phid, &x);
    for(int i = 0; i < 2 * h; i++) std::cerr << ' ';
    std::cerr << name << " : " << x;
    if(hub_err != EPHIDGET_OK)
      std::cerr << " - err: " << hub_err;
    std::cerr << std::endl;
    return x;
  };

  {
  }
#define PHIDGET_DEBUG(p) debug_param(p, #p)

  int chan;
  Phidget_getIsChannel(phid, &chan);
  if(chan)
  {
    for(int i = 0; i < 2 * h; i++) std::cerr << ' ';
    std::cerr << (int64_t) phid.phid << " => "
              << phid.get_device_classname() << " "
              << phid.get_channel_classname()  << " "
              << phid.get_channel_subclassname()  << std::endl;

    PHIDGET_DEBUG(Phidget_getChannel);
  }
  else
  {
    for(int i = 0; i < 2 * h; i++) std::cerr << ' ';
    Phidget_DeviceClass dc;
    Phidget_getDeviceClass(phid, &dc);
    std::cerr << (int64_t) phid.phid << " => "
              << phid.get_device_classname()  << std::endl;
  }

  {
    PhidgetHandle hub;
    if(Phidget_getHub(phid, &hub) == EPHIDGET_OK)
    {
      for(int i = 0; i < 2 * h; i++) std::cerr << ' ';
      std::cerr << "Phidget_getHub : " << (int64_t)hub << std::endl;
    }
  }
  {
    Phidget_DeviceID hub;
    if(Phidget_getDeviceID(phid, &hub) == EPHIDGET_OK)
    {
      for(int i = 0; i < 2 * h; i++) std::cerr << ' ';
      std::cerr << "Phidget_getDeviceID : " << phid.get_device_id_name() << std::endl;
    }
  }
  PHIDGET_DEBUG(Phidget_getAttached);
  PHIDGET_DEBUG(Phidget_getIsHubPortDevice);
  PHIDGET_DEBUG(Phidget_getHubPort);
  PHIDGET_DEBUG(Phidget_getHubPortCount);
  // Phidget_close(phid);
}

ossia::net::node_base* phidget_protocol::get_parent(phidget_handle_t phid)
{
  int k = 0;
  debug_handle(phid, k++);
  PhidgetHandle parent{};

  std::vector<PhidgetHandle> parents;
  auto err = Phidget_getParent(phid, &parent);
  while(err == EPHIDGET_OK && parent)
  {
    debug_handle(parent, k++);
    parents.push_back(parent);
    err = Phidget_getParent(parent, &parent);
  }
  ossia::net::node_base* par_node = &m_dev->get_root_node();

  std::vector<ossia::net::node_base*> current_ports;
  for(auto it = parents.rbegin(); it != parents.rend(); ++it)
  {
    phidget_handle_t hdl = *it;
    auto phid_it = m_phidgetMap.find(hdl);
    if(phid_it != m_phidgetMap.end())
    {
      par_node = phid_it->second;
      current_ports.clear();
      if(hdl.get_device_class() == PHIDCLASS_HUB)
      {
        for(auto n : par_node->children_copy())
        {
          current_ports.push_back(n);
        }
      }
      continue;
    }
    if(hdl.get_device_class() == PHIDCLASS_HUB)
    {
      auto node = new ossia::phidget_node{hdl, *m_dev, *par_node};
      m_phidgetMap.insert({hdl, node});
      par_node->add_child(std::unique_ptr<ossia::net::node_base>(node));
      par_node = node;

      current_ports.clear();
      for(int i = 0; i < hdl.get_hub_port_count(); i++)
      {
        auto port = new ossia::phidget_hub_port_node{hdl, i, *m_dev, *par_node};
        current_ports.push_back(port);
        par_node->add_child(std::unique_ptr<ossia::net::node_base>(port));
      }
    }
    else if(hdl.get_device_class() == PHIDCLASS_VINT
            && hdl.get_parent().get_device_class() == PHIDCLASS_HUB
            && (hdl.get_device_id() == PHIDID_VOLTAGEINPUT_PORT
             || hdl.get_device_id() == PHIDID_VOLTAGERATIOINPUT_PORT
             || hdl.get_device_id() == PHIDID_DIGITALINPUT_PORT
             || hdl.get_device_id() == PHIDID_DIGITALOUTPUT_PORT)
            )
    {
      if(!current_ports.empty())
      {
        auto cur_idx = hdl.get_hub_port();
        assert(cur_idx >= 0);
        assert(cur_idx < (int)current_ports.size());
        par_node = current_ports[cur_idx];
        current_ports.clear();
      }
    }
    else
    {
      if(!current_ports.empty())
      {
        auto cur_idx = hdl.get_hub_port();
        assert(cur_idx >= 0);
        assert(cur_idx < (int)current_ports.size());
        par_node = current_ports[cur_idx];
        current_ports.clear();
      }

      auto node = new ossia::phidget_node{hdl, *m_dev, *par_node};
      m_phidgetMap.insert({hdl, node});
      par_node->add_child(std::unique_ptr<ossia::net::node_base>(node));
      par_node = node;
    }

  }
  return par_node;
}
phidget_id::phidget_id(ossia::phidget_handle_t hdl)
{
  serialNumber = hdl.get_serial();
  hubPort = hdl.get_hub_port();
  channel = hdl.get_channel();
  isHubPort = hdl.is_hub_port_device();
}

}

