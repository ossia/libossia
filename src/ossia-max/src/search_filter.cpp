#include "search_filter.hpp"

#include "ext_obex_util.h"
#include "utils.hpp"

#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/node_attributes.hpp>

namespace ossia
{
namespace max_binding
{

static auto s_set = gensym("set");
static auto s_cell = gensym("cell");
static auto s_clear = gensym("clear");
static auto s_rows = gensym("rows");
static auto s_namespace = gensym("namespace");
static auto s_explore = gensym("explore");
static auto s_size = gensym("size");

std::vector<ossia::net::node_base*>  search_sort_filter::sort_and_filter(std::vector<std::shared_ptr<matcher>>& matchers)
{
  std::vector<ossia::net::node_base*> nodes;
  const bool alpha_sort = m_sort == gensym("alphabetic");
  const bool prio_sort = m_sort == gensym("priority");
  const bool default_sort = !(alpha_sort || prio_sort);
  for(const auto& m : matchers)
  {
    ossia::net::list_all_children(m->get_node(), nodes, m_depth, default_sort);
  }

  // Filter search parameters
  ossia::remove_erase_if(
      nodes, [&](const ossia::net::node_base* m) { return filter(*m); });

  // Sort
  if(alpha_sort)
  {
    // FIXME refactor with preset.cpp, etc
    constexpr auto sort = [] (ossia::net::node_base* n1, ossia::net::node_base* n2) -> bool {
      return ossia::net::osc_parameter_string(*n1) < ossia::net::osc_parameter_string(*n2);
    };
    ossia::sort(nodes, sort);
  }
  else if(prio_sort)
  {
    // FIXME refactor with preset.cpp, etc
    constexpr auto sort = [] (ossia::net::node_base* n1, ossia::net::node_base* n2) -> bool {
      return ossia::net::get_priority(*n1) > ossia::net::get_priority(*n2);
    };
    ossia::sort(nodes, sort);
  }

  return nodes;
}

bool search_filter::filter(const ossia::net::node_base& node)
{
  if(m_filter_terminal > 0)
  {
    if(!node.children().empty())
      return true;
  }

  if(m_filter_type_size > 0)
  {
    auto& omax = ossia_max::instance();
    std::lock_guard _{omax.s_node_matchers_mut};
    auto candidates_it = omax.s_node_matchers_map.find((ossia::net::node_base*)&node);

    // If it's an intermediary node it does not match any proper ossia object
    if(candidates_it == omax.s_node_matchers_map.end())
    {
      return true;
    }

    auto& candidates = candidates_it->second.reference();
    if(m_filter_type[0] == gensym("model"))
    {
      for(const auto& m : candidates)
      {
        if(object_base* obj = m->get_owner())
        {
          if(obj->m_otype != object_class::model)
          {
            return true;
          }
        }
      }
    }
    else if(m_filter_type[0] == gensym("parameter"))
    {
      for(const auto& m : candidates)
      {
        if(object_base* obj = m->get_owner())
        {
          if(obj->m_otype != object_class::param)
          {
            return true;
          }
        }
      }
    }
  }

  if(m_filter_tags_size > 0)
  {
    bool match = true;
    const auto& tags_opt = ossia::net::get_tags(node.get_extended_attributes());
    if(!tags_opt.has_value())
      return true;

    const std::vector<std::string>& tags = *tags_opt;

    for(int i = 0; i < m_filter_tags_size; i++)
    {
      std::string tag(m_filter_tags[i]->s_name);

      if(ossia::find(tags, tag) == tags.end())
      {
        match = false;
        break;
      }
    }
    if(!match)
      return true;
  }

  if(m_filter_visible > 0)
  {
    auto hidden = ossia::net::get_hidden(node.get_extended_attributes());
    switch(m_filter_visible)
    {
      case 1:
        if(hidden)
          return true;
        break;
      case 2:
        if(!hidden)
          return true;
        break;
      default:;
    }
  }

  if(m_filter_modes_size > 0)
  {
    const auto& access_opt = ossia::net::get_access_mode(node);
    if(!access_opt.has_value())
      return true;
    bool match = false;
    for(int i = 0; i < m_filter_modes_size; i++)
    {
      if(symbol2access_mode(m_filter_modes[i]) == *access_opt)
      {
        match = true;
        break;
      }
    }
    if(!match)
      return true;
  }

  return false;
}


void dump_node_list(void* outlet, const std::vector<ossia::net::node_base*>& nodes, t_symbol* format) 
{
  // Output
  {
    t_atom a;
    A_SETLONG(&a, nodes.size());
    outlet_anything(outlet, s_size, 1, &a);
  }
  auto format_param_default
      = [outlet](const std::vector<ossia::net::node_base*>& nodes, std::vector<t_atom>& va) {
    for(const auto& nn : nodes)
    {
      auto& n = *nn;

      // Print the name
      va.clear();
      value2atom vm{va};
      vm(ossia::net::osc_parameter_string(n));

      // Print the param
      if(auto param = n.get_parameter())
      {
        const ossia::value& val = param->value();
        val.apply(vm);
      }

      outlet_anything(outlet, s_namespace, va.size(), va.data());
    }
  };

  auto format_param_cellblock
      = [outlet](const std::vector<ossia::net::node_base*>& nodes, std::vector<t_atom>& va) {
    int k = 0;
    // First clear
    write_message(va, outlet, s_clear, "all");
    write_message(va, outlet, s_rows, int(std::ssize(nodes)));

    for(const auto& nn : nodes)
    {
      auto& n = *nn;
      // Format:

      // set x y <name>
      const auto& osc_param = ossia::net::osc_parameter_string(n);
      write_message(va, outlet, s_set, 0, k, osc_param.c_str());

      // cell x y brgb r g b
      write_message(va, outlet, s_cell, 0, k, "brgb", 0, 0, 0);
      k++;
    }
  };


  std::vector<t_atom> va;
  va.reserve(6);
  if(format == gensym("list") || format == gensym("none") || !format)
  {
    format_param_default(nodes, va);
  }
  else if(format == gensym("jit.cellblock"))
  {
    format_param_cellblock(nodes, va);
  }
}

} // namespace max
} // namespace ossia
