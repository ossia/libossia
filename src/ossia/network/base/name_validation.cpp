#include "name_validation.hpp"

#include <ossia/detail/optional.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/network/base/node.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
namespace ossia
{
namespace net
{
std::string& sanitize_device_name(std::string& ret)
{
  // Note : if this is changed, also change the Qt version in js_utilities.cpp
  for (auto& c : ret)
  {
    if (is_valid_character_for_device(c))
      continue;
    else
      c = '_';
  }
  return ret;
}

std::string& sanitize_name(std::string& ret)
{
  // Note : if this is changed, also change the Qt version in js_utilities.cpp
  for (auto& c : ret)
  {
    if (is_valid_character_for_name(c))
      continue;
    else
      c = '_';
  }
  return ret;
}

std::string sanitize_name(const std::string& ret)
{
  std::string n = ret;
  ossia::net::sanitize_name(n);
  return n;
}

std::string sanitize_name(std::string&& ret)
{
  std::string n = std::move(ret);
  ossia::net::sanitize_name(n);
  return n;
}

std::string sanitize_name(const char* ret)
{
  std::string n = ret;
  ossia::net::sanitize_name(n);
  return n;
}

std::string
sanitize_name(std::string name, const std::vector<std::string>& brethren)
{
  sanitize_name(name);
  bool is_here = false;
  ossia::optional<int> name_instance;
  ossia::small_vector<int, 16> instance_num;
  instance_num.reserve(brethren.size());

  // First get the root name : the first part of the "a.b"
  std::string root_name = name;
  {
    auto pos = name.find_last_of('.');
    if (pos != std::string::npos)
    {
      try
      {
        name_instance
            = boost::lexical_cast<int>(name.substr(pos + 1)); // OPTIMIZEME
        root_name = name.substr(0, pos);
      }
      catch (...)
      {
      }
    }
  }

  const auto root_len = root_name.size();
  for (const std::string& n_name : brethren)
  {
    if (n_name == name)
    {
      is_here = true;
    }

    if (n_name.size() < (root_len + 1))
      continue;

    bool same_root = (n_name.compare(0, root_len, root_name) == 0);
    if (same_root && (n_name[root_len] == '.'))
    {
      // Instance
      try
      {
        int n = boost::lexical_cast<int>(
            n_name.substr(root_len + 1)); // OPTIMIZEME
        instance_num.push_back(n);
      }
      catch (...)
      {
        continue;
      }
    }
    // case where we have the "default" instance without .0
    else if (same_root && root_len == n_name.length())
    {
      instance_num.push_back(0);
    }
  }

  if (!is_here)
  {
    return name;
  }
  else
  {
    auto n = instance_num.size();
    if ((n == 0) || ((n == 1) && (instance_num[0] == 0)))
    {
      return root_name + ".1";
    }
    else
    {
      std::sort(instance_num.begin(), instance_num.end());
      return root_name + "."
             + boost::lexical_cast<std::string>(instance_num.back() + 1);
    }
  }
}

void sanitize_name(
    std::string& name, const ossia::net::node_base::children_t& brethren)
{
  sanitize_name(name);
  bool is_here = false;
  ossia::optional<int> name_instance;
  ossia::small_vector<int, 16> instance_num;
  instance_num.reserve(brethren.size());

  // First get the root name : the first part of the "a.b"
  std::string root_name = name;
  {
    auto pos = name.find_last_of('.');
    if (pos != std::string::npos)
    {
      int n{};
      if(boost::conversion::detail::try_lexical_convert(name.substr(pos + 1), n))
      {
        // OPTIMIZEME
        name_instance = n;
        root_name = name.substr(0, pos);
      }
    }
  }

  const auto root_len = root_name.size();
  for (const auto& bro : brethren)
  {
    const auto& n_name = bro->get_name();
    if (n_name == name)
    {
      is_here = true;
    }

    if (n_name.size() < (root_len + 1))
      continue;

    bool same_root = (n_name.compare(0, root_len, root_name) == 0);
    if (same_root && (n_name[root_len] == '.'))
    {
      // Instance
      int n{};
      // OPTIMIZEME
      if(boost::conversion::detail::try_lexical_convert(n_name.substr(root_len + 1), n))
      {
        instance_num.push_back(n);
      }
    }
    // case where we have the "default" instance without .0
    else if (same_root && root_len == n_name.length())
    {
      instance_num.push_back(0);
    }
  }

  if (!is_here)
  {
    return;
  }
  else
  {
    auto n = instance_num.size();
    if ((n == 0) || ((n == 1) && (instance_num[0] == 0)))
    {
      name = root_name;
      name += ".1";
    }
    else
    {
      std::sort(instance_num.begin(), instance_num.end());
      name = root_name;
      name += '.';
      name += boost::lexical_cast<std::string>(instance_num.back() + 1);
    }
  }
}

std::vector<std::string> address_parts(ossia::string_view src)
{
  std::vector<std::string> sub;

  if (!src.empty())
  {
    if (src.front() == '/')
    {
      src.remove_prefix(1);
    }
    if (!src.empty() && src.back() == '/')
    {
      src.remove_suffix(1);
    }

    sub.reserve(4);
    boost::split(sub, src, boost::is_any_of("/"));
  }

  return sub;
}

bool is_brace_expansion(string_view s)
{
  int brace_count = 0;
  int arr_count = 0;
  bool b = false;

  for (std::size_t i = 0; i < s.size(); i++)
  {
    if (s[i] == '{')
    {
      b = true;
      brace_count++;
    }
    else if (s[i] == '[')
    {
      b = true;
      arr_count++;
    }
    else if (s[i] == '}')
    {
      brace_count--;
      if (brace_count < 0)
        return false;
    }
    else if (s[i] == ']')
    {
      arr_count--;
      if (arr_count < 0)
        return false;
    }
  }

  return b && brace_count == 0 && arr_count == 0;
}
}
}
