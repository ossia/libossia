// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include "ossia_utils.hpp"
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/common/complex_type.hpp>

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
        ossia_node_t** data,
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
        auto ptr = (ossia_node_t*)std::malloc(sizeof(ossia_node_t) * nodes.size());
        for(std::size_t i = 0; i < nodes.size(); i++)
          ptr[i] = convert(nodes[i]);
        *data = ptr;
      }
  });
}

void ossia_node_array_free(
    ossia_node_t* data)
{
    free(data);
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
        ossia_node_t** data,
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
        auto ptr = (ossia_node_t*)std::malloc(sizeof(ossia_node_t) * nodes.size());
        for(std::size_t i = 0; i < nodes.size(); i++)
          ptr[i] = convert(nodes[i]);
        *data = ptr;
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
    return convert(n->find_child(ossia::string_view(child_n)));
  });
}

ossia_parameter_t ossia_node_create_parameter(ossia_node_t node, ossia_type type)
{
  return safe_function(__func__, [=]() -> ossia_parameter_t {
    if (!node)
    {
      ossia_log_error("ossia_node_create_parameter: node is null");
      return nullptr;
    }

    auto n = convert_node(node);
    return convert(n->create_parameter(convert(type)));
  });
}

ossia_parameter_t ossia_create_parameter(
    ossia_node_t node,
    const char* name,
    const char* type)
{
  return safe_function(__func__, [=]() -> ossia_parameter_t {
    if (!node)
    {
      ossia_log_error("ossia_create_parameter: node is null");
      return nullptr;
    }
    if (!name)
    {
      ossia_log_error("ossia_create_parameter: name is null");
      return nullptr;
    }
    if (!type)
    {
      ossia_log_error("ossia_create_parameter: type is null");
      return nullptr;
    }

    auto n = convert_node(node);
    return convert(ossia::create_parameter(*n, name, type));
  });
}


ossia_parameter_t ossia_node_get_parameter(ossia_node_t node)
{
  return safe_function(__func__, [=]() -> ossia_parameter_t {
    if (!node)
    {
      ossia_log_error("ossia_node_create_parameter: node is null");
      return nullptr;
    }

    auto n = convert_node(node);
    return convert(n->get_parameter());
  });
}

void ossia_node_remove_parameter(ossia_node_t node)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_remove_parameter: node is null");
      return;
    }

    auto n = convert_node(node);
    n->remove_parameter();
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


void ossia_node_set_extended_type(
    ossia_node_t node,
    const char* extended_type)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_set_extended_type: node is null");
      return;
    }

    auto& n = *convert_node(node);
    if(extended_type)
      ossia::net::set_extended_type(n, ossia::extended_type{extended_type});
    else
      ossia::net::set_extended_type(n, ossia::none);
  });
}

const char* ossia_node_get_extended_type(
    ossia_node_t node)
{
  return safe_function(__func__, [=]() -> const char* {
    if (!node)
    {
      ossia_log_error("ossia_node_get_extended_type: node is null");
      return nullptr;
    }

    auto str = ossia::net::get_extended_type(*convert_node(node));
    if(!str)
      return nullptr;

    return copy_string(*str);
  });
}


OSSIA_EXPORT
void ossia_node_set_tags(
    ossia_node_t node,
    const char** tags,
    size_t sz)
{
  if (!node)
  {
    ossia_log_error("ossia_node_set_tags: a parameter is null");
  }
  else
  {
    auto& n = *convert_node(node);

    if(tags == nullptr || sz == 0)
    {
      ossia::net::set_tags(n, {});
    }
    else
    {
      std::vector<std::string> vec(sz);
      for(size_t i = 0; i < sz; ++i)
      {
        vec[i] = tags[i];
      }
      ossia::net::set_tags(n, std::move(vec));
    }
  }
}

OSSIA_EXPORT
void ossia_node_get_tags(
    ossia_node_t node,
    char*** tags,
    size_t* size)
{
  if (!node || !tags || !size)
  {
    ossia_log_error("ossia_node_get_tags: a parameter is null");
  }
  else
  {
    auto t = ossia::net::get_tags(*convert_node(node));
    if(t && !t->empty())
    {
      auto& vtags = *t;
      auto ptr = new char*[vtags.size()];
      for(size_t i = 0; i < vtags.size(); ++i)
      {
        ptr[i] = copy_string(vtags[i]);
      }
      *tags = ptr;
      return;
    }
  }

  *tags = nullptr;
  *size = 0;
}

void ossia_tags_free(char** tags, size_t size)
{
  for(size_t i = 0; i < size; i++)
    free(tags[i]);
  delete[] tags;
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

void ossia_node_set_priority(
    ossia_node_t node,
    float priority)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_set_priority: node is null");
      return;
    }

    ossia::net::set_priority(*convert_node(node), priority);
  });
}
void ossia_node_unset_priority(
    ossia_node_t node)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_unset_priority: node is null");
      return;
    }

    ossia::net::set_priority(*convert_node(node), ossia::none);
  });
}
float ossia_node_get_priority(
    ossia_node_t node,
    int* b)
{
  return safe_function(__func__, [=]() -> float {
    if (!node)
    {
      ossia_log_error("ossia_node_get_priority: node is null");
      if(b) *b = 0;
      return 0;
    }

    auto p = ossia::net::get_priority(*convert_node(node));
    if(p) {
      if(b) *b = 1;
      return *p;
    } else {
      if(b) *b = 0;
      return 0.f;
    }
  });
}

void ossia_node_set_refresh_rate(
    ossia_node_t node,
    int refresh_rate)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_set_refresh_rate: node is null");
      return;
    }

    ossia::net::set_refresh_rate(*convert_node(node), refresh_rate);
  });
}
void ossia_node_unset_refresh_rate(
    ossia_node_t node)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_unset_refresh_rate: node is null");
      return;
    }

    ossia::net::set_refresh_rate(*convert_node(node), ossia::none);
  });
}
int ossia_node_get_refresh_rate(
    ossia_node_t node,
    int* b)
{
  return safe_function(__func__, [=]() -> int {
    if (!node)
    {
      ossia_log_error("ossia_node_get_refresh_rate: node is null");
      if(b) *b = 0;
      return 0;
    }

    auto p = ossia::net::get_refresh_rate(*convert_node(node));
    if(p) {
      if(b) *b = 1;
      return *p;
    } else {
      if(b) *b = 0;
      return 0;
    }
  });
}

void ossia_node_set_value_step_size(
    ossia_node_t node,
    double value_step_size)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_set_value_step_size: node is null");
      return;
    }

    ossia::net::set_value_step_size(*convert_node(node), value_step_size);
  });
}
void ossia_node_unset_value_step_size(
    ossia_node_t node)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_unset_value_step_size: node is null");
      return;
    }

    ossia::net::set_value_step_size(*convert_node(node), ossia::none);
  });
}
double ossia_node_get_value_step_size(
    ossia_node_t node,
    int* b)
{
  return safe_function(__func__, [=]() -> double {
    if (!node)
    {
      ossia_log_error("ossia_node_get_value_step_size: node is null");
      if(b) *b = 0;
      return 0;
    }

    auto p = ossia::net::get_value_step_size(*convert_node(node));
    if(p) {
      if(b) *b = 1;
      return *p;
    } else {
      if(b) *b = 0;
      return 0.;
    }
  });
}


void ossia_node_set_instance_bounds(
    ossia_node_t node,
    int min, int max)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_set_instance_bounds: node is null");
      return;
    }

    ossia::net::set_instance_bounds(
          *convert_node(node),
          ossia::net::instance_bounds{min, max});
  });
}
void ossia_node_unset_instance_bounds(
    ossia_node_t node)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_unset_instance_bounds: node is null");
      return;
    }

    ossia::net::set_instance_bounds(*convert_node(node), ossia::none);
  });
}
void ossia_node_get_instance_bounds(
    ossia_node_t node,
    int* min, int *max,
    int* ok)
{
  return safe_function(__func__, [=] {
    if (!node || !min || !max)
    {
      ossia_log_error("ossia_node_get_instance_bounds: an argument is null");
      if(ok) *ok = 0;
      return;
    }

    auto p = ossia::net::get_instance_bounds(*convert_node(node));
    if(p) {
      *min = p->min_instances;
      *max = p->max_instances;
      if(ok) *ok = 1;
      return;
    } else {
      if(ok) *ok = 0;
      return;
    }
  });
}



void ossia_node_set_default_value(
    ossia_node_t node,
    ossia_value_t default_value)
{
  return safe_function(__func__, [=] {
    if (!node)
    {
      ossia_log_error("ossia_node_set_default_value: node is null");
      return;
    }

    auto& n = *convert_node(node);
    if(!default_value)
      ossia::net::set_default_value(n, ossia::none);
    else
      ossia::net::set_default_value(n, convert(default_value->value));
  });
}
ossia_value_t ossia_node_get_default_value(
    ossia_node_t node)
{
  return safe_function(__func__, [=]() -> ossia_value_t {
    if (!node)
    {
      ossia_log_error("ossia_node_get_default_value: node is null");
      return 0;
    }

    auto val = ossia::net::get_default_value(*convert_node(node));
    if(val)
      return convert(*val);
    else
      return nullptr;
  });
}
}
