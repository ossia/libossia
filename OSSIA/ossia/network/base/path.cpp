#include <ossia/network/base/path.hpp>
#include <ossia/network/base/node.hpp>
#include <hopscotch_map.h>
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


void get_parent(std::vector<ossia::net::node_base*>& vec)
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

void match_with_regex(
    std::vector<ossia::net::node_base*>& vec,
    const std::regex& r)
{
  std::vector<ossia::net::node_base*> old = std::move(vec);

  for(ossia::net::node_base* node : old)
  {
    for(auto& cld : node->children())
      if(std::regex_match(cld->getName(), r))
        vec.push_back(cld.get());
  }
}

path make_path(const std::string& address)
{
  path p;

  bool starts_any = boost::starts_with(address, "//");
  if(!starts_any)
  {
    // Split on "/"
    auto parts = ossia::net::address_parts(address);

    // Potentially remove first ":".
    if(parts[0].back() == ':')
      parts[0].resize(parts[0].size() - 1);

    const std::string ossia_chars = ossia::net::name_characters().to_string();
    for(auto part : parts)
    {
      if(part != "..")
      {
        // Perform the various regex-like replacements
        boost::replace_all(part, "?", "[" + ossia_chars + "]??");
        boost::replace_all(part, "*", "[" + ossia_chars + "]*?");

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
  }
  else
  {
    // Means that we can start at any point, of any node.
    //* means all the nodes.
    // We have to pass an array with all the devices ?
  }

  return p;
}
}
}
