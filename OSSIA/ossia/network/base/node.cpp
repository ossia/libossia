#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/domain/domain.hpp>
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
std::string& sanitize_name(std::string& ret)
{
  // Note : if this is changed, also change the Qt version in js_utilities.cpp
  for(auto& c : ret)
  {
    if(is_valid_character_for_name(c))
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


std::string sanitize_name(std::string name, const std::vector<std::string>& brethren)
{
  sanitize_name(name);
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



node_base* node_base::createChild(std::string name)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return nullptr;

  auto res = makeChild(sanitize_name(std::move(name), childrenNames()));

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

node_base* node_base::findChild(ossia::string_view name)
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

  std::string n = name;
  sanitize_name(n);

  auto it = find_if(
              mChildren, [&](const auto& c) { return c->getName() == n; });

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
  return get_optional_attribute<Type>(n, String ## s); \
} \
  \
  void set_ ## Name (extended_attributes& n, optional<Type> i) \
{ using namespace std::literals; \
  set_optional_attribute(n, String, std::move(i)); \
} \
  ossia::string_view text_ ## Name () \
{ return make_string_view(String); }


#define OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_2(Type, Name, String) \
  optional<Type> get_ ## Name (const extended_attributes& n) \
{ using namespace std::literals; \
  return get_optional_attribute<Type>(n, String ## s); \
} \
  void set_ ## Name (extended_attributes& n, Type&& i) \
{ using namespace std::literals; \
  set_attribute(n, String, std::move(i)); \
} \
  void set_ ## Name (extended_attributes& n, const Type& i) \
{ using namespace std::literals; \
  set_attribute(n, String, std::move(i)); \
} \
  void set_ ## Name (extended_attributes& n, ossia::none_t i) \
{ using namespace std::literals; \
  set_attribute(n, String, std::move(i)); \
} \
  ossia::string_view text_ ## Name () \
{ return make_string_view(String); }

OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(instance_bounds, instance_bounds, "instanceBounds")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(tags, tags, "tags")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(description, description, "description")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(priority, priority, "priority")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(refresh_rate, refresh_rate, "refreshRate")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(value_step_size, value_step_size, "valueStepsize")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(critical, critical, "critical")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_name, app_name, "appName")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_version, app_version, "appVersion")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_creator, app_creator, "appCreator")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_2(default_value, default_value, "valueDefault")

ossia::string_view text_extended_type()
{ return make_string_view("extended_type"); }

optional<extended_type> get_extended_type(const ossia::net::node_base& n)
{
  auto opt = get_optional_attribute<extended_type>(n, text_extended_type() );
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
        default:
          break;
      }
    }
  }
  return opt;
}

void set_extended_type(extended_attributes& n, optional<extended_type> i)
{
  set_optional_attribute(n, text_extended_type(), std::move(i));
}

void set_description(extended_attributes& n, const char* arg)
{
  if(arg)
    set_description(n, std::string{arg});
  else
    set_description(n, ossia::none);
}

// Address-related ones

ossia::string_view text_value()
{ return make_string_view("value"); }
optional<value> clone_value(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->cloneValue();
  return ossia::none;
}
void set_value(ossia::net::node_base& n, value v)
{
  if(auto addr = n.getAddress()) addr->setValue(std::move(v));
}

ossia::string_view text_value_type()
{ return make_string_view("type"); }
optional<val_type> get_value_type(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getValueType();
  return ossia::none;
}
void set_value_type(ossia::net::node_base& n, val_type v)
{
  if(auto addr = n.getAddress()) addr->setValueType(std::move(v));
}

ossia::string_view text_domain()
{ return make_string_view("domain"); }
optional<domain> get_domain(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getDomain();
  return ossia::none;
}
void set_domain(ossia::net::node_base& n, domain v)
{
  if(auto addr = n.getAddress()) addr->setDomain(std::move(v));
}

ossia::string_view text_access_mode()
{ return make_string_view("access"); }
optional<access_mode> get_access_mode(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getAccessMode();
  return ossia::none;
}
void set_access_mode(ossia::net::node_base& n, access_mode v)
{
  if(auto addr = n.getAddress()) addr->setAccessMode(std::move(v));
}

ossia::string_view text_bounding_mode()
{ return make_string_view("bounding"); }
optional<bounding_mode> get_bounding_mode(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getBoundingMode();
  return ossia::none;
}
void set_bounding_mode(ossia::net::node_base& n, bounding_mode v)
{
  if(auto addr = n.getAddress()) addr->setBoundingMode(std::move(v));
}

ossia::string_view text_repetition_filter()
{ return make_string_view("repetition_filter"); }
optional<repetition_filter> get_repetition_filter(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getRepetitionFilter();
  return ossia::none;
}
void set_repetition_filter(ossia::net::node_base& n, repetition_filter v)
{
  if(auto addr = n.getAddress()) addr->setRepetitionFilter(std::move(v));
}

ossia::string_view text_unit()
{ return make_string_view("unit"); }
optional<unit_t> get_unit(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getUnit();
  return ossia::none;
}
void set_unit(ossia::net::node_base& n, unit_t v)
{
  if(auto addr = n.getAddress()) addr->setUnit(std::move(v));
}


}
}
