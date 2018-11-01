#pragma once
#include <ossia/detail/apply.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia
{
struct flat_vec_state
{
  using vec_type = ossia::small_vector<ossia::state_element, 16>;
  using iterator = typename vec_type::iterator;
  using const_iterator = typename vec_type::const_iterator;
  ossia::small_vector<ossia::state_element, 16> m_children;
  void add(const ossia::state_element& other) noexcept
  {
    m_children.push_back(other);
  }
  void add(ossia::state_element&& other) noexcept
  {
    m_children.push_back(std::move(other));
  }

  void remove(const_iterator other) noexcept
  {
    m_children.erase(other);
  }

  void remove(const state_element& e)
  {
    ossia::remove_erase(m_children, e);
  }

  template <typename T>
  auto find(const T& val) noexcept
  {
    return ossia::find(m_children, val);
  }

  void launch() noexcept
  {
    for (auto& state : m_children)
    {
      ossia::apply(state_execution_visitor{}, std::move(state));
    }
  }

  auto reserve(std::size_t n) noexcept
  {
    return m_children.reserve(n);
  }
  auto clear() noexcept
  {
    return m_children.clear();
  }
  auto begin() noexcept
  {
    return m_children.begin();
  }
  auto end() noexcept
  {
    return m_children.end();
  }
  auto begin() const noexcept
  {
    return m_children.begin();
  }
  auto end() const noexcept
  {
    return m_children.end();
  }

  auto size() const noexcept
  {
    return m_children.size();
  }
};

struct mono_state
{
  ossia::state_element e;
  void add(const ossia::state_element& other)
  {
    e = other;
  }
  void add(ossia::state_element&& other)
  {
    e = std::move(other);
  }
  template <typename T>
  void remove(const T& other)
  {
  }

  template <typename T>
  auto find(const T&)
  {
    return e ? &e : nullptr;
  }

  auto end() const
  {
    return nullptr;
  }
};

#if defined(OSSIA_SMALL_VECTOR)
inline ossia::state_element&
get_state_element(ossia::flat_vec_state::iterator iterator)
{
  return *iterator;
}
#endif
}
