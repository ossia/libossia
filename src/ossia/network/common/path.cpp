// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/address_scope.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/path.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <ossia/detail/algorithms.hpp>
#include <tsl/hopscotch_set.h>

#include <iostream>
#include <regex>

namespace ossia
{

namespace traversal
{

void apply(const path& p, std::vector<ossia::net::node_base*>& nodes)
{
  for (const auto& fun : p.child_functions)
  {
    fun(nodes);
  }
}

void get_parent(std::vector<ossia::net::node_base*>& vec)
{
  std::vector<ossia::net::node_base*> old = std::move(vec);
  vec.clear();

  tsl::hopscotch_set<ossia::net::node_base*> inserted;
  for (auto ptr : old)
  {
    if (auto par = ptr->get_parent())
    {
      auto it = inserted.find(par);
      if (it == inserted.end())
      {
        vec.push_back(par);
        inserted.insert(par);
      }
    }
  }
}

void get_all_children_rec(
    std::vector<ossia::net::node_base*>& vec,
    tsl::hopscotch_set<ossia::net::node_base*>& inserted)
{
  auto source = vec;
  std::vector<ossia::net::node_base*> sub;

  for (const ossia::net::node_base* node : source)
  {
    const auto n = node->children().size();
    vec.reserve(vec.size() + n);
    sub.reserve(n);

    for (auto& c : node->children())
    {
      auto ptr = c.get();
      auto it = inserted.find(ptr);
      if (it == inserted.end())
        sub.push_back(ptr);
    }

    get_all_children_rec(sub, inserted);

    for (auto child : sub)
      vec.push_back(child);

    sub.clear();
  }
}
void get_all_children(std::vector<ossia::net::node_base*>& vec)
{
  tsl::hopscotch_set<ossia::net::node_base*> inserted;
  get_all_children_rec(vec, inserted);
}

void match_device_with_regex(
    std::vector<ossia::net::node_base*>& vec, const std::regex& r)
{
  for (auto it = vec.cbegin(); it != vec.cend();)
  {
    const auto& name = (*it)->get_device().get_name();
    if (!std::regex_match(name, r))
      it = vec.erase(it);
    else
      ++it;
  }
}
void match_device_simple(
    std::vector<ossia::net::node_base*>& vec, const std::string& r)
{
  for (auto it = vec.cbegin(); it != vec.cend();)
  {
    const auto& name = (*it)->get_device().get_name();
    if (name != r)
      it = vec.erase(it);
    else
      ++it;
  }
}

void match_with_regex(
    std::vector<ossia::net::node_base*>& vec, const std::regex& r)
{
  ossia::small_vector<ossia::net::node_base*, 16> old(vec.begin(), vec.end());
  vec.clear();

  for (ossia::net::node_base* node : old)
  {
    for (auto& cld : node->children())
    {
      if (std::regex_match(cld->get_name(), r))
      {
        vec.push_back(cld.get());
      }
    }
  }
}

void match_simple(
    std::vector<ossia::net::node_base*>& vec, const std::string& r)
{
  ossia::small_vector<ossia::net::node_base*, 16> old(vec.begin(), vec.end());
  vec.clear();

  for (ossia::net::node_base* node : old)
  {
    for (auto& cld : node->children())
    {
      if (cld->get_name() == r)
      {
        vec.push_back(cld.get());
      }
    }
  }
}

struct regex_cache
{
  static regex_cache& instance()
  {
    static regex_cache c;
    return c;
  }

  ossia::string_map<std::regex> map;
  std::mutex mutex;
};

std::string substitute_characters(const std::string& part)
{
  std::string res;
  res.reserve(part.size() + 16);

  static const auto qmark
      = "[" + std::string(ossia::net::name_characters()) + "]?";
  static const auto smark
      = "[" + std::string(ossia::net::name_characters()) + "]*";
  for (std::size_t i = 0, N = part.size(); i < N; i++)
  {
    if (part[i] == '(')
      res.append("\\(");
    else if (part[i] == ')')
      res.append("\\)");
    else if (part[i] == '{')
      res.append("(");
    else if (part[i] == '}')
      res.append(")");
    else if (part[i] == ',')
      res.append("|");
    else if (part[i] == '?')
      res.append(qmark);
    else if (part[i] == '*')
      res.append(smark);
    else if (part[i] == '!')
      res.append(regex_path::any_instance::instance_regex());
    else
      res += part[i];
  }
  return res;
}

std::regex make_regex(std::string& part)
{
  // Perform the various regex-like replacements
  // note: seriously, don't do this with regex if possible
  net::expand_ranges(part);
  std::string res = "^";
  res += substitute_characters(part);
  res += "$";

  return std::regex{res};
}

constexpr bool is_regex(std::string_view v)
{
  for(char c : v)
  {
    if(c == '(' || c == '{' || c == '[' || c == '*' || c == '?' || c == '^' || c == '$' || c == '\\' || c == '/' || c == '.' || c == ',' || c == '!')
    {
      return true;
    }
  }
  return false;
}

void add_device_part(std::string part, path& p)
{
  // TODO LRU cache
  if(!is_regex(part))
  {
    p.child_functions.push_back([=, p = std::move(part)](auto& v) { match_device_simple(v, p); });
  }
  else
  {
    auto& map = regex_cache::instance();
    std::lock_guard<std::mutex> _(map.mutex);

    auto it = map.map.find(part);
    if (it != map.map.end())
    {
      p.child_functions.push_back(
          [r = it->second](auto& v) { match_device_with_regex(v, r); });
    }
    else
    {
      std::string orig = part;

      std::regex r = make_regex(part);
      p.child_functions.push_back([=](auto& v) { match_device_with_regex(v, r); });
      map.map.insert(std::make_pair(std::move(orig), std::move(r)));
    }
  }
}

void add_relative_path(std::string& part, path& p)
{
  using namespace std::literals;
  if (part != ".."sv)
  {
    if(!is_regex(part))
    {
      p.child_functions.push_back([p = std::move(part)](auto& v) { match_simple(v, p); });
    }
    else
    {
      // TODO LRU cache
      auto& map = regex_cache::instance();
      std::lock_guard<std::mutex> _(map.mutex);

      auto it = map.map.find(part);
      if (it != map.map.end())
      {
        p.child_functions.push_back(
            [r = it->second](auto& v) { match_with_regex(v, r); });
      }
      else
      {
        std::string orig = part;

        std::regex r = make_regex(part);

        p.child_functions.push_back([=](auto& v) { match_with_regex(v, r); });
        map.map.insert(std::make_pair(std::move(orig), std::move(r)));
      }
    }
  }
  else
  {
    p.child_functions.push_back([](auto& x) { return get_parent(x); });
  }
}

bool is_pattern(ossia::string_view address)
{
  if (boost::starts_with(address, "//"))
    return true;
  static const auto pred = boost::is_any_of("?*[]{}!");
  for (char c : address)
  {
    if (pred(c))
      return true;
  }

  return false;
}

std::optional<path> make_path(std::string_view address) try
{
  const auto scope = ossia::net::get_address_scope(address);
  path p{std::string(address), scope, {}};

  if (address.empty())
  {
    return p;
  }

  auto add_simple_address = [&](std::string_view address) {
    // Split on "/"
    // TODO is this copy really necessary ?
    for (auto part : ossia::net::address_parts(address))
      add_relative_path(part, p);
  };

  auto add_address = [&](std::string_view address) {
    if (boost::starts_with(address, "//"))
    {
      // Means that we can start at any point, of any node.
      //* means all the nodes.
      // We have to pass an array with all the devices ?
      // We can't just make a big regex because of '..'
      p.child_functions.push_back([](auto& x) { return get_all_children(x); });

      add_simple_address(address.substr(2));
    }
    else
    {
      add_simple_address(address);
    }
  };

  switch (scope)
  {
    case ossia::net::address_scope::relative:
    case ossia::net::address_scope::absolute:
    {
      add_address(address);
      break;
    }

    case ossia::net::address_scope::global:
    {
      // First add a match on the device
      auto start = address.find_first_of('/');

      add_device_part(std::string(address.data(), start - 1), p);
      add_address({address.data() + start, address.size() - start});
    }
  }

  return p;
}
catch (...)
{
  return std::nullopt;
}

bool match(const path& p, const ossia::net::node_base& node)
{
  return match(p, node, node.get_device().get_root_node());
}

bool match(
    const path& p, const ossia::net::node_base& node,
    ossia::net::node_base& root)
{
  std::vector<ossia::net::node_base*> vec{&root};
  apply(p, vec);
  return ossia::find(vec, &node) != std::end(vec);
}

}

std::ostream& regex_path::operator<<(std::ostream& s, const ossia::regex_path::path_element& p)
{
  return s << p.address;
}

}
