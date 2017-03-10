#include <ossia/network/common/path.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/address.hpp>
#include <hopscotch_set.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace ossia
{

namespace traversal
{

void apply(
    const path& p,
    std::vector<ossia::net::node_base*>& nodes)
{
  for(auto fun : p.functions)
  {
    fun(nodes);
  }
}


void get_parent(
    std::vector<ossia::net::node_base*>& vec)
{
  std::vector<ossia::net::node_base*> old = std::move(vec);

  tsl::hopscotch_set<ossia::net::node_base*> inserted;
  for(auto ptr : old)
  {
    if(auto par = ptr->getParent())
    {
      auto it = inserted.find(par);
      if(it == inserted.end())
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

  for(const ossia::net::node_base* node : source)
  {
    const auto n = node->children().size();
    vec.reserve(vec.size() + n);
    sub.reserve(n);

    for(auto& c : node->children())
    {
      auto ptr = c.get();
      auto it = inserted.find(ptr);
      if(it == inserted.end())
        sub.push_back(ptr);
    }

    get_all_children_rec(sub, inserted);

    for(auto node : sub)
      vec.push_back(node);

    sub.clear();
  }
}
void get_all_children(
    std::vector<ossia::net::node_base*>& vec)
{
  tsl::hopscotch_set<ossia::net::node_base*> inserted;
  get_all_children_rec(vec, inserted);
}

void match_with_regex(
    std::vector<ossia::net::node_base*>& vec,
    const std::regex& r)
{
  std::vector<ossia::net::node_base*> old = std::move(vec);

  for(ossia::net::node_base* node : old)
  {
    for(auto& cld : node->children())
    {
      if(std::regex_match(cld->getName(), r))
      {
        vec.push_back(cld.get());
      }
    }
  }
}
void add_relative_path(
    std::string& part,
    path& p,
    const std::string& ossia_chars)
{
  if(part != "..")
  {
    // Perform the various regex-like replacements
    boost::replace_all(part, "(", "\\(");
    boost::replace_all(part, ")", "\\)");
    boost::replace_all(part, "{", "(");
    boost::replace_all(part, "}", ")");
    boost::replace_all(part, ",", "|");

    boost::replace_all(part, "?", "[" + ossia_chars + "]?");
    boost::replace_all(part, "*", "[" + ossia_chars + "]*");

    std::regex r(part);
    p.functions.push_back([=] (auto& v) {
      match_with_regex(v, r);
    });

  }
  else
  {
    p.functions.push_back(get_parent);
  }

}

ossia::optional<path> make_path(const std::string& address)
try
{
  path p;

  bool starts_any = boost::starts_with(address, "//");
  const std::string ossia_chars = ossia::net::name_characters().to_string();
  if(!starts_any)
  {
    // Split on "/"
    auto parts = ossia::net::address_parts(address);

    // Potentially remove first ":".
    if(parts[0].back() == ':')
      parts[0].resize(parts[0].size() - 1);

    for(auto part : parts)
    {
      add_relative_path(part, p, ossia_chars);
    }
  }
  else
  {
    // Means that we can start at any point, of any node.
    //* means all the nodes.
    // We have to pass an array with all the devices ?
    // We can't just make a big regex because of '..'
    p.functions.push_back(get_all_children);

    // Split on "/"
    auto parts = ossia::net::address_parts(ossia::string_view(address).substr(2));

    for(auto part : parts)
    {
      add_relative_path(part, p, ossia_chars);
    }
  }

  return p;
}
catch(...)
{
return ossia::none;
}
}
}
