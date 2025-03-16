// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>

namespace ossia
{
state::children_type::iterator state::begin() noexcept
{
  return m_children.begin();
}
state::children_type::iterator state::end() noexcept
{
  return m_children.end();
}
state::children_type::const_iterator state::begin() const noexcept
{
  return m_children.begin();
}
state::children_type::const_iterator state::end() const noexcept
{
  return m_children.end();
}
state::children_type::const_iterator state::cbegin() const noexcept
{
  return m_children.cbegin();
}
state::children_type::const_iterator state::cend() const noexcept
{
  return m_children.cend();
}

const state::children_type& state::children() const noexcept
{
  return m_children;
}
std::size_t state::size() const
{
  return m_children.size();
}

bool state::empty() const
{
  return m_children.empty();
}

void state::launch()
{
  for(auto& state : m_children)
  {
    ossia::apply(state_execution_visitor{}, state);
  }
}

void state::add(const state_element& e)
{
  if(e)
    m_children.push_back(e);
}

void state::add(state_element&& e)
{
  if(e)
    m_children.push_back(std::move(e));
}

void state::remove(const state_element& e)
{
  ossia::remove_erase(m_children, e);
}

void state::remove(std::vector<state_element>::iterator e)
{
  m_children.erase(e);
}

void state::remove(std::vector<state_element>::const_iterator e)
{
  m_children.erase(e);
}

void state::reserve(std::size_t s)
{
  m_children.reserve(s);
}

void state::clear()
{
  m_children.clear();
}

bool operator==(const state& lhs, const state& rhs)
{
  return lhs.m_children == rhs.m_children;
}

bool operator!=(const state& lhs, const state& rhs)
{
  return lhs.m_children != rhs.m_children;
}

void flatten_and_filter(ossia::state& state, const state_element& element)
{
  ossia::apply(state_flatten_visitor<ossia::state, false>{state}, element);
}

void flatten_and_filter(ossia::state& state, state_element&& element)
{
  ossia::apply(state_flatten_visitor<ossia::state, false>{state}, std::move(element));
}

void merge_flatten_and_filter(ossia::state& state, const state_element& element)
{
  ossia::apply(state_flatten_visitor<ossia::state, true>{state}, element);
}

void merge_flatten_and_filter(ossia::state& state, state_element&& element)
{
  ossia::apply(state_flatten_visitor<ossia::state, true>{state}, std::move(element));
}
}
