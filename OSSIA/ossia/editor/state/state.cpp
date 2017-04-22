#include <ossia/editor/state/state_element.hpp>

#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>
#include <iostream>

namespace ossia
{
std::size_t state::size() const
{
  return m_children.size();
}

bool state::empty() const
{
  return m_children.empty();
}

void state::launch() const
{
  for (const auto& state : m_children)
  {
    ossia::apply(state_execution_visitor{}, state);
  }
}

void state::add(const state_element& e)
{
  if (e)
    m_children.push_back(e);
}

void state::add(state_element&& e)
{
  if (e)
    m_children.push_back(std::move(e));
}

void state::remove(const state_element& e)
{
  m_children.erase(
      std::remove(m_children.begin(), m_children.end(), e), m_children.end());
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

void flatten_and_filter(state& state, const state_element& element)
{
  ossia::apply(state_flatten_visitor{state}, element);
}

void flatten_and_filter(state& state, state_element&& element)
{
  ossia::apply(state_flatten_visitor{state}, std::move(element));
}
}
