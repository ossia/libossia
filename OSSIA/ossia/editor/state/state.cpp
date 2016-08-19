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
  return mChildren.size();
}

void state::launch() const
{
  for (const auto& state : mChildren)
  {
    ossia::apply(state_execution_visitor{}, state);
  }
}

void state::add(const state_element& e)
{
  if (e)
    mChildren.push_back(e);
}

void state::add(state_element&& e)
{
  if (e)
    mChildren.push_back(std::move(e));
}

void state::remove(const state_element& e)
{
  mChildren.erase(
      std::remove(mChildren.begin(), mChildren.end(), e), mChildren.end());
}

void state::reserve(std::size_t s)
{
  mChildren.reserve(s);
}

void state::clear()
{
  mChildren.clear();
}

bool operator==(const state& lhs, const state& rhs)
{
  return lhs.mChildren == rhs.mChildren;
}

bool operator!=(const state& lhs, const state& rhs)
{
  return lhs.mChildren != rhs.mChildren;
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
