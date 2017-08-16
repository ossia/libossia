// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_utils.hpp"

extern "C" {

void ossia_node_remove_child(ossia_node_t node, ossia_node_t child)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_remove_child: node is null");
      return;
    }
    if (!child)
    {
      ossia_log_error("ossia_node_remove_child: child is null");
      return;
    }

    auto n = convert_node(node);
    auto c = convert_node(child);
    n->remove_child(c->get_name());
  });
}

const char* ossia_node_get_name(ossia_node_t node)
{
  return safe_function(__func__, [=]() -> const char* {
    if (!node)
    {
      ossia_log_error("ossia_node_get_name: node is null");
      return nullptr;
    }

    auto n = convert_node(node);

    return strdup(n->get_name().c_str());
  });
}

ossia_device_t ossia_node_get_device(
        ossia_node_t node)
{
    return safe_function(__func__, [=]() -> ossia_device_t {
      if (!node)
      {
        ossia_log_error("ossia_node_get_name: node is null");
        return nullptr;
      }

      auto n = convert_node(node);
      auto& devs = static_devices();
      std::lock_guard<std::mutex> lock(devs.mutex);
      auto it = devs.devices.find(n->get_device().get_name());
      if(it != devs.devices.end())
          return it->second;
      return nullptr;
    });
}

ossia_node_t ossia_node_find(
    ossia_node_t node,
    const char* name)
{
  return safe_function(__func__, [=]() -> ossia_node_t {
    if (!node)
    {
      ossia_log_error("ossia_node_add_child: node is null");
      return nullptr;
    }

    auto n = convert_node(node);
    return convert(ossia::net::find_node(*n, name));
  });
}

void ossia_node_find_pattern(
        ossia_node_t node,
        const char* pattern,
        ossia_node_t* data,
        size_t* size)
{
    return safe_function(__func__, [=] {
      if (!node)
      {
        ossia_log_error("ossia_node_add_child: node is null");
        return;
      }

      auto n = convert_node(node);
      auto nodes = ossia::net::find_nodes(*n, pattern);
      if(nodes.size() == 0)
      {
        *data = nullptr;
        *size = 0;
      }
      else
      {
        *size = nodes.size();
        *data = (ossia_node_t*)std::malloc(sizeof(ossia_node_t) * nodes.size());
        for(std::size_t i = 0; i < nodes.size(); i++)
          data[i] = convert(nodes[i]);
      }
  });
}

void ossia_node_array_free(
    ossia_node_t* data)
{
    free(*data);
    *data = nullptr;
}

ossia_node_t ossia_node_create(
    ossia_node_t node,
    const char* name)
{
  return safe_function(__func__, [=]() -> ossia_node_t {
    if (!node)
    {
      ossia_log_error("ossia_node_add_child: node is null");
      return nullptr;
    }

    auto n = convert_node(node);
    return convert(&ossia::net::create_node(*n, name));
  });
}

void ossia_node_create_pattern(
        ossia_node_t node,
        const char* pattern,
        ossia_node_t* data,
        size_t* size)
{
    return safe_function(__func__, [=] {
      if (!node)
      {
        ossia_log_error("ossia_node_add_child: node is null");
        return;
      }

      auto n = convert_node(node);
      auto nodes = ossia::net::create_nodes(*n, pattern);
      if(nodes.size() == 0)
      {
        *data = nullptr;
        *size = 0;
      }
      else
      {
        *size = nodes.size();
        *data = new ossia_node_t[nodes.size()];
        for(std::size_t i = 0; i < nodes.size(); i++)
          data[i] = convert(nodes[i]);
      }
  });
}
ossia_node_t ossia_node_add_child(ossia_node_t node, const char* name)
{
  return safe_function(__func__, [=]() -> ossia_node_t {
    if (!node)
    {
      ossia_log_error("ossia_node_add_child: node is null");
      return nullptr;
    }

    auto n = convert_node(node);
    return convert(n->create_child(name));
  });
}

int ossia_node_child_size(ossia_node_t node)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_child_size: node is null");
      return std::size_t(0);
    }

    auto n = convert_node(node);
    return n->children().size();
  });
}

ossia_node_t ossia_node_get_child(ossia_node_t node, int child_n)
{
  return safe_function(__func__, [=]() -> ossia_node_t {
    if (!node)
    {
      ossia_log_error("ossia_node_get_child: node is null");
      return nullptr;
    }

    auto n = convert_node(node);
    if (n->children().size() < (std::size_t)child_n)
      return nullptr;

    return convert(n->children()[child_n].get());
  });
}

ossia_node_t ossia_node_find_child(ossia_node_t node, const char* child_n)
{
  return safe_function(__func__, [=]() -> ossia_node_t {
    if (!node)
    {
      ossia_log_error("ossia_node_find_child: node is null");
      return nullptr;
    }
    if (!child_n)
    {
      ossia_log_error("ossia_node_find_child: child_n is null");
      return nullptr;
    }

    auto n = convert_node(node);
    return convert(n->find_child(child_n));
  });
}

ossia_address_t ossia_node_create_address(ossia_node_t node, ossia_type type)
{
  return safe_function(__func__, [=]() -> ossia_address_t {
    if (!node)
    {
      ossia_log_error("ossia_node_create_address: node is null");
      return nullptr;
    }

    auto n = convert_node(node);
    return convert(n->create_address(convert(type)));
  });
}

ossia_address_t ossia_node_get_address(ossia_node_t node)
{
  return safe_function(__func__, [=]() -> ossia_address_t {
    if (!node)
    {
      ossia_log_error("ossia_node_create_address: node is null");
      return nullptr;
    }

    auto n = convert_node(node);
    return convert(n->get_address());
  });
}

void ossia_node_remove_address(ossia_node_t node)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_remove_address: node is null");
      return;
    }

    auto n = convert_node(node);
    n->remove_address();
  });
}


ossia_node_callback_idx_t ossia_node_add_deleting_callback(
        ossia_node_t node,
        ossia_node_callback_t callback,
        void* ctx)
{
  return safe_function(__func__, [=]() -> ossia_node_callback_idx_t {
    if (!node)
    {
      ossia_log_error("ossia_node_add_deleting_callback: node is null");
      return nullptr;
    }
    if (!callback)
    {
      ossia_log_error("ossia_node_add_deleting_callback: callback is null");
      return nullptr;
    }

    auto the_cb = new node_cb{callback, ctx};

    convert_node(node)->about_to_be_deleted.connect<node_cb>(the_cb);
    return reinterpret_cast<ossia_node_callback_idx_t>(the_cb);
  });
}

void ossia_node_remove_deleting_callback(
        ossia_node_t node,
        ossia_node_callback_idx_t index)
{
  return safe_function(__func__, [=] {
    auto idx = (node_cb*) index;
    if (!node)
    {
      ossia_log_error("ossia_node_remove_deleting_callback: node is null");
      return;
    }
    if (!idx)
    {
      ossia_log_error("ossia_node_remove_deleting_callback: index is null");
      return;
    }

    convert_node(node)->about_to_be_deleted.disconnect<node_cb>(idx);
    delete idx;
  });
}


void ossia_node_set_description(
    ossia_node_t node,
    const char* description)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_set_description: node is null");
      return;
    }

    ossia::net::set_description(*convert_node(node), description);
  });
}

const char* ossia_node_get_description(
    ossia_node_t node)
{
  return safe_function(__func__, [=]() -> const char* {
    if (!node)
    {
      ossia_log_error("ossia_node_get_description: node is null");
      return nullptr;
    }

    auto str = ossia::net::get_description(*convert_node(node));
    if(!str)
      return nullptr;
    
    return copy_string(*str);
  });
}


void ossia_node_set_hidden(
    ossia_node_t node,
    int hidden)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_set_hidden: node is null");
      return;
    }

    ossia::net::set_hidden(*convert_node(node), hidden);
  });
}

int ossia_node_get_hidden(
    ossia_node_t node)
{
  return safe_function(__func__, [=]() -> int {
    if (!node)
    {
      ossia_log_error("ossia_node_get_hidden: node is null");
      return 0;
    }

    return int(ossia::net::get_hidden(*convert_node(node)));
  });
}

}
