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
}
