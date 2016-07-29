#include <ossia/editor/state/state_element.hpp>

#include <ossia/editor/state/detail/state_execution_visitor.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/detail/algorithms.hpp>
#include <iostream>

namespace ossia
{
std::size_t State::size() const
{
    return children.size();
}

void State::launch() const
{
    StateExecutionVisitor v;
    for(const auto& state : children)
    {
        if(state)
            eggs::variants::apply(v, state);
    }
}

void State::add(const StateElement& e)
{
    if(e)
        children.push_back(e);
}

void State::add(StateElement&& e)
{
    if(e)
        children.push_back(std::move(e));
}

void State::remove(const StateElement& e)
{
    children.erase(
                std::remove(children.begin(), children.end(), e),
                children.end());
}

void State::reserve(std::size_t s)
{
    children.reserve(s);
}

void State::clear()
{
    children.clear();
}

bool operator==(const State& lhs, const State& rhs)
{
    return lhs.children == rhs.children;
}

bool operator!=(const State& lhs, const State& rhs)
{
    return lhs.children != rhs.children;
}


void flattenAndFilter(State& state, const StateElement& element)
{
    if(element)
        eggs::variants::apply(StateFlattenVisitor{state}, element);
}

void flattenAndFilter(State& state, StateElement&& element)
{
    if(element)
        eggs::variants::apply(StateFlattenVisitor{state}, std::move(element));
}
}
