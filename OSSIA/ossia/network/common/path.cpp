// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/common/path.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <hopscotch_set.h>

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

    for (auto node : sub)
      vec.push_back(node);

    sub.clear();
  }
}
void get_all_children(std::vector<ossia::net::node_base*>& vec)
{
  tsl::hopscotch_set<ossia::net::node_base*> inserted;
  get_all_children_rec(vec, inserted);
}

void match_with_regex(
    std::vector<ossia::net::node_base*>& vec, const std::regex& r)
{
  std::vector<ossia::net::node_base*> old = std::move(vec);

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

void add_relative_path(
    std::string& part, path& p)
{
  if (part != "..")
  {
    {
      auto& map = regex_cache::instance();
      std::lock_guard<std::mutex> _(map.mutex);

      auto it = map.map.find(part);
      if(it != map.map.end())
      {
        p.child_functions.push_back([r=it->second](auto& v) { match_with_regex(v, r); });
      }
      else
      {
        std::string orig = part;

        // Perform the various regex-like replacements
        // note: seriously, don't do this with regex if possible
        net::expand_ranges(part);
        std::string res = "^";
        res.reserve(part.size() + 16);

        static const auto qmark = "[" + std::string(ossia::net::name_characters()) + "]?";
        static const auto smark = "[" + std::string(ossia::net::name_characters()) + "]*";
        for(std::size_t i = 0, N = part.size(); i < N; i++)
        {
          if(part[i] == '(') res.append("\\(");
          else if(part[i] == ')') res.append("\\)");
          else if(part[i] == '{') res.append("(");
          else if(part[i] == '}') res.append(")");
          else if(part[i] == ',') res.append("|");
          else if(part[i] == '?') res.append(qmark);
          else if(part[i] == '*') res.append(smark);
          else if(part[i] == '!') res.append(regex_path::any_instance::instance_regex());
          else res += part[i];
        }
        res += "$";

        std::regex r(res);
        p.child_functions.push_back([=](auto& v) { match_with_regex(v, r); });
        map.map.insert(std::make_pair(std::move(orig), std::move(r)));
      }
    }
  }
  else
  {
    p.child_functions.push_back([] (auto& x) { return get_parent(x); });
  }
}

bool is_pattern(ossia::string_view address)
{
  if(boost::starts_with(address, "//"))
    return true;
  static const auto pred = boost::is_any_of("?*[]{}!");
  for(char c : address)
  {
    if(pred(c))
      return true;
  }

  return false;
}

ossia::optional<path> make_path(ossia::string_view address) try
{
  path p{std::string(address), {}};

  if(address.empty())
  {
    return p;
  }

  const bool starts_any = boost::starts_with(p.pattern, "//");
  if (!starts_any)
  {
    // Split on "/"
    auto parts = ossia::net::address_parts(p.pattern);

    // Potentially remove first ":".
    if (!parts[0].empty() && parts[0].back() == ':')
      parts[0].resize(parts[0].size() - 1);

    for (auto part : parts)
    {
      add_relative_path(part, p);
    }
  }
  else
  {
    // Means that we can start at any point, of any node.
    //* means all the nodes.
    // We have to pass an array with all the devices ?
    // We can't just make a big regex because of '..'
    p.child_functions.push_back([] (auto& x) { return get_all_children(x); });

    // Split on "/"
    auto parts
        = ossia::net::address_parts(ossia::string_view(p.pattern).substr(2));

    for (auto part : parts)
    {
      add_relative_path(part, p);
    }
  }

  return p;
}
catch (...)
{
  return ossia::none;
}

bool match(const path& p, const ossia::net::node_base& node)
{
  return match(p, node, node.get_device().get_root_node());
}

bool match(const path& p, const ossia::net::node_base& node, ossia::net::node_base& root)
{
  std::vector<ossia::net::node_base*> vec{&root};
  apply(p, vec);
  return ossia::find(vec, &node) != std::end(vec);
}

}
}
