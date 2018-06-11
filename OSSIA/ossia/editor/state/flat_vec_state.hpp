#pragma once
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/apply.hpp>

namespace ossia
{
struct flat_vec_state
{
    using vec_type = ossia::small_vector<ossia::state_element, 16>;
    using iterator = typename vec_type::iterator;
    using const_iterator = typename vec_type::const_iterator;
    ossia::small_vector<ossia::state_element, 16> m_children;
    void add(const ossia::state_element& other)
    { m_children.push_back(other); }
    void add(ossia::state_element&& other)
    { m_children.push_back(std::move(other)); }

    void remove(const_iterator other)
    { m_children.erase(other); }

    template<typename T>
    auto find(const T& val)
    { return ossia::find(m_children, val); }

    void launch()
    {
      for (auto& state : m_children)
      {
        ossia::apply(state_execution_visitor{}, std::move(state));
      }
    }

    auto reserve(std::size_t n) { return m_children.reserve(n); }
    auto clear() { return m_children.clear(); }
    auto begin() { return m_children.begin(); }
    auto end() { return m_children.end(); }
    auto begin() const { return m_children.begin(); }
    auto end() const { return m_children.end(); }
};

struct mono_state
{
    ossia::state_element e;
    void add(const ossia::state_element& other)
    { e = other; }
    void add(ossia::state_element&& other)
    { e = std::move(other); }
    template<typename T>
    void remove(const T& other)
    {
    }

    template<typename T>
    auto find(const T&)
    {
      return e ? &e : nullptr;
    }

    auto end() const { return nullptr; }
};

#if defined(OSSIA_SMALL_VECTOR)
  ossia::state_element& get_state_element(ossia::flat_vec_state::iterator iterator)
  {
    return *iterator;
  }
#endif
}
