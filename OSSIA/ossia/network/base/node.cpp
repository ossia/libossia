// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia/detail/logger.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <boost/algorithm/string/predicate.hpp>
#if defined(OSSIA_QT)
#include <ossia-qt/name_utils.hpp>
#endif

namespace ossia
{
namespace net
{
node_base::~node_base() = default;

void node_base::set_parameter(std::unique_ptr<parameter_base>)
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

ossia::any node_base::get_attribute(ossia::string_view str) const
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

}
}
