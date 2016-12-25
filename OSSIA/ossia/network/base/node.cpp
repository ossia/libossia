#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/detail/optional.hpp>
#include <boost/iterator/counting_iterator.hpp>
#define BOOST_LEXICAL_CAST_ASSUME_C_LOCALE
#include <boost/lexical_cast.hpp>
#include <iostream>
namespace ossia
{
namespace net
{
std::string sanitize_name(std::string ret)
{
  for(auto& c : ret)
  {
    if(is_valid_character_for_name(c))
      continue;
    else
      c = '_';
  }
  return ret;
}

std::string sanitize_name(std::string name_base, const std::vector<std::string>& brethren)
{
  auto name = sanitize_name(name_base);
  bool is_here = false;
  ossia::optional<int> name_instance;
  std::vector<int> instance_num;
  instance_num.reserve(brethren.size());

  // First get the root name : the first part of the "a.b"
  std::string root_name = name;
  {
    auto pos = name.find_last_of('.');
    if(pos != std::string::npos)
    {
      try
      {
        name_instance = boost::lexical_cast<int>(name.substr(pos + 1)); // OPTIMIZEME
        root_name = name.substr(0, pos);
      }
      catch (...)
      {
      }
    }
  }

  auto root_len = root_name.length();
  for (const std::string& n_name : brethren)
  {
    if (n_name == name)
    {
      is_here = true;
    }

    if(n_name.size() < root_len)
      continue;

    bool same_root = (n_name.compare(0, root_len, root_name) == 0);
    if (same_root && (n_name[root_len] == '.'))
    {
      // Instance
      try
      {
        int n = boost::lexical_cast<int>(n_name.substr(root_len + 1)); // OPTIMIZEME
        instance_num.push_back(n);
      }
      catch (...)
      {
        continue;
      }
    }
    // case where we have the "default" instance without .0
    else if(same_root && root_len == n_name.length())
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
      return root_name + "." + boost::lexical_cast<std::string>(instance_num.back() + 1);
    }
  }
}

std::vector<std::string> address_parts(const ossia::string_view& src)
{
  std::vector<std::string> sub;

  if(!src.empty())
  {
    sub.reserve(4);
    if(src[0] != '/')
    {
      boost::split(sub, src, boost::is_any_of("/"));
    }
    else
    {
      ossia::string_view sv = src.substr(1);
      boost::split(
            sub,
            sv,
            boost::is_any_of("/"));
    }
  }

  return sub;
}
node_base::~node_base() = default;

const extended_attributes& node_base::getExtendedAttributes() const
{
  return mExtended;
}

void node_base::setExtendedAttributes(
    const extended_attributes& e)
{
  mExtended = e;
}

boost::any node_base::getAttribute(
    const std::string& str) const
{
  auto it = mExtended.find(str);
  if(it != mExtended.end())
    return it.value();
  return {};
}



node_base* node_base::createChild(const std::string& name)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return nullptr;

  auto res = makeChild(sanitize_name(name));

  auto ptr = res.get();
  if (res)
  {
    mChildren.push_back(std::move(res));
    dev.onNodeCreated(*ptr);
  }

  return ptr;
}

node_base* node_base::findChild(const std::string& name)
{
  for(auto& node : mChildren)
  {
    if(node->getName() == name)
      return node.get();
  }

  return nullptr;
}

bool node_base::removeChild(const std::string& name)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return false;

  auto it = find_if(
              mChildren, [&](const auto& c) { return c->getName() == sanitize_name(name); });

  if (it != mChildren.end())
  {
    dev.onNodeRemoving(**it);
    removingChild(**it);
    mChildren.erase(it);

    return true;
  }
  else
  {
    return false;
  }
}

bool node_base::removeChild(const node_base& n)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return false;
  auto it = find_if(mChildren, [&](const auto& c) { return c.get() == &n; });

  if (it != mChildren.end())
  {
    dev.onNodeRemoving(**it);
    removingChild(**it);
    mChildren.erase(it);

    return true;
  }
  else
  {
    return false;
  }
}

void node_base::clearChildren()
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return;

  for (auto& child : mChildren)
    removingChild(*child);
  mChildren.clear();
}

optional<instance_bounds> get_dynamic_instances(const extended_attributes& n)
{
  return get_optional_attribute<instance_bounds>(n, "instanceBounds");
}

void set_dynamic_instances(extended_attributes& n, optional<instance_bounds> i)
{
  set_optional_attribute(n, "instanceBounds", std::move(i));
}


optional<tags> get_tags(const extended_attributes& n)
{
  return get_optional_attribute<tags>(n, "tags");
}

void set_tags(extended_attributes& n, const optional<tags>& v)
{
  set_optional_attribute(n, "tags", v);
}


optional<description> get_description(const extended_attributes& n)
{
  return get_optional_attribute<description>(n, "description");
}

void set_description(extended_attributes& n, const optional<description>& v)
{
  set_optional_attribute(n, "description", v);
}


optional<priority> get_priority(const extended_attributes& n)
{
  return get_optional_attribute<priority>(n, "priority");
}

void set_priority(extended_attributes& n, optional<priority> v)
{
  set_optional_attribute(n, "priority", v);
}


optional<refresh_rate> get_refresh_rate(const extended_attributes& n)
{
  return get_optional_attribute<refresh_rate>(n, "refreshRate");
}

void set_refresh_rate(extended_attributes& n, optional<refresh_rate> v)
{
  set_optional_attribute(n, "refreshRate", v);
}

}
}
