#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/address.hpp>
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
  auto name = sanitize_name(std::move(name_base));
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

  auto res = makeChild(sanitize_name(name, childrenNames()));

  auto ptr = res.get();
  if (res)
  {
    mChildren.push_back(std::move(res));
    dev.onNodeCreated(*ptr);
  }

  return ptr;
}

std::vector<std::string> node_base::childrenNames() const
{
  std::vector<std::string> bros_names;
  bros_names.reserve(mChildren.size());

  std::transform(mChildren.cbegin(), mChildren.cend(), std::back_inserter(bros_names),
                 [] (const auto& n) { return n->getName(); });

  return bros_names;
}

node_base*node_base::addChild(std::unique_ptr<node_base> n)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return nullptr;

  if(n)
  {
    const auto name = n->getName();
    if(name == sanitize_name(name, childrenNames()))
    {
      auto ptr = n.get();

      mChildren.push_back(std::move(n));
      dev.onNodeCreated(*ptr);
      return ptr;
    }
  }
  return nullptr;
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

  auto san_name = sanitize_name(name);

  auto it = find_if(
        mChildren, [&](const auto& c) { return c->getName() == san_name; });

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

#define OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(Type, Name, String) \
  optional<Type> get_ ## Name (const extended_attributes& n) \
{ using namespace std::literals; \
  return get_optional_attribute<Type>(n, String); \
} \
  \
  void set_ ## Name (extended_attributes& n, optional<Type> i) \
{ using namespace std::literals; \
  set_optional_attribute(n, String, std::move(i)); \
} \


OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(instance_bounds, instance_bounds, "instanceBounds"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(tags, tags, "tags"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(description, description, "description"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(priority, priority, "priority"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(refresh_rate, refresh_rate, "refreshRate"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(value_step_size, value_step_size, "valueStepsize"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(critical, critical, "critical"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_name, app_name, "appName"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_version, app_version, "appVersion"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_creator, app_creator, "appCreator"s)
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(default_value, default_value, "valueDefault"s)

optional<extended_type> get_extended_type(const ossia::net::node_base& n)
{
  auto opt = get_optional_attribute<extended_type>(n, "extended_type");
  if(!opt)
  {
    if(address_base* addr = n.getAddress())
    {
      switch(addr->getValueType())
      {
        case ossia::val_type::VEC2F:
        case ossia::val_type::VEC3F:
        case ossia::val_type::VEC4F:
          return float_array_type();
      }
    }
  }
  return opt;
}

void set_extended_type(extended_attributes& n, optional<extended_type> i)
{
  set_optional_attribute(n, "extended_type", std::move(i));
}


void set_description(extended_attributes& n, const char* arg)
{
  if(arg)
    set_description(n, std::string{arg});
  else
    set_description(n, ossia::none);
}

extended_type generic_buffer_type()
{
  return "buffer";
}

extended_type filesystem_path_type()
{
  return "filepath";
}

extended_type float_array_type()
{
  return "vecf";
}

}
}
