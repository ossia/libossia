// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/address_data.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/domain/domain.hpp>
#define BOOST_LEXICAL_CAST_ASSUME_C_LOCALE
#include <ossia/detail/logger.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#if defined(OSSIA_QT)
#include <ossia-qt/name_utils.hpp>
#endif

namespace ossia
{
namespace net
{
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
  chobo::small_vector<int, 16> instance_num;
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

void sanitize_name(std::string& name, const node_base::children_t& brethren)
{
  sanitize_name(name);
  bool is_here = false;
  ossia::optional<int> name_instance;
  chobo::small_vector<int, 16> instance_num;
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

std::vector<std::string> address_parts(const ossia::string_view& src)
{
  std::vector<std::string> sub;

  if (!src.empty())
  {
    sub.reserve(4);
    if (src[0] != '/')
    {
      boost::split(sub, src, boost::is_any_of("/"));
    }
    else
    {
      ossia::string_view sv = src.substr(1);
      boost::split(sub, sv, boost::is_any_of("/"));
    }
  }

  return sub;
}
node_base::~node_base() = default;

void node_base::set_address(std::unique_ptr<address_base>)
{
}

const extended_attributes& node_base::get_extended_attributes() const
{
  return m_extended;
}

void node_base::set_extended_attributes(const extended_attributes& e)
{
  m_extended = e;
}

boost::any node_base::get_attribute(ossia::string_view str) const
{
  auto it = m_extended.find(str);
  if (it != m_extended.end())
    return it.value();
  return {};
}

void node_base::set(string_view str, bool value)
{
  auto opt = ossia::has_attribute(*this, str);
  if (opt != value)
  {
    if(value)
      ossia::set_attribute((extended_attributes&)*this, str);
    else
      ossia::unset_attribute((extended_attributes&)*this, str);

    get_device().on_attribute_modified(*this, str);
  }
}

node_base* node_base::create_child(std::string name)
{
  auto& dev = get_device();
  if (!dev.get_capabilities().change_tree)
    return nullptr;
  ossia::net::node_base* ptr{};
  {
    write_lock_t lock{m_mutex};

    sanitize_name(name, m_children);
    auto res = make_child(name);

    if ((ptr = res.get()))
    {
      m_children.push_back(std::move(res));
    }
  }

  if (ptr)
  {
    dev.on_node_created(*ptr);
  }
  return ptr;
}

std::vector<std::string> node_base::children_names() const
{
  SPDLOG_TRACE((&ossia::logger()), "locking(childrenNames)");
  read_lock_t lock{m_mutex};
  SPDLOG_TRACE((&ossia::logger()), "locked(childrenNames)");
  std::vector<std::string> bros_names;
  bros_names.reserve(m_children.size());

  std::transform(
      m_children.cbegin(), m_children.cend(), std::back_inserter(bros_names),
      [](const auto& n) { return n->get_name(); });

  SPDLOG_TRACE((&ossia::logger()), "unlocked(childrenNames)");
  return bros_names;
}

bool node_base::is_root_instance(const node_base& child) const
{
  SPDLOG_TRACE((&ossia::logger()), "locking(is_root_instance)");
  read_lock_t lock{m_mutex};
  SPDLOG_TRACE((&ossia::logger()), "locked(is_root_instance)");

  const auto& child_name = child.get_name();
  for (auto& cld : m_children)
  {
    const auto& bro_name = cld->get_name();
    if ((bro_name.size() > (child_name.size() + 1))
        && boost::starts_with(bro_name, child_name)
        && (bro_name[child_name.size()] == '.'))
    {
      // TODO to be sure we should do
      // a full regex check but is it really worth it..
      SPDLOG_TRACE((&ossia::logger()), "unlocked(is_root_instance)");
      return true;
    }
  }

  SPDLOG_TRACE((&ossia::logger()), "unlocked(is_root_instance)");
  return false;
}

node_base* node_base::add_child(std::unique_ptr<node_base> n)
{
  auto& dev = get_device();
  if (!dev.get_capabilities().change_tree)
    return nullptr;

  if (n)
  {
    const auto& name = n->get_name();
    if (name == sanitize_name(name, children_names()))
    {
      auto ptr = n.get();
      {
        write_lock_t lock{m_mutex};
        m_children.push_back(std::move(n));
      }
      dev.on_node_created(*ptr);
      return ptr;
    }
  }
  return nullptr;
}

node_base* node_base::find_child(ossia::string_view name)
{
  {
    SPDLOG_TRACE((&ossia::logger()), "locking(findChild)");
    read_lock_t lock{m_mutex};
    SPDLOG_TRACE((&ossia::logger()), "locked(findChild)");
    for (auto& node : m_children)
    {
      if (node->get_name() == name)
      {
        SPDLOG_TRACE((&ossia::logger()), "unlocked(findChild)");
        return node.get();
      }
    }
  }

  SPDLOG_TRACE((&ossia::logger()), "unlocked(findChild)");
  return nullptr;
}

#if defined(OSSIA_QT)
node_base* node_base::find_child(const QString& name)
{
  {
    SPDLOG_TRACE((&ossia::logger()), "locking(findChild)");
    read_lock_t lock{m_mutex};
    SPDLOG_TRACE((&ossia::logger()), "locked(findChild)");
    for (auto& node : m_children)
    {
      if (latin_compare(name, node->get_name()))
      {
        SPDLOG_TRACE((&ossia::logger()), "unlocked(findChild)");
        return node.get();
      }
    }
  }

  SPDLOG_TRACE((&ossia::logger()), "unlocked(findChild)");
  return nullptr;
}
#endif

bool node_base::has_child(node_base& n)
{
  SPDLOG_TRACE((&ossia::logger()), "locking(hasChild)");
  read_lock_t lock{m_mutex};

  SPDLOG_TRACE((&ossia::logger()), "locked(hasChild)");
  // TODO why not n.parent() == this ?
  return any_of(m_children, [&](auto& cld) { return cld.get() == &n; });
}

bool node_base::remove_child(const std::string& name)
{
  auto& dev = get_device();
  if (!dev.get_capabilities().change_tree)
    return false;

  std::string n = name;
  sanitize_name(n);

  write_lock_t lock{m_mutex};
  auto it
      = find_if(m_children, [&](const auto& c) { return c->get_name() == n; });

  if (it != m_children.end())
  {
    dev.on_node_removing(**it);
    removing_child(**it);
    m_children.erase(it);

    return true;
  }
  else
  {
    return false;
  }
}

bool node_base::remove_child(const node_base& n)
{
  auto& dev = get_device();
  if (!dev.get_capabilities().change_tree)
    return false;

  write_lock_t lock{m_mutex};
  auto it = find_if(m_children, [&](const auto& c) { return c.get() == &n; });

  if (it != m_children.end())
  {
    dev.on_node_removing(**it);
    removing_child(**it);
    m_children.erase(it);

    return true;
  }
  else
  {
    return false;
  }
}

void node_base::clear_children()
{
  auto& dev = get_device();
  if (!dev.get_capabilities().change_tree)
    return;

  write_lock_t lock{m_mutex};
  for (auto& child : m_children)
  {
    dev.on_node_removing(*child);
    removing_child(*child);
    child.reset();
  }

  m_children.clear();
}

std::vector<node_base*> node_base::children_copy() const
{
  std::vector<node_base*> copy;
  SPDLOG_TRACE((&ossia::logger()), "locking(children_copy)");
  read_lock_t lock{m_mutex};

  SPDLOG_TRACE((&ossia::logger()), "locked(children_copy)");
  copy.reserve(m_children.size());
  for (auto& e : m_children)
    copy.push_back(e.get());

  SPDLOG_TRACE((&ossia::logger()), "unlocking(children_copy)");
  return copy;
}

#define OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(Type, Name, String) \
  optional<Type> get_##Name(const extended_attributes& n)      \
  {                                                            \
    return get_optional_attribute<Type>(n, text_##Name());     \
  }                                                            \
                                                               \
  void set_##Name(extended_attributes& n, optional<Type> i)    \
  {                                                            \
    set_optional_attribute(n, String, std::move(i));           \
  }                                                            \
  void set_##Name(ossia::net::node_base& n, optional<Type> i)  \
  {                                                            \
    n.set(ossia::string_view(String), std::move(i));           \
  }                                                            \
  ossia::string_view text_##Name()                             \
  {                                                            \
    return make_string_view(String);                           \
  }

#define OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_2(Type, Name, String) \
  optional<Type> get_##Name(const extended_attributes& n)        \
  {                                                              \
    return get_optional_attribute<Type>(n, text_##Name());       \
  }                                                              \
  void set_##Name(extended_attributes& n, Type&& i)              \
  {                                                              \
    set_attribute(n, String, std::move(i));                      \
  }                                                              \
  void set_##Name(extended_attributes& n, const Type& i)         \
  {                                                              \
    set_attribute(n, String, i);                      \
  }                                                              \
  void set_##Name(extended_attributes& n, ossia::none_t i)       \
  {                                                              \
    set_attribute(n, String, std::move(i));                      \
  }                                                              \
  void set_##Name(ossia::net::node_base& n, Type&& i)            \
  {                                                              \
    n.set(ossia::string_view(String), std::move(i));             \
  }                                                              \
  void set_##Name(ossia::net::node_base& n, const Type& i)       \
  {                                                              \
    n.set(ossia::string_view(String), i);             \
  }                                                              \
  void set_##Name(ossia::net::node_base& n, ossia::none_t i)     \
  {                                                              \
    n.set(ossia::string_view(String), optional<Type>{});         \
  }                                                              \
  ossia::string_view text_##Name()                               \
  {                                                              \
    return make_string_view(String);                             \
  }

#define OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_BOOL(Type, Name, String) \
  Type get_##Name(const extended_attributes& n)                     \
  {                                                                 \
    return has_attribute(n, text_##Name());                         \
  }                                                                 \
                                                                    \
  void set_##Name(extended_attributes& n, Type i)                   \
  {                                                                 \
    if (i)                                                          \
      set_attribute(n, String);                                     \
    else                                                            \
      set_attribute(n, String, ossia::none);                        \
  }                                                                 \
  void set_##Name(ossia::net::node_base& n, Type i)                 \
  {                                                                 \
    n.set(ossia::string_view(String), i);                \
  }                                                                 \
  ossia::string_view text_##Name()                                  \
  {                                                                 \
    return make_string_view(String);                                \
  }

OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(
    instance_bounds, instance_bounds, "instanceBounds")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(tags, tags, "tags")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(description, description, "description")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(priority, priority, "priority")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(refresh_rate, refresh_rate, "refreshRate")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(
    value_step_size, value_step_size, "valueStepsize")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_BOOL(zombie, zombie, "zombie")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_BOOL(hidden, hidden, "hidden")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_name, app_name, "appName")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_version, app_version, "appVersion")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL(app_creator, app_creator, "appCreator")
OSSIA_ATTRIBUTE_GETTER_SETTER_IMPL_2(
    default_value, default_value, "valueDefault")

ossia::string_view text_extended_type()
{
  return make_string_view("extended_type");
}

optional<extended_type> get_extended_type(const ossia::net::node_base& n)
{
  auto opt = get_optional_attribute<extended_type>(n, text_extended_type());
  if (!opt)
  {
    if (address_base* addr = n.get_address())
    {
      switch (addr->get_value_type())
      {
        case ossia::val_type::VEC2F:
        case ossia::val_type::VEC3F:
        case ossia::val_type::VEC4F:
          return float_array_type();
        case ossia::val_type::TUPLE:
          return tuple_type();
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
void set_extended_type(ossia::net::node_base& n, optional<extended_type> i)
{
  n.set(text_extended_type(), std::move(i));
}

void set_description(extended_attributes& n, const char* arg)
{
  if (arg)
    set_description(n, std::string{arg});
  else
    set_description(n, ossia::none);
}

void set_description(ossia::net::node_base& n, const char* arg)
{
  if (arg)
    set_description(n, std::string{arg});
  else
    set_description(n, ossia::none);
}

void set_default_value(extended_attributes& n, int arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, long arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, bool arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, char arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, float arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, double arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(extended_attributes& n, const char* arg)
{
  set_default_value(n, ossia::value{std::string(arg)});
}

void set_default_value(ossia::net::node_base& n, int arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, long arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, bool arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, char arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, float arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, double arg)
{
  set_default_value(n, ossia::value{arg});
}
void set_default_value(ossia::net::node_base& n, const char* arg)
{
  set_default_value(n, ossia::value{std::string(arg)});
}

// Address-related getters - setters
ossia::string_view text_value()
{
  return make_string_view("value");
}
value clone_value(const ossia::net::node_base& n)
{
  if (auto addr = n.get_address())
    return addr->value();
  return {};
}
void set_value(ossia::net::node_base& n, value v)
{
  if (auto addr = n.get_address())
    addr->set_value(std::move(v));
}
void set_value(ossia::net::address_data& n, value v)
{
  n.value = std::move(v);
}

ossia::string_view text_value_type()
{
  return make_string_view("type");
}
optional<val_type> get_value_type(const ossia::net::node_base& n)
{
  if (auto addr = n.get_address())
    return addr->get_value_type();
  return ossia::none;
}
void set_value_type(ossia::net::node_base& n, val_type v)
{
  if (auto addr = n.get_address())
    addr->set_value_type(std::move(v));
}

ossia::string_view text_domain()
{
  return make_string_view("domain");
}
domain get_domain(const ossia::net::node_base& n)
{
  if (auto addr = n.get_address())
    return addr->get_domain();
  return {};
}
void set_domain(ossia::net::node_base& n, domain v)
{
  if (auto addr = n.get_address())
    addr->set_domain(std::move(v));
}
void set_domain(ossia::net::address_data& n, domain v)
{
  n.domain = std::move(v);
}

ossia::string_view text_access_mode()
{
  return make_string_view("access");
}
optional<access_mode> get_access_mode(const ossia::net::node_base& n)
{
  if (auto addr = n.get_address())
    return addr->get_access();
  return ossia::none;
}
void set_access_mode(ossia::net::node_base& n, access_mode v)
{
  if (auto addr = n.get_address())
    addr->set_access(std::move(v));
}
void set_access_mode(ossia::net::address_data& n, access_mode v)
{
  n.access = v;
}

ossia::string_view text_bounding_mode()
{
  return make_string_view("bounding");
}
optional<bounding_mode> get_bounding_mode(const ossia::net::node_base& n)
{
  if (auto addr = n.get_address())
    return addr->get_bounding();
  return ossia::none;
}
void set_bounding_mode(ossia::net::node_base& n, bounding_mode v)
{
  if (auto addr = n.get_address())
    addr->set_bounding(std::move(v));
}
void set_bounding_mode(ossia::net::address_data& n, bounding_mode v)
{
  n.bounding = v;
}

ossia::string_view text_muted()
{
  return make_string_view("muted");
}
muted get_muted(const ossia::net::node_base& n)
{
  if (auto addr = n.get_address())
    return addr->get_muted();
  return false;
}
void set_muted(ossia::net::node_base& n, muted v)
{
  if (auto addr = n.get_address())
    addr->set_muted(v);
}
void set_muted(ossia::net::address_data& n, muted v)
{
  n.muted = v;
}

ossia::string_view text_critical()
{
  return make_string_view("critical");
}
critical get_critical(const ossia::net::node_base& n)
{
  if (auto addr = n.get_address())
    return addr->get_critical();
  return false;
}
void set_critical(ossia::net::node_base& n, critical v)
{
  if (auto addr = n.get_address())
    addr->set_critical(v);
}
void set_critical(ossia::net::address_data& n, critical v)
{
  n.critical = v;
}

ossia::string_view text_repetition_filter()
{
  return make_string_view("repetition_filter");
}
repetition_filter get_repetition_filter(const ossia::net::node_base& n)
{
  if (auto addr = n.get_address())
    return addr->get_repetition_filter();
  return repetition_filter::OFF;
}
void set_repetition_filter(ossia::net::node_base& n, repetition_filter v)
{
  if (auto addr = n.get_address())
    addr->set_repetition_filter(std::move(v));
}

ossia::string_view text_unit()
{
  return make_string_view("unit");
}
unit_t get_unit(const ossia::net::node_base& n)
{
  if (auto addr = n.get_address())
    return addr->get_unit();
  return {};
}
void set_unit(ossia::net::node_base& n, unit_t v)
{
  if (auto addr = n.get_address())
    addr->set_unit(std::move(v));
}

namespace
{
static node_base* find_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto child = node.find_child(address.substr(0, first_slash_index));
    if (child)
    {
      // There are still nodes since we found a slash
      return find_node_rec(*child, address.substr(first_slash_index + 1));
    }
    else
    {
      return nullptr;
    }
  }
  else
  {
    // One of the child may be the researched node.
    return node.find_child(address);
  }
}

static node_base& find_or_create_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto cld = node.find_child(cur);
    if (cld)
    {
      // There are still nodes since we found a slash
      return find_or_create_node_rec(
          *cld, address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto& child = *node.create_child(std::string(cur));

      // Recurse on it
      return find_or_create_node_rec(
          child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // One of the child may be the researched node.
    auto n = node.find_child(address);
    if (n)
    {
      return *n;
    }
    else
    {
      // Create and return the node
      return *node.create_child(std::string(address));
    }
  }
}

static node_base& create_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto cld = node.find_child(cur);
    if (cld)
    {
      // There are still nodes since we found a slash
      return create_node_rec(*cld, address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto& child = *node.create_child(std::string(cur));

      // Recurse on it
      return create_node_rec(child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // Create and return the node
    return *node.create_child(std::string(address));
  }
}

//! Note : here we modify the string_view only.
//! The original address remains unchanged.
static ossia::string_view sanitize_address(ossia::string_view address)
{
  while (boost::algorithm::starts_with(address, "/"))
    address.remove_prefix(1);
  while (boost::algorithm::ends_with(address, "/"))
    address.remove_suffix(1);
  return address;
}
}

node_base* find_node(node_base& dev, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    return &dev;

  // address now looks like a/b/c
  return find_node_rec(dev, address);
}

node_base& find_or_create_node(node_base& node, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    return node;

  // address now looks like a/b/c
  return find_or_create_node_rec(node, address);
}

node_base& create_node(node_base& node, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    address = "node";

  // address now looks like a/b/c
  return create_node_rec(node, address);
}

node_base*
find_or_create_node(node_base& dev, string_view address_base, bool create)
{
  if (create)
  {
    return ossia::net::find_node(dev, address_base);
  }
  else
  {
    return &ossia::net::create_node(dev, address_base);
  }
}
}
}
