// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "v8connector_branch.hpp"

#include <ossia/network/base/address_scope.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia/network/common/value_bounding.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/oscquery/detail/osc_writer.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/value/impulse.hpp>
#include <ossia/network/value/vec.hpp>

#include <cmath>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <utility>

namespace ossia
{
namespace max_binding
{
namespace v8connector_detail
{

namespace
{

// Set on the pushing thread while a branch pushes a value itself, with the
// value the host asked for. libossia calls the value callbacks synchronously on
// that thread, so the branch can tell its own pushes apart from every other
// source without a race, and let through a value the parameter changed.
thread_local const branch* t_pushing = nullptr;
thread_local const scalars* t_pushing_request = nullptr;

struct pushing_scope
{
  pushing_scope(const branch* b, const scalars* request)
  {
    t_pushing = b;
    t_pushing_request = request;
  }
  ~pushing_scope()
  {
    t_pushing = nullptr;
    t_pushing_request = nullptr;
  }
};

bool is_number(const scalar& s)
{
  return !std::holds_alternative<std::string>(s);
}

float to_float(const scalar& s)
{
  switch(s.index())
  {
    case 0:
      return static_cast<float>(std::get<int>(s));
    case 1:
      return std::get<float>(s);
    default:
      return 0.f;
  }
}

std::string to_string(const scalar& s)
{
  switch(s.index())
  {
    case 0:
      return std::to_string(std::get<int>(s));
    case 1: {
      std::ostringstream o;
      o << std::get<float>(s);
      return o.str();
    }
    default:
      return std::get<std::string>(s);
  }
}

ossia::value to_value(const scalar& s)
{
  switch(s.index())
  {
    case 0:
      return std::get<int>(s);
    case 1:
      return std::get<float>(s);
    default:
      return std::get<std::string>(s);
  }
}

struct flatten
{
  scalars& out;
  void operator()(float f) const { out.push_back(f); }
  void operator()(int i) const { out.push_back(i); }
  void operator()(bool b) const { out.push_back(b ? 1 : 0); }
  void operator()(ossia::impulse) const { }
  void operator()(const std::string& s) const { out.push_back(s); }
  void operator()(const std::vector<ossia::value>& l) const
  {
    for(const auto& v : l)
      v.apply(*this);
  }
  void operator()(const ossia::value_map_type&) const { }
  template <std::size_t N>
  void operator()(const std::array<float, N>& a) const
  {
    for(float f : a)
      out.push_back(f);
  }
  void operator()() const { }
};

// True when a value holds exactly what the host sent: same count, numbers equal
// as numbers, symbols equal as text. An impulse carries nothing to compare.
bool scalars_match(const scalars& request, const ossia::value& v)
{
  if(v.get_type() == ossia::val_type::IMPULSE)
    return true;
  scalars got;
  if(v.valid())
    v.apply(flatten{got});
  if(got.size() != request.size())
    return false;
  for(std::size_t i = 0; i < got.size(); ++i)
  {
    const bool num_r = is_number(request[i]);
    const bool num_g = is_number(got[i]);
    if(num_r != num_g)
      return false;
    if(num_r)
    {
      if(to_float(request[i]) != to_float(got[i]))
        return false;
    }
    else if(std::get<std::string>(request[i]) != std::get<std::string>(got[i]))
      return false;
  }
  return true;
}

ossia::val_type type_of(std::string_view type)
{
  if(type == "float")
    return ossia::val_type::FLOAT;
  if(type == "int")
    return ossia::val_type::INT;
  if(type == "bool")
    return ossia::val_type::BOOL;
  if(type == "impulse")
    return ossia::val_type::IMPULSE;
  if(type == "string")
    return ossia::val_type::STRING;
  if(type == "list")
    return ossia::val_type::LIST;
  if(type == "vec2f")
    return ossia::val_type::VEC2F;
  if(type == "vec3f")
    return ossia::val_type::VEC3F;
  if(type == "vec4f")
    return ossia::val_type::VEC4F;
  return ossia::val_type::NONE;
}

std::string_view type_name(ossia::val_type t)
{
  switch(t)
  {
    case ossia::val_type::FLOAT:
      return "float";
    case ossia::val_type::INT:
      return "int";
    case ossia::val_type::BOOL:
      return "bool";
    case ossia::val_type::IMPULSE:
      return "impulse";
    case ossia::val_type::STRING:
      return "string";
    case ossia::val_type::LIST:
      return "list";
    case ossia::val_type::VEC2F:
      return "vec2f";
    case ossia::val_type::VEC3F:
      return "vec3f";
    case ossia::val_type::VEC4F:
      return "vec4f";
    default:
      return "unknown";
  }
}

// Number of numeric components of a type, 0 for non numeric types.
int numeric_components(std::string_view type)
{
  if(type == "float" || type == "int")
    return 1;
  if(type == "vec2f")
    return 2;
  if(type == "vec3f")
    return 3;
  if(type == "vec4f")
    return 4;
  return 0;
}

bool known_type(std::string_view type)
{
  static constexpr std::string_view types[]
      = {"float", "int", "bool", "impulse", "string", "list", "vec2f", "vec3f", "vec4f"};
  return std::find(std::begin(types), std::end(types), type) != std::end(types);
}

std::string one_symbol(const attribute& a, std::string& out)
{
  if(a.values.size() != 1 || !std::holds_alternative<std::string>(a.values[0]))
    return "@" + a.name + " expects one symbol";
  out = std::get<std::string>(a.values[0]);
  return {};
}

std::string numbers(const attribute& a, std::vector<float>& out)
{
  if(a.values.empty())
    return "@" + a.name + " expects at least one number";
  out.clear();
  for(const auto& v : a.values)
  {
    if(!is_number(v))
      return "@" + a.name + " expects numbers";
    out.push_back(to_float(v));
  }
  return {};
}

std::string one_number(const attribute& a, float& out)
{
  if(a.values.size() != 1 || !is_number(a.values[0]))
    return "@" + a.name + " expects one number";
  out = to_float(a.values[0]);
  return {};
}

std::string flag(const attribute& a, bool& out)
{
  if(a.values.size() != 1 || !is_number(a.values[0]))
    return "@" + a.name + " expects 0 or 1";
  out = to_float(a.values[0]) != 0.f;
  return {};
}

ossia::bounding_mode to_bounding(std::string_view clip)
{
  if(clip == "both")
    return ossia::bounding_mode::CLIP;
  if(clip == "low")
    return ossia::bounding_mode::CLAMP_LOW;
  if(clip == "high")
    return ossia::bounding_mode::CLAMP_HIGH;
  if(clip == "wrap")
    return ossia::bounding_mode::WRAP;
  if(clip == "fold")
    return ossia::bounding_mode::FOLD;
  return ossia::bounding_mode::FREE;
}

ossia::access_mode to_access(std::string_view mode)
{
  if(mode == "get" || mode == "r")
    return ossia::access_mode::GET;
  if(mode == "set" || mode == "w")
    return ossia::access_mode::SET;
  return ossia::access_mode::BI;
}

template <std::size_t N>
std::string to_vec(const scalars& in, ossia::value& out)
{
  if(in.size() != N)
    return "expects " + std::to_string(N) + " numbers, got " + std::to_string(in.size());
  std::array<float, N> a{};
  for(std::size_t i = 0; i < N; ++i)
  {
    if(!is_number(in[i]))
      return "expects numbers";
    a[i] = to_float(in[i]);
  }
  out = a;
  return {};
}

}

std::string normalize_relative_address(std::string_view address)
{
  while(!address.empty() && std::isspace(static_cast<unsigned char>(address.front())))
    address.remove_prefix(1);
  while(!address.empty() && std::isspace(static_cast<unsigned char>(address.back())))
    address.remove_suffix(1);
  while(!address.empty() && address.front() == '/')
    address.remove_prefix(1);
  while(!address.empty() && address.back() == '/')
    address.remove_suffix(1);
  if(address.empty())
    return {};
  if(address.find("//") != std::string_view::npos)
    return {};
  if(address.find(':') != std::string_view::npos)
    return {};
  for(unsigned char c : address)
    if(std::isspace(c))
      return {};
  return std::string(address);
}

std::string make_leaf_spec(
    std::string_view address, const std::vector<attribute>& attrs, leaf_spec& out)
{
  leaf_spec spec;
  spec.address = normalize_relative_address(address);
  if(spec.address.empty())
    return "invalid address '" + std::string(address) + "'";

  const std::string where = spec.address + ": ";
  std::vector<scalar> range;
  bool has_type = false;

  for(const auto& a : attrs)
  {
    std::string err;
    if(a.name == "type")
    {
      err = one_symbol(a, spec.type);
      if(err.empty())
      {
        if(spec.type == "symbol")
          spec.type = "string";
        if(!known_type(spec.type))
          err = "unknown type '" + spec.type + "'";
        has_type = true;
      }
    }
    else if(a.name == "min")
      err = numbers(a, spec.min);
    else if(a.name == "max")
      err = numbers(a, spec.max);
    else if(a.name == "range")
    {
      if(a.values.empty())
        err = "@range expects at least one value";
      range = a.values;
    }
    else if(a.name == "unit")
      err = one_symbol(a, spec.unit);
    else if(a.name == "default")
    {
      if(a.values.empty())
        err = "@default expects a value";
      spec.default_value = a.values;
    }
    else if(a.name == "clip")
    {
      err = one_symbol(a, spec.clip);
      if(err.empty())
      {
        // "none" is the name OSCQuery gives to the free mode.
        if(spec.clip == "none")
          spec.clip = "free";
        static constexpr std::string_view clips[]
            = {"free", "both", "low", "high", "wrap", "fold"};
        if(std::find(std::begin(clips), std::end(clips), spec.clip) == std::end(clips))
          err = "unknown clip mode '" + spec.clip + "'";
      }
    }
    else if(a.name == "mode")
    {
      err = one_symbol(a, spec.mode);
      if(err.empty())
      {
        static constexpr std::string_view modes[] = {"bi", "get", "set", "rw", "r", "w"};
        if(std::find(std::begin(modes), std::end(modes), spec.mode) == std::end(modes))
          err = "unknown mode '" + spec.mode + "'";
      }
    }
    else if(a.name == "description")
      err = one_symbol(a, spec.description);
    else if(a.name == "tags")
    {
      spec.tags.clear();
      for(const auto& v : a.values)
        spec.tags.push_back(to_string(v));
    }
    else if(a.name == "priority")
    {
      float f{};
      err = one_number(a, f);
      spec.priority = f;
    }
    else if(a.name == "rate")
    {
      float f{};
      err = one_number(a, f);
      spec.rate = f;
    }
    else if(a.name == "critical")
      err = flag(a, spec.critical);
    else if(a.name == "invisible")
      err = flag(a, spec.invisible);
    else if(a.name == "mute")
      err = flag(a, spec.mute);
    else if(a.name == "repetitions")
      err = flag(a, spec.repetitions);
    else if(a.name == "enable")
      err = flag(a, spec.enable);
    else if(a.name == "recall_safe")
      err = flag(a, spec.recall_safe);
    else
      return where + "unknown attribute '@" + a.name + "'";

    if(!err.empty())
      return where + err;
  }

  if(!has_type)
    return where + "missing @type";

  const int n = numeric_components(spec.type);
  if(n > 0)
  {
    if(!range.empty())
    {
      if(!spec.min.empty() || !spec.max.empty())
        return where + "@range and @min/@max are exclusive";
      std::vector<float> r;
      for(const auto& v : range)
      {
        if(!is_number(v))
          return where + "@range expects numbers for a numeric type";
        r.push_back(to_float(v));
      }
      if(r.size() == 2)
      {
        spec.min = {r[0]};
        spec.max = {r[1]};
      }
      else if(static_cast<int>(r.size()) == 2 * n)
      {
        for(int i = 0; i < n; ++i)
        {
          spec.min.push_back(r[2 * i]);
          spec.max.push_back(r[2 * i + 1]);
        }
      }
      else
        return where + "@range expects 2 or " + std::to_string(2 * n) + " numbers";
    }
    if(spec.min.empty() || spec.max.empty())
      return where + "numeric type '" + spec.type
             + "' requires both @min and @max (or @range)";
    if(spec.min.size() == 1 && n > 1)
    {
      const float m = spec.min[0];
      spec.min.assign(n, m);
    }
    if(spec.max.size() == 1 && n > 1)
    {
      const float m = spec.max[0];
      spec.max.assign(n, m);
    }
    if(static_cast<int>(spec.min.size()) != n || static_cast<int>(spec.max.size()) != n)
      return where + "@min and @max expect 1 or " + std::to_string(n) + " numbers";
    for(int i = 0; i < n; ++i)
      if(spec.min[i] > spec.max[i])
        return where + "@min is greater than @max";
  }
  else if(spec.type == "string" && !range.empty())
  {
    for(const auto& v : range)
      spec.enumeration.push_back(to_string(v));
  }
  else if(!spec.min.empty() || !spec.max.empty() || !range.empty())
  {
    return where + "@min, @max and @range do not apply to type '" + spec.type + "'";
  }

  if(!spec.unit.empty())
  {
    const auto unit = ossia::parse_pretty_unit(spec.unit);
    if(!unit)
      return where + "unknown unit '" + spec.unit + "'";
    // A unit imposes the type of its values: refuse a declared type that the
    // unit would silently change.
    const auto required = ossia::matching_type(unit);
    if(required != ossia::val_type::IMPULSE && required != type_of(spec.type))
      return where + "unit '" + spec.unit + "' requires type '"
             + std::string(type_name(required)) + "', not '" + spec.type + "'";
  }

  out = std::move(spec);
  return {};
}

std::string scalars_to_value(
    const ossia::net::parameter_base& param, const scalars& in, ossia::value& out)
{
  switch(param.get_value_type())
  {
    case ossia::val_type::IMPULSE:
      out = ossia::impulse{};
      return {};

    case ossia::val_type::FLOAT:
      if(in.size() != 1 || !is_number(in[0]))
        return "expects one number";
      out = to_float(in[0]);
      return {};

    case ossia::val_type::INT:
      if(in.size() != 1 || !is_number(in[0]))
        return "expects one number";
      out = static_cast<int>(std::lround(to_float(in[0])));
      return {};

    case ossia::val_type::BOOL:
      if(in.size() != 1 || !is_number(in[0]))
        return "expects 0 or 1";
      out = to_float(in[0]) != 0.f;
      return {};

    case ossia::val_type::STRING:
      if(in.size() != 1)
        return "expects one symbol";
      out = to_string(in[0]);
      return {};

    case ossia::val_type::LIST: {
      std::vector<ossia::value> v;
      v.reserve(in.size());
      for(const auto& s : in)
        v.push_back(to_value(s));
      out = std::move(v);
      return {};
    }

    case ossia::val_type::VEC2F:
      return to_vec<2>(in, out);
    case ossia::val_type::VEC3F:
      return to_vec<3>(in, out);
    case ossia::val_type::VEC4F:
      return to_vec<4>(in, out);

    default:
      return "unsupported parameter type";
  }
}

branch::branch() = default;

branch::~branch()
{
  detach();
  for(auto& b : m_bindings)
    release(b);
  for(auto* dev : m_devices)
    unwatch_device(*dev, true);
}

void branch::set_node_factory(std::function<ossia::net::node_base*()> factory)
{
  m_node_factory = std::move(factory);
}

void branch::reattach()
{
  ensure_node();
}

// A node is only asked for when there is a leaf to put under it: a branch that
// only binds creates nothing, like an ossia.remote.
void branch::ensure_node()
{
  if(m_node || m_leaves.empty() || !m_node_factory)
    return;
  if(auto* node = m_node_factory())
    attach(*node);
}

void branch::attach(ossia::net::node_base& node)
{
  detach();
  m_node = &node;
  m_node->about_to_be_deleted.connect<&branch::on_node_removing>(this);
  if(m_owns_node)
  {
    for(auto& l : m_leaves)
      create(l);
    return;
  }
  // In a shared node, an address may have been taken while the branch was
  // away: that leaf is refused and forgotten, as a new declaration would be.
  for(std::size_t i = 0; i < m_leaves.size();)
  {
    if(auto err = create(m_leaves[i]); !err.empty())
    {
      refuse(m_leaves[i].spec.address, err);
      m_leaves.erase(m_leaves.begin() + i);
      reindex_leaves();
      m_attach_refusal = std::move(err);
    }
    else
      ++i;
  }
}

void branch::detach()
{
  if(!m_node)
    return;
  if(!m_owns_node)
    for(auto& l : m_leaves)
      destroy(l);
  m_node->about_to_be_deleted.disconnect<&branch::on_node_removing>(this);
  for(auto& l : m_leaves)
  {
    if(l.node)
      l.node->about_to_be_deleted.disconnect<&branch::on_node_removing>(this);
    l.node = nullptr;
    l.param = nullptr;
    l.created = nullptr;
  }
  m_node = nullptr;
}

branch::leaf* branch::find(std::string_view address)
{
  auto it = m_leaf_index.find(address);
  return it == m_leaf_index.end() ? nullptr : &m_leaves[it->second];
}

void branch::reindex_leaves()
{
  m_leaf_index.clear();
  for(std::size_t i = 0; i < m_leaves.size(); i++)
    m_leaf_index.emplace(m_leaves[i].spec.address, i);
}

void branch::reindex_bindings()
{
  m_binding_index.clear();
  m_binding_by_name.clear();
  m_pattern_bindings.clear();
  for(std::size_t i = 0; i < m_bindings.size(); i++)
    index_binding(i);
}

void branch::index_binding(std::size_t i)
{
  const std::string& a = m_bindings[i].address;
  m_binding_index.emplace(a, i);
  if(ossia::traversal::is_pattern(a))
  {
    m_pattern_bindings.push_back(i);
    return;
  }
  // "dev:/a/b", "/a/b" or "a/b": the name of the node is "b"
  std::string_view name = a;
  if(auto slash = name.find_last_of('/'); slash != std::string_view::npos)
    name.remove_prefix(slash + 1);
  else if(auto colon = name.find(':'); colon != std::string_view::npos)
    name.remove_prefix(colon + 1);
  m_binding_by_name[std::string(name)].push_back(i);
}

std::string branch::add(const leaf_spec& spec)
{
  auto refused = [&](std::string err) {
    refuse(spec.address, err);
    return err;
  };
  if(auto* l = find(spec.address))
    return l->spec == spec
               ? std::string{}
               : refused(spec.address + ": already declared with other attributes");
  if(find_binding(spec.address))
    return refused(spec.address + ": already used by bind");
  if(m_node)
  {
    if(auto* existing = ossia::net::find_node(*m_node, spec.address))
      if(existing->get_parameter())
        return refused(
            m_owns_node ? spec.address + ": a parameter already exists at this address"
                        : std::string("already exists"));
  }
  m_leaves.push_back(leaf{spec, nullptr, nullptr});
  m_leaf_index.emplace(spec.address, m_leaves.size() - 1);
  if(m_node)
  {
    if(auto err = create(m_leaves.back()); !err.empty())
    {
      m_leaves.pop_back();
      m_leaf_index.erase(spec.address);
      return refused(err);
    }
  }
  else
  {
    // First leaf: the node is created now, and attach creates the leaf. In a
    // shared node, attach refuses it if its address is taken.
    m_attach_refusal.clear();
    ensure_node();
    if(!m_owns_node && m_node && !find(spec.address))
      return m_attach_refusal;
  }
  return {};
}

// The node of a leaf in a shared node: the existing node at the address, or a
// new one; created is set to the topmost node created on the way.
static ossia::net::node_base* shared_leaf_node(
    ossia::net::node_base& root, std::string_view address,
    ossia::net::node_base*& created)
{
  created = nullptr;
  auto* parent = &root;
  while(!address.empty())
  {
    const auto slash = address.find('/');
    const auto name = address.substr(0, slash);
    auto* child = parent->find_child(name);
    if(!child)
    {
      auto& node = ossia::net::find_or_create_node(*parent, address);
      created = parent->find_child(name);
      return &node;
    }
    parent = child;
    address = slash == std::string_view::npos ? std::string_view{}
                                              : address.substr(slash + 1);
  }
  return parent;
}

std::string branch::create(leaf& l)
{
  const auto& spec = l.spec;
  ossia::net::node_base* created{};
  auto& node = m_owns_node ? ossia::net::find_or_create_node(*m_node, spec.address)
                           : *shared_leaf_node(*m_node, spec.address, created);
  if(node.get_parameter())
    return m_owns_node ? spec.address + ": a parameter already exists at this address"
                       : std::string("already exists");

  auto* param = ossia::try_setup_parameter(spec.type, node);
  if(!param)
    return spec.address + ": cannot create a parameter of type '" + spec.type + "'";

  const auto type = param->get_value_type();
  if(!spec.min.empty() && !spec.max.empty())
  {
    std::vector<ossia::value> mn, mx;
    for(std::size_t i = 0; i < spec.min.size(); ++i)
    {
      if(type == ossia::val_type::INT)
      {
        mn.emplace_back(static_cast<int>(std::lround(spec.min[i])));
        mx.emplace_back(static_cast<int>(std::lround(spec.max[i])));
      }
      else
      {
        mn.emplace_back(spec.min[i]);
        mx.emplace_back(spec.max[i]);
      }
    }
    param->set_domain(ossia::make_domain_from_minmax(mn, mx, type));
  }
  else if(!spec.enumeration.empty())
  {
    param->set_domain(ossia::make_domain(spec.enumeration));
  }

  param->set_bounding(to_bounding(spec.clip));
  param->set_access(to_access(spec.mode));
  if(!spec.unit.empty())
    param->set_unit(ossia::parse_pretty_unit(spec.unit));
  if(!spec.description.empty())
    ossia::net::set_description(node, spec.description.c_str());
  if(!spec.tags.empty())
    ossia::net::set_tags(node, spec.tags);
  if(spec.priority)
    ossia::net::set_priority(node, *spec.priority);
  if(spec.rate)
    ossia::net::set_refresh_rate(node, *spec.rate);
  param->set_critical(spec.critical);
  if(spec.mute)
    ossia::net::set_muted(node, true);
  param->set_repetition_filter(
      spec.repetitions ? ossia::repetition_filter::OFF : ossia::repetition_filter::ON);
  if(!spec.enable)
    ossia::net::set_disabled(node, true);
  if(spec.invisible)
    ossia::net::set_hidden(node, true);
  if(spec.recall_safe)
    ossia::net::set_recall_safe(node, true);

  if(spec.default_value)
  {
    ossia::value v;
    if(auto err = scalars_to_value(*param, *spec.default_value, v); !err.empty())
    {
      node.remove_parameter();
      return spec.address + ": invalid @default: " + err;
    }
    ossia::net::set_default_value(node, v);
    // Same as ossia.parameter at registration: the value is stored and sent
    // to the network, the local callbacks are not fired. The host then outputs
    // it locally, as ossia-max outputs the defaults of its objects after they
    // registered.
    param->push_value_quiet(v);
    on_value(spec.address, param, v);
  }

  const std::string address = spec.address;
  param->add_callback(
      [this, address, param](const ossia::value& v) { on_value(address, param, v); });
  node.about_to_be_deleted.connect<&branch::on_node_removing>(this);

  l.node = &node;
  l.param = param;
  l.created = created;
  return {};
}

void branch::destroy(leaf& l)
{
  if(!l.node || !m_node)
  {
    l.node = nullptr;
    l.param = nullptr;
    l.created = nullptr;
    return;
  }
  auto* node = l.node;
  node->about_to_be_deleted.disconnect<&branch::on_node_removing>(this);
  l.node = nullptr;
  l.param = nullptr;

  if(!m_owns_node)
  {
    // Remove only what the leaf created: a node that existed keeps everything
    // but the parameter, and the removal stops at the topmost created node.
    auto* top = std::exchange(l.created, nullptr);
    if(!top || !node->children().empty())
    {
      node->remove_parameter();
      return;
    }
    auto* parent = node->get_parent();
    while(parent)
    {
      const bool last = node == top;
      auto* grandparent = parent->get_parent();
      parent->remove_child(*node);
      if(last || !parent->children().empty() || parent->get_parameter())
        break;
      node = parent;
      parent = grandparent;
    }
    return;
  }

  // A leaf that holds other leaves stays as a container.
  if(!node->children().empty())
  {
    node->remove_parameter();
    return;
  }

  // Remove the leaf, then the intermediate nodes it leaves empty, up to the
  // branch node itself.
  auto* parent = node->get_parent();
  while(parent)
  {
    auto* grandparent = parent->get_parent();
    parent->remove_child(*node);
    if(parent == m_node || !parent->children().empty() || parent->get_parameter())
      break;
    node = parent;
    parent = grandparent;
  }
}

std::string
branch::declare(std::string_view address, const std::vector<attribute>& attrs)
{
  leaf_spec spec;
  if(auto err = make_leaf_spec(address, attrs, spec); !err.empty())
  {
    refuse(address, err);
    return err;
  }
  return add(spec);
}

void branch::refuse(std::string_view address, std::string_view reason)
{
  {
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    m_queue.push_back(
        {"refused",
         ossia::value(
             std::vector<ossia::value>{std::string(address), std::string(reason)}),
         nullptr});
  }
  if(m_wake)
    m_wake();
}

bool branch::remove(std::string_view address)
{
  const auto a = normalize_relative_address(address);
  auto* l = a.empty() ? nullptr : find(a);
  if(!l)
  {
    refuse(address, "unknown address '" + std::string(address) + "'");
    return false;
  }
  destroy(*l);
  m_leaves.erase(m_leaves.begin() + (l - m_leaves.data()));
  reindex_leaves();
  return true;
}

void branch::clear()
{
  for(auto& l : m_leaves)
    destroy(l);
  m_leaves.clear();
  m_leaf_index.clear();
}

std::string branch::set(std::string_view address, const scalars& values)
{
  auto err = set_impl(address, values);
  if(!err.empty())
    refuse(address, err);
  return err;
}

std::string branch::set_impl(std::string_view address, const scalars& values)
{
  const auto a = normalize_relative_address(address);
  if(auto* l = a.empty() ? nullptr : find(a))
  {
    if(!l->param)
      return a + ": not attached to a device yet";

    ossia::value v;
    if(auto err = scalars_to_value(*l->param, values, v); !err.empty())
      return a + ": " + err;

    pushing_scope guard{this, &values};
    // Stored as sent, as ossia.parameter does; bounded on output.
    push(*l->param, std::move(v));
    return {};
  }

  if(auto* b = find_binding(address))
  {
    if(b->targets.empty())
      return b->address + ": bound parameter not found yet";
    pushing_scope guard{this, &values};
    for(auto& t : b->targets)
    {
      ossia::value v;
      if(auto err = scalars_to_value(*t.param, values, v); !err.empty())
        return b->address + ": " + err;
      push(*t.param, std::move(v));
    }
    return {};
  }

  return "unknown address '" + std::string(address) + "'";
}

bool branch::push(ossia::net::parameter_base& param, ossia::value&& v)
{
  if(!m_bundle)
  {
    param.push_value(std::move(v));
    return true;
  }
  // Only what changes leaves, as the parameter's repetition filter says.
  if(param.get_repetition_filter() == ossia::repetition_filter::ON && param.value() == v)
    return false;
  param.set_value(std::move(v));
  if(m_outgoing_set.insert(&param).second)
  {
    m_outgoing.push_back(&param);
    if(m_outgoing.size() == 1 && m_wake)
      m_wake();
  }
  return true;
}

void branch::set_bundle(bool on)
{
  if(m_bundle && !on)
    flush();
  m_bundle = on;
}

namespace
{
constexpr std::size_t osc_bundle_header_size = 16; // "#bundle\0" and the time tag
constexpr std::size_t osc_udp_bundle_limit = 1400;
}

std::size_t branch::flush()
{
  if(m_outgoing.empty())
    return 0;
  std::vector<ossia::net::parameter_base*> queued;
  queued.swap(m_outgoing);
  m_outgoing_set.clear();

  // Only the parameters this branch still holds: a leaf removed or a bound
  // parameter gone since its set is not sent.
  std::unordered_set<const ossia::net::parameter_base*> live;
  for(const auto& l : m_leaves)
    if(l.param)
      live.insert(l.param);
  for(const auto& b : m_bindings)
    for(const auto& t : b.targets)
      if(t.param)
        live.insert(t.param);

  // One group per protocol, in the order of the first set.
  std::vector<std::pair<
      ossia::net::protocol_base*, std::vector<const ossia::net::parameter_base*>>>
      groups;
  std::size_t sent = 0;
  for(auto* p : queued)
  {
    if(!live.count(p))
      continue;
    auto* proto = &p->get_node().get_device().get_protocol();
    auto it = std::find_if(groups.begin(), groups.end(), [proto](const auto& g) {
      return g.first == proto;
    });
    if(it == groups.end())
    {
      groups.push_back({proto, {}});
      it = groups.end() - 1;
    }
    it->second.push_back(p);
    sent++;
  }

  for(auto& [proto, params] : groups)
  {
    // A client's mirror splits its bundles itself, by transport.
    if(dynamic_cast<ossia::oscquery::oscquery_mirror_protocol*>(proto))
    {
      proto->push_bundle(params);
      continue;
    }
    // Other protocols send what they are given as one bundle: groups of at
    // most 1400 bytes of messages.
    std::vector<const ossia::net::parameter_base*> group;
    std::size_t size = osc_bundle_header_size;
    for(auto* p : params)
    {
      const auto message
          = 4 + ossia::oscquery::osc_writer::to_message(*p, p->value()).size();
      if(!group.empty() && size + message > osc_udp_bundle_limit)
      {
        proto->push_bundle(group);
        group.clear();
        size = osc_bundle_header_size;
      }
      group.push_back(p);
      size += message;
    }
    if(!group.empty())
      proto->push_bundle(group);
  }
  return sent;
}

std::vector<std::string> branch::addresses() const
{
  std::vector<std::string> out;
  out.reserve(m_leaves.size());
  for(const auto& l : m_leaves)
    out.push_back(l.spec.address);
  return out;
}

void branch::set_wake_callback(std::function<void()> cb)
{
  m_wake = std::move(cb);
}

std::vector<inbound> branch::drain()
{
  std::vector<queued> q;
  {
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    q.swap(m_queue);
  }
  std::vector<inbound> out;
  out.reserve(q.size());
  for(auto& e : q)
  {
    if(e.read_from)
    {
      // Only read a parameter this branch still follows.
      bool live = false;
      for(const auto& b : m_bindings)
        for(const auto& t : b.targets)
          live = live || t.param == e.read_from;
      if(!live || !e.read_from->value().valid())
        continue;
      auto bounded = ossia::bound_value(
          e.read_from->get_domain(), e.read_from->value(), e.read_from->get_bounding());
      if(e.read_from->filter_value(bounded) || !bounded.valid())
        continue;
      out.push_back({std::move(e.address), std::move(bounded)});
    }
    else
      out.push_back({std::move(e.address), std::move(e.value)});
  }
  return out;
}

void branch::queue_current_value(
    const std::string& address, ossia::net::parameter_base* param)
{
  {
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    m_queue.push_back({address, {}, param});
  }
  if(m_wake)
    m_wake();
}

void branch::on_value(
    const std::string& address, ossia::net::parameter_base* param, const ossia::value& v)
{
  // Same sequence as the outlet of every ossia-max object: the value is
  // bounded to the parameter's range, then the parameter filters it
  // (disabled, muted, or a repetition when the repetition filter is on). The
  // parameter itself stores what it was given.
  ossia::value bounded
      = param ? ossia::bound_value(param->get_domain(), v, param->get_bounding()) : v;
  if(!bounded.valid())
    return;
  if(param && param->filter_value(bounded))
    return;
  // The branch's own push comes back only if the value that leaves differs
  // from the value sent (clip, type conversion): the host then learns it.
  if(t_pushing == this && t_pushing_request
     && scalars_match(*t_pushing_request, bounded))
    return;
  {
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    m_queue.push_back({address, std::move(bounded), nullptr});
  }
  if(m_wake)
    m_wake();
}

namespace
{
std::string trim_address(std::string_view address)
{
  while(!address.empty() && std::isspace(static_cast<unsigned char>(address.front())))
    address.remove_prefix(1);
  while(!address.empty() && std::isspace(static_cast<unsigned char>(address.back())))
    address.remove_suffix(1);
  return std::string(address);
}

// Address of a node relative to a root node, without leading slash.
std::string
relative_to(const ossia::net::node_base& root, const ossia::net::node_base& node)
{
  const auto full = node.osc_address();
  auto base = root.osc_address();
  if(base == "/")
    return full.substr(1);
  if(full.rfind(base + "/", 0) == 0)
    return full.substr(base.size() + 1);
  return full;
}
}

branch::binding* branch::find_binding(std::string_view address)
{
  auto it = m_binding_index.find(trim_address(address));
  return it == m_binding_index.end() ? nullptr : &m_bindings[it->second];
}

std::string branch::bind(std::string_view address)
{
  const auto a = trim_address(address);
  if(a.empty())
  {
    refuse(address, "bind expects an address");
    return "bind expects an address";
  }
  if(find_binding(a))
    return {};
  if(find(normalize_relative_address(a)))
  {
    refuse(a, a + ": already declared by add");
    return a + ": already declared by add";
  }
  m_bindings.push_back(binding{a, {}});
  index_binding(m_bindings.size() - 1);
  resolve(m_bindings.back());
  if(m_binding_changed)
    m_binding_changed(a, m_bindings.back().targets.size());
  return {};
}

bool branch::unbind(std::string_view address)
{
  auto* b = find_binding(address);
  if(!b)
  {
    refuse(address, "unknown address '" + std::string(address) + "'");
    return false;
  }
  release(*b);
  m_bindings.erase(m_bindings.begin() + (b - m_bindings.data()));
  reindex_bindings();
  return true;
}

std::size_t branch::bound_count(std::string_view address) const
{
  const auto a = trim_address(address);
  for(const auto& b : m_bindings)
    if(b.address == a)
      return b.targets.size();
  return 0;
}

void branch::release(binding& b)
{
  for(auto& t : b.targets)
  {
    if(t.node)
      t.node->about_to_be_deleted.disconnect<&branch::on_node_removing>(this);
    if(t.param)
      t.param->remove_callback(t.callback);
  }
  b.targets.clear();
}

void branch::set_resolution_context(
    std::vector<ossia::net::node_base*> parents,
    std::vector<ossia::net::device_base*> devices)
{
  std::vector<std::size_t> before;
  before.reserve(m_bindings.size());
  for(const auto& b : m_bindings)
    before.push_back(b.targets.size());

  for(auto* dev : m_devices)
    unwatch_device(*dev, true);
  m_parents = std::move(parents);
  m_parent_devices.clear();
  for(auto* n : m_parents)
    m_parent_devices.push_back(&n->get_device());
  m_devices = std::move(devices);
  for(auto* dev : m_devices)
    watch_device(*dev);

  // A target on a device that left the context is released: a client that
  // connected to another device, the old one kept alive elsewhere (the
  // zeroconf listener keeps its mirrors), must not leave a binding on it,
  // which would receive and send every value a second time.
  const auto in_context = [&](const ossia::net::device_base* dev) {
    return std::find(m_parent_devices.begin(), m_parent_devices.end(), dev)
               != m_parent_devices.end()
           || std::find(m_devices.begin(), m_devices.end(), dev) != m_devices.end();
  };
  for(auto& b : m_bindings)
  {
    for(auto& t : b.targets)
    {
      if(in_context(&t.node->get_device()))
        continue;
      t.node->about_to_be_deleted.disconnect<&branch::on_node_removing>(this);
      t.param->remove_callback(t.callback);
      t.node = nullptr;
    }
    std::erase_if(b.targets, [](const target& t) { return t.node == nullptr; });
  }

  for(std::size_t i = 0; i < m_bindings.size(); i++)
  {
    auto& b = m_bindings[i];
    resolve(b);
    if(b.targets.size() != before[i] && m_binding_changed)
      m_binding_changed(b.address, b.targets.size());
  }
}

// The branch listens to the destruction of the root of every device it knows,
// so that it never keeps a device the host forgot to drop: ossia-max's client
// clears its device before it announces the removal.
void branch::watch_device(ossia::net::device_base& dev)
{
  dev.on_parameter_created.connect<&branch::on_parameter_created>(this);
  dev.get_root_node().about_to_be_deleted.connect<&branch::on_node_removing>(this);
}

void branch::unwatch_device(ossia::net::device_base& dev, bool root_alive)
{
  dev.on_parameter_created.disconnect<&branch::on_parameter_created>(this);
  if(root_alive)
    dev.get_root_node().about_to_be_deleted.disconnect<&branch::on_node_removing>(this);
}

void branch::drop_device(ossia::net::device_base* dev)
{
  forget_device(dev, true);
}

void branch::forget_device(ossia::net::device_base* dev, bool root_alive)
{
  auto it = std::find(m_devices.begin(), m_devices.end(), dev);
  if(it == m_devices.end())
    return;
  unwatch_device(*dev, root_alive);
  m_devices.erase(it);
  // The parents of a device being destroyed may already be gone: their device
  // was noted when they were given.
  for(std::size_t i = m_parents.size(); i-- > 0;)
  {
    if(m_parent_devices[i] == dev)
    {
      m_parents.erase(m_parents.begin() + i);
      m_parent_devices.erase(m_parent_devices.begin() + i);
    }
  }
  for(auto& b : m_bindings)
  {
    const auto before = b.targets.size();
    for(auto& t : b.targets)
    {
      if(&t.node->get_device() == dev)
      {
        t.node->about_to_be_deleted.disconnect<&branch::on_node_removing>(this);
        t.param->remove_callback(t.callback);
        t.node = nullptr;
      }
    }
    std::erase_if(b.targets, [](const target& t) { return t.node == nullptr; });
    if(b.targets.size() != before && m_binding_changed)
      m_binding_changed(b.address, b.targets.size());
  }
}

void branch::set_binding_changed_callback(
    std::function<void(const std::string&, std::size_t)> cb)
{
  m_binding_changed = std::move(cb);
}

void branch::resolve_bindings()
{
  for(auto& b : m_bindings)
  {
    const auto before = b.targets.size();
    resolve(b);
    if(b.targets.size() != before && m_binding_changed)
      m_binding_changed(b.address, b.targets.size());
  }
}

void branch::resolve(binding& b)
{
  // Candidate (root, node) pairs for the address, with the rules of ossia.remote.
  std::vector<std::pair<ossia::net::node_base*, ossia::net::node_base*>> found;
  std::string_view address = b.address;
  const auto scope = ossia::net::get_address_scope(address);
  const bool pattern = ossia::traversal::is_pattern(address);

  auto collect = [&](ossia::net::node_base& root, std::string_view addr) {
    if(pattern)
    {
      for(auto* n : ossia::net::find_nodes(root, addr))
        found.emplace_back(&root, n);
    }
    else if(auto* n = ossia::net::find_node(root, addr))
      found.emplace_back(&root, n);
  };

  switch(scope)
  {
    case ossia::net::address_scope::relative: {
      if(address.rfind("//", 0) == 0)
        address.remove_prefix(2);
      for(auto* p : m_parents)
        collect(*p, address);
      break;
    }
    case ossia::net::address_scope::absolute: {
      if(!m_parents.empty())
        collect(m_parents.front()->get_device().get_root_node(), address);
      break;
    }
    case ossia::net::address_scope::global: {
      const auto pos = address.find(":/");
      const auto name = address.substr(0, pos);
      const auto osc = address.substr(pos + 1);
      for(auto* dev : m_devices)
        if(dev->get_name() == name)
          collect(dev->get_root_node(), osc);
      break;
    }
  }

  for(auto [root, node] : found)
  {
    auto* param = node->get_parameter();
    if(!param)
      continue;
    const bool already
        = std::any_of(b.targets.begin(), b.targets.end(), [node](const target& t) {
      return t.node == node;
    });
    if(already)
      continue;

    target t;
    t.node = node;
    t.param = param;
    if(!pattern)
      t.out_address = b.address;
    else if(scope == ossia::net::address_scope::global)
      t.out_address = std::string(b.address.substr(0, b.address.find(":/"))) + ":"
                      + node->osc_address();
    else if(scope == ossia::net::address_scope::absolute)
      t.out_address = node->osc_address();
    else
      t.out_address = relative_to(*root, *node);

    const std::string out = t.out_address;
    t.callback = param->add_callback(
        [this, out, param](const ossia::value& v) { on_value(out, param, v); });
    node->about_to_be_deleted.connect<&branch::on_node_removing>(this);
    b.targets.push_back(std::move(t));

    // The value the parameter holds when the subscription starts, read when
    // the host drains, as an ossia.remote outputs the value it finds.
    queue_current_value(out, param);
  }
}

// Only the bindings the new parameter can satisfy are resolved again: those
// with a pattern, and those still waiting whose address ends with the name
// of its node. Resolving every binding at each creation cost, for 3000
// bindings waiting while 3000 parameters appear, 15 s in one poll.
void branch::on_parameter_created(const ossia::net::parameter_base& p)
{
  auto again = [this](std::size_t i) {
    auto& b = m_bindings[i];
    const auto before = b.targets.size();
    resolve(b);
    if(b.targets.size() != before && m_binding_changed)
      m_binding_changed(b.address, b.targets.size());
  };
  if(auto it = m_binding_by_name.find(p.get_node().get_name());
     it != m_binding_by_name.end())
  {
    const auto candidates = it->second;
    for(auto i : candidates)
      if(i < m_bindings.size() && m_bindings[i].targets.empty())
        again(i);
  }
  const auto patterns = m_pattern_bindings;
  for(auto i : patterns)
    if(i < m_bindings.size())
      again(i);
}

void branch::on_node_removing(const ossia::net::node_base& n)
{
  for(auto& b : m_bindings)
  {
    const auto before = b.targets.size();
    std::erase_if(b.targets, [&n](const target& t) { return t.node == &n; });
    if(b.targets.size() != before && m_binding_changed)
      m_binding_changed(b.address, b.targets.size());
  }

  // A node without parent is the root of its device: the device is being
  // destroyed, its children are already gone. Forget it without touching the
  // signal being emitted nor calling the device, which is half destroyed. The
  // root may also be the node of a branch without name; this signal may then
  // fire twice for it, the second time with nothing left to do.
  const bool root = !n.get_parent();

  // The node is being destroyed: forget it without touching its signals.
  if(&n == m_node)
  {
    for(auto& l : m_leaves)
    {
      l.node = nullptr;
      l.param = nullptr;
      l.created = nullptr;
    }
    m_node = nullptr;
    if(root)
      forget_device(&n.get_device(), false);
    if(m_node_lost)
      m_node_lost();
    return;
  }
  if(root)
  {
    forget_device(&n.get_device(), false);
    return;
  }
  for(auto& l : m_leaves)
  {
    if(l.node == &n)
    {
      l.node = nullptr;
      l.param = nullptr;
      l.created = nullptr;
    }
  }
}

void branch::set_node_lost_callback(std::function<void()> cb)
{
  m_node_lost = std::move(cb);
}

}
}
}
