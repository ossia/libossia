#include "cue.hpp"

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/string_algorithms.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/common/path.hpp>

#include <boost/container/flat_map.hpp>
#include <boost/container/map.hpp>
#include <boost/container/small_vector.hpp>

#if defined(OSSIA_PROTOCOL_OSCQUERY)
#include <ossia/network/local/local.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>
#endif

#include <cassert>

namespace ossia
{
static constexpr bool
pattern_is_child_of_path(std::string_view path, std::string_view pattern)
{
  if(ossia::string_starts_with(path, pattern))
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

static void list_all_children_unsorted(ossia::net::node_base* node, auto& set)
{
  for(auto& n : node->unsafe_children())
  {
    //if(n->get_parameter())
    set.insert(n.get());
    list_all_children_unsorted(n.get(), set);
  }
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
    ossia::hash_set<ossia::net::node_base*>& preset, const ossia::net::node_base& node)
{
  preset.erase(const_cast<ossia::net::node_base*>(&node));

  for(auto& n : node.unsafe_children())
  {
    remove_node_from_selection_recursively(preset, *n);
  }
}

void namespace_selection::set_device(ossia::net::device_base* dev)
{
  m_selection.clear();

  if(this->dev)
  {
    this->dev->on_node_created.disconnect<&namespace_selection::on_node_created>(this);
    this->dev->on_node_removing.disconnect<&namespace_selection::on_node_removed>(this);
  }

  this->dev = dev;
  if(this->dev)
  {
    this->dev->on_node_created.connect<&namespace_selection::on_node_created>(this);
    this->dev->on_node_removing.connect<&namespace_selection::on_node_removed>(this);
  }

  namespace_select("/");
}

void cues::recall(ossia::net::node_base& root, namespace_selection& sel, int idx)
{
  if(!has_cue(idx))
    return;
  m_current = idx;
  recall(root, sel);
}

struct priority_sort
{
  bool operator()(
      ossia::net::parameter_base* n1, ossia::net::parameter_base* n2) const noexcept
  {
    return ossia::net::get_priority(n1->get_node())
           > ossia::net::get_priority(n2->get_node());
  }
};

struct protocol_safe_mode_enabler
{
#if defined(OSSIA_PROTOCOL_OSCQUERY)
  std::vector<std::pair<ossia::oscquery_asio::oscquery_server_protocol_base*, bool>>
      protocols{};
#endif

  explicit protocol_safe_mode_enabler(ossia::net::node_base& root)
  {
#if defined(OSSIA_PROTOCOL_OSCQUERY)
    auto& proto = root.get_device().get_protocol();
    if(auto oscq = dynamic_cast<ossia::oscquery_asio::oscquery_server_protocol_base*>(&proto))
    {
      protocols.emplace_back(oscq, oscq->force_ws());
      oscq->set_force_ws(true);
    }
    else if(auto mplex = dynamic_cast<ossia::net::multiplex_protocol*>(&proto))
    {
      for(auto& proto : mplex->get_protocols())
      {
        if(auto oscq
           = dynamic_cast<ossia::oscquery_asio::oscquery_server_protocol_base*>(proto.get()))
        {
          protocols.emplace_back(oscq, oscq->force_ws());
          oscq->set_force_ws(true);
        }
      }
    }
#endif
  }

  ~protocol_safe_mode_enabler()
  {
#if defined(OSSIA_PROTOCOL_OSCQUERY)
    for(auto [oscq, prev_val] : protocols)
    {
      oscq->set_force_ws(prev_val);
    }
#endif
  }
};

void cues::recall(ossia::net::node_base& root, namespace_selection& sel)
{
  if(!has_cue(m_current))
    return;

  auto oscq_safe_mode = protocol_safe_mode_enabler{root};

  sel.m_selection.clear();
  boost::container::small_flat_multimap<
      ossia::net::parameter_base*, ossia::value*, 512, priority_sort>
      params;
  params.reserve(m_cues[m_current].preset.size());
  for(auto& [addr, val] : this->m_cues[m_current].preset)
  {
    // No pattern in saved cue
    if(auto n = ossia::net::find_node(root, addr))
    {
      if(auto p = n->get_parameter())
      {
        if(!ossia::net::get_recall_safe(*n))
        {
          params.emplace(p, &val);
        }
      }

      // Add the node to the running selection
      while(sel.m_selection.insert(n).second)
      {
        n = n->get_parent();
        if(!n)
          break;
      }
    }
  }

  for(auto elt : params)
    elt.first->push_value(*elt.second);
}

void cues::remove()
{
  remove(m_current);
}

void namespace_selection::namespace_select(std::string_view name)
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
    list_all_children_unsorted(n, this->m_selection);
  }
}

void namespace_selection::namespace_deselect(std::string_view pattern)
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

  // v1
  // Actually this does not change the preset at all. Only the selection
  // // Remove all the nodes to the current preset
  // for(auto node : nodes)
  // {
  //   remove_node_recursively(cue.preset, *node);
  // }

  // v2
  for(auto n : nodes)
  {
    remove_node_from_selection_recursively(m_selection, *n);
  }
}

void namespace_selection::namespace_switch(std::string_view name)
{
  if(!dev)
    return;

  auto nodes = ossia::net::find_nodes(dev->get_root_node(), name);
  for(auto n : nodes)
  {
    if(!this->m_selection.contains(n))
    {
      // Select
      this->m_selection.insert(n);
      list_all_children_unsorted(n, this->m_selection);
    }
    else
    {
      // Deselect
      remove_node_from_selection_recursively(m_selection, *n);
    }
  }
}

void namespace_selection::namespace_grab(std::string_view name)
{
  if(!dev)
    return;
}

void namespace_selection::on_node_created(const net::node_base& n)
{
  if(m_selection.find(&dev->get_root_node()) != m_selection.end())
  {
    m_selection.insert(const_cast<net::node_base*>(&n));
  }
  else
  {
    while(auto parent = n.get_parent())
    {
      if(m_selection.find(parent) != m_selection.end())
      {
        m_selection.insert(const_cast<net::node_base*>(&n));
      }
    }
  }
}

void namespace_selection::on_node_removed(const net::node_base& n)
{
  m_selection.erase(const_cast<net::node_base*>(&n));
}

int cues::get_cue(std::string_view name)
{
  auto it = std::find_if(
      m_cues.begin(), m_cues.end(), [=](const cue& c) { return c.name == name; });
  if(BOOST_UNLIKELY(it == m_cues.end()))
  {
    m_cues.push_back(cue{.name = std::string(name), .preset = {}});
    return m_cues.size() - 1;
  }
  else
  {
    return std::distance(m_cues.begin(), it);
  }
}

std::optional<int> cues::find_cue(std::string_view name)
{
  auto it = std::find_if(
      m_cues.begin(), m_cues.end(), [=](const cue& c) { return c.name == name; });
  if(BOOST_UNLIKELY(it == m_cues.end()))
  {
    return std::nullopt;
  }
  else
  {
    return std::distance(m_cues.begin(), it);
  }
}

void cues::create(std::string_view name)
{
  m_current = get_cue(name);
}

void cues::remove(int idx)
{
  if(!has_cue(idx))
    return;

  m_cues.erase(m_cues.begin() + idx);

  if(m_current < idx)
  {
    // do nothing
  }
  else if(m_current == idx)
  {
    // We keep the same index selected
    if(m_current >= std::ssize(m_cues))
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

  if(m_cues.empty())
  {
    m_cues.push_back({.name{"Init"}, .preset = {}});
    m_current = 0;
  }
}

void cues::remove(std::string_view name)
{
  auto it = std::find_if(this->m_cues.begin(), this->m_cues.end(), [=](const cue& c) {
    return c.name == name;
  });

  if(it != this->m_cues.end())
  {
    int idx = std::distance(this->m_cues.begin(), it);
    remove(idx);
  }
}

void cues::rename(int idx, std::string_view newname)
{
  if(!has_cue(idx))
    return;

  m_cues[idx].name.assign(newname.begin(), newname.end());
}

void cues::rename(std::string_view name, std::string_view newname)
{
  auto it = std::find_if(this->m_cues.begin(), this->m_cues.end(), [=](const cue& c) {
    return c.name == name;
  });

  if(it != this->m_cues.end())
  {
    int idx = std::distance(this->m_cues.begin(), it);
    rename(idx, newname);
  }
}

void cues::rename(std::string_view newname)
{
  rename(m_current, newname);
}

void cues::recall(
    ossia::net::node_base& root, namespace_selection& sel, std::string_view name)
{
  m_current = get_cue(name);
  recall(root, sel);
}

void cues::update(
    ossia::net::node_base& root, const namespace_selection& selection, int idx)
{
  if(!has_cue(idx))
    return;

  auto& cue = this->m_cues[idx];

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
  // for(auto& [addr, val] : cue.preset)
  // {
  //   if(auto n = ossia::net::find_node(root, addr))
  //     if(auto p = n->get_parameter())
  //       val = p->value();
  // }

  // v4: we start from scratch
  cue.preset.clear();

  // And we also add the new ones
  for(auto node : selection.m_selection)
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

void cues::update(ossia::net::node_base& root, const namespace_selection& selection)
{
  update(root, selection, m_current);
}

void cues::update(
    ossia::net::node_base& root, const namespace_selection& selection,
    std::string_view name)
{
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
  update(root, selection, get_cue(name));
}

void cues::clear()
{
  m_cues.clear();
}

void cues::move(std::string_view name, int to)
{
  if(auto idx = find_cue(name))
    move(*idx, to);
}

void cues::move(int from, int to)
{
  if(from < 0 || to < 0 || from == to || from >= std::ssize(m_cues)
     || to >= std::ssize(m_cues))
    return;
  change_item_position(m_cues, from, to);
}

static bool filter_all_pass(const ossia::net::node_base& n, const selection_filters& pat)
{
  auto nn = n.get_parameter();
  if(!nn)
    return true;

  for(auto v : pat.access)
  {
    if(nn->get_access() != v)
      return false;
  }
  for(auto v : pat.bounding)
  {
    if(nn->get_bounding() != v)
      return false;
  }
  for(auto v : pat.type)
  {
    if(nn->get_value_type() != v)
      return false;
  }
  for(const auto& v : pat.tags)
  {
    const auto& tags = ossia::net::get_tags(n);
    if(!tags || !ossia::contains(*tags, v))
      return false;
  }
  return true;
}
static bool filter_any_pass(const ossia::net::node_base& n, const selection_filters& pat)
{
  auto nn = n.get_parameter();
  if(!nn)
    return true;

  for(auto v : pat.access)
  {
    if(nn->get_access() == v)
      return true;
  }
  for(auto v : pat.bounding)
  {
    if(nn->get_bounding() == v)
      return true;
  }
  for(auto v : pat.type)
  {
    if(nn->get_value_type() == v)
      return true;
  }
  for(const auto& v : pat.tags)
  {
    const auto& tags = ossia::net::get_tags(n);
    if(tags && ossia::contains(*tags, v))
      return true;
  }
  return false;
}

void namespace_selection::namespace_filter_all(const selection_filters& pat)
{
  for(auto it = this->m_selection.begin(); it != this->m_selection.end();)
  {
    if(filter_all_pass(**it, pat))
    {
      ++it;
    }
    else
    {
      it = m_selection.erase(it);
    }
  }
}

void namespace_selection::namespace_filter_any(const selection_filters& pat)
{
  for(auto it = this->m_selection.begin(); it != this->m_selection.end();)
  {
    if(filter_any_pass(**it, pat))
    {
      ++it;
    }
    else
    {
      it = m_selection.erase(it);
    }
  }
}

bool cues::has_cue(int cue) const noexcept
{
  return cue >= 0 && cue < std::ssize(m_cues);
}

}
