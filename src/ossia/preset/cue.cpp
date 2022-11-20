#include "cue.hpp"

#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/common/path.hpp>

#include <cassert>
#include <iostream>

namespace ossia
{
static constexpr bool
pattern_is_child_of_path(std::string_view path, std::string_view pattern)
{
  if(path.starts_with(pattern))
  {
    if(path == pattern || path[pattern.size()] == '/')
    {
      return true;
    }
  }
  return false;
}

static void
add_node_recursively(ossia::presets::preset& preset, const ossia::net::node_base& node)
{
  if(auto p = node.get_parameter())
  {
    const auto& addr = node.osc_address();
    auto it = ossia::find_key(preset, addr);
    if(it == preset.end())
      preset.emplace_back(addr, p->value());
    else
      it->second = p->value();
  }

  for(auto& cld : node.children())
  {
    add_node_recursively(preset, *cld);
  }
}
static void load_in_cue(cue& c, const std::vector<ossia::net::node_base*>& nodes)
{
  // Add all the nodes to the current preset
  for(auto node : nodes)
  {
    add_node_recursively(c.preset, *node);
  }
}

static void list_all_children_unsorted_rec(ossia::net::node_base* node, auto& set)
{
  for(auto& n : node->unsafe_children())
  {
    //if(n->get_parameter())
    set.insert(n.get());
    list_all_children_unsorted_rec(n.get(), set);
  }
}

static void list_all_children_unsorted(ossia::net::node_base* node, auto& set)
{
  list_all_children_unsorted_rec(node, set);
}

static void remove_node_recursively(
    ossia::presets::preset& preset, const ossia::net::node_base& node)
{
  const auto& addr = node.osc_address();
  for(auto it = preset.begin(); it != preset.end();)
  {
    if(pattern_is_child_of_path(it->first, addr))
      it = preset.erase(it);
    else
      ++it;
  }
}
static void remove_node_from_selection_recursively(
    std::unordered_set<ossia::net::node_base*>& preset,
    const ossia::net::node_base& node)
{
  preset.erase(const_cast<ossia::net::node_base*>(&node));

  for(auto& n : node.unsafe_children())
  {
    remove_node_from_selection_recursively(preset, *n);
  }
}

void cues::set_device(ossia::net::device_base* dev)
{
  m_selection.clear();

  if(this->dev)
  {
    this->dev->on_node_created.disconnect<&cues::on_node_created>(this);
    this->dev->on_node_removing.disconnect<&cues::on_node_removed>(this);
  }

  this->dev = dev;
  if(this->dev)
  {
    this->dev->on_node_created.connect<&cues::on_node_created>(this);
    this->dev->on_node_removing.connect<&cues::on_node_removed>(this);
  }

  namespace_select("/");
}

void cues::recall(int idx)
{
  if(idx < 0)
    return;
  if(idx >= std::ssize(cues))
    return;
  m_current = idx;
  recall();
}

void cues::recall()
{
  auto& root = dev->get_root_node();
  for(auto& [addr, val] : cues[m_current].preset)
  {
    // No pattern in saved cue
    if(auto n = ossia::net::find_node(root, addr))
      if(auto p = n->get_parameter())
        if(!ossia::net::get_recall_safe(*n))
          p->push_value(val);
    // if(ossia::traversal::is_pattern(name))
    // {
    //   ossia::traversal::make_path(name);
    // }
  }
}

void cues::remove()
{
  remove(m_current);
}

void cues::namespace_select(std::string_view name)
{
  if(!dev)
    return;

  //auto& cue = m_cues[m_current];
  //cue.selection.emplace_back(name);
  //load_in_cue(cue, nodes);

  auto nodes = ossia::net::find_nodes(dev->get_root_node(), name);
  for(auto n : nodes)
  {
    //if(n->get_parameter())
    this->m_selection.insert(n);
    list_all_children_unsorted_rec(n, this->m_selection);
  }
}

void cues::namespace_deselect(std::string_view pattern)
{
  if(!dev)
    return;

  // auto& cue = m_cues[m_current];

  /*
  for(auto it = selection.begin(); it != selection.end();)
  {
    auto& path = *it;
    if(pattern_is_child_of_path(path, pattern))
      it = selection.erase(it);
    else
      ++it;
  }
  */

  auto nodes = ossia::net::find_nodes(dev->get_root_node(), pattern);
  // std::cout << "namespace_deselect: " << pattern << " : " << nodes.size() << std::endl;

  // v1
  // Actually this does not change the preset at all. Only the selection
  // // Remove all the nodes to the current preset
  // for(auto node : nodes)
  // {
  //   remove_node_recursively(cue.preset, *node);
  // }

  // v2
  for(auto node : nodes)
  {
    remove_node_from_selection_recursively(m_selection, *node);
  }
}

void cues::namespace_grab(std::string_view name)
{
  if(!dev)
    return;
}

void cues::on_node_created(const net::node_base& n)
{
  if(m_selection.contains(&dev->get_root_node()))
  {
    m_selection.insert(const_cast<net::node_base*>(&n));
  }
  else
  {
    while(auto parent = n.get_parent())
    {
      if(m_selection.contains(parent))
      {
        m_selection.insert(const_cast<net::node_base*>(&n));
      }
    }
  }
}

void cues::on_node_removed(const net::node_base& n)
{
  m_selection.erase(const_cast<net::node_base*>(&n));
}

int cues::get_cue(std::string_view name)
{
  auto it = std::find_if(
      cues.begin(), cues.end(), [=](const cue& c) { return c.name == name; });
  if(BOOST_UNLIKELY(it == cues.end()))
  {
    cues.push_back(cue{.name{name}});
    return cues.size() - 1;
  }
  else
  {
    return std::distance(cues.begin(), it);
  }
}
std::optional<int> cues::find_cue(std::string_view name)
{
  auto it = std::find_if(
      cues.begin(), cues.end(), [=](const cue& c) { return c.name == name; });
  if(BOOST_UNLIKELY(it == cues.end()))
  {
    return std::nullopt;
  }
  else
  {
    return std::distance(cues.begin(), it);
  }
}

void cues::create(std::string_view name)
{
  if(!dev)
    return;
  m_current = get_cue(name);
}

void cues::remove(int idx)
{
  if(idx < 0)
    return;
  if(idx >= std::ssize(cues))
    return;

  cues.erase(cues.begin() + idx);

  if(m_current < idx)
  {
    // do nothing
  }
  else if(m_current == idx)
  {
    // We keep the same index selected
    if(m_current >= std::ssize(cues))
    {
      m_current -= 1;
    }
  }
  else if(m_current > idx)
  {
    // If we were at idx 4 and we remove cue 2,
    // we are now at idx 3 to keep the same cue
    m_current -= 1;
  }

  if(cues.empty())
  {
    cues.push_back({.name{"Init"}});
    m_current = 0;
  }
}

void cues::remove(std::string_view name)
{
  auto it = std::find_if(
      cues.begin(), cues.end(), [=](const cue& c) { return c.name == name; });

  if(it != cues.end())
  {
    int idx = std::distance(cues.begin(), it);
    remove(idx);
  }
}

void cues::recall(std::string_view name)
{
  if(!dev)
    return;
  m_current = get_cue(name);
  recall();
}

void cues::update(int idx)
{
  if(!dev)
    return;
  assert(idx >= 0);
  assert(idx < std::ssize(cues));
  auto& cue = cues[idx];

  // v1
  /*
  auto& root = dev->get_root_node();
  for(auto& [addr, val] : m_cues[idx].preset)
  {
    if(auto n = ossia::net::find_node(root, addr))
      if(auto p = n->get_parameter())
        val = p->value();
  }
  */

  // v2
  /*
  for(auto& name : cue.selection)
  {
    auto nodes = ossia::net::find_nodes(dev->get_root_node(), name);
    load_in_cue(cue, nodes);
  }
  */

  // v3: we always update the local nodes
  auto& root = dev->get_root_node();
  for(auto& [addr, val] : cue.preset)
  {
    if(auto n = ossia::net::find_node(root, addr))
      if(auto p = n->get_parameter())
        val = p->value();
  }

  // And we also add the new ones
  for(auto node : m_selection)
  {
    if(auto p = node->get_parameter())
    {
      const auto& addr = node->osc_address();
      auto it = find_key(cue.preset, addr);
      if(it != cue.preset.end())
      {
        it->second = p->value();
      }
      else
      {
        cue.preset.push_back({addr, p->value()});
      }
    }
  }
}

void cues::update()
{
  update(m_current);
}

void cues::update(std::string_view name)
{
  if(!dev)
    return;
  // Question:
  // If we add the pattern: /foo.* to a cue
  // in the tree
  // /foo.1/volume
  // /foo.2/volume
  //
  // And we add a /foo.3
  // is it taken as part of the update too ?
  // e.g. are patterns stored and evaluated lazily or eagerly?
  // Let's assume eagerly: cue only has explicit addresses
  update(get_cue(name));
}

void cues::output(std::string_view name)
{
  if(!dev)
    return;
  int idx = get_cue(name);
}

void cues::output(std::string_view name, std::string_view pattern)
{
  if(!dev)
    return;
  int idx = get_cue(name);
}

void cues::clear()
{
  cues.clear();
}

void cues::move(std::string_view name, int to)
{
  if(auto idx = find_cue(name))
    move(*idx, to);
}

void cues::move(int from, int to)
{
  if(from < 0 || to < 0 || from == to)
    return;
  change_item_position(cues, from, to);
}
}