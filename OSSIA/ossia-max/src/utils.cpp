#include "utils.hpp"
#include "device.hpp"
#include "client.hpp"

namespace ossia
{
namespace max
{

bool find_peers(t_object_base* x)
{
    t_object *patcher, *box, *obj;
    object_obex_lookup(x, gensym("#P"), &patcher);
    for (box = jpatcher_get_firstobject(patcher); box; box =
         jbox_get_nextobject(box)) {
        obj = jbox_get_object(box);
        if (obj
            && object_classname(obj) == object_classname(x)
            && (t_object_base*)obj != x)
            return true;
    }
}

ossia::net::node_base* find_global_node(const std::string& addr)
{
  auto& instance = ossia_max::instance();
  for (auto device : instance.devices.copy())
  {
    auto dev = device->m_device;
    std::string name = dev->get_name();
    size_t pos = addr.find(name);
    if (pos != std::string::npos)
    {
      std::string osc_name = addr.substr(name.length());
      auto node = ossia::net::find_node(dev->get_root_node(),osc_name);
      if (node) return node;
    }
  }

  for (auto client : instance.clients.copy())
  {
    auto dev = client->m_device;
    std::string name = dev->get_name();
    size_t pos = addr.find(name);
    if (pos != std::string::npos)
    {
      std::string osc_name = addr.substr(name.length());
      auto node = ossia::net::find_node(dev->get_root_node(),osc_name);
      if (node) return node;
    }
  }
  return nullptr;
}

} // namespace max
} // namespace ossia
