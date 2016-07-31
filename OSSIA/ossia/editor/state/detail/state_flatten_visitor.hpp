#pragma once
#include <ossia/editor/state/state_element.hpp>
#include <ossia/detail/algorithms.hpp>
namespace ossia
{
  struct state_flatten_visitor
  {
          ossia::state& state;

          // Const reference overloads
          void operator()(const message& messageToAppend)
          {
              // find message with the same address to replace it
              auto it = find_if(state, [&] (const state_element& e)
              {
                  if(auto m = e.target<message>())
                      return &m->address.get() == &messageToAppend.address.get();
                  return false;
              });

              if(it != state.end())
              {
                  // Merge messages
                  it->target<message>()->value = messageToAppend.value;
              }
              else
              {
                  state.add(messageToAppend);
              }
          }

          void operator()(const ossia::state& s)
          {
              for (const auto& e : s)
              {
                  flattenAndFilter(state, e);
              }
          }

          void operator()(const custom_state& e)
          {
              state.add(e);
          }

          // rvalue reference overloads
          void operator()(message&& messageToAppend)
          {
              // find message with the same address to replace it
              auto it = find_if(state, [&] (const state_element& e)
              {
                  if(auto m = e.target<message>())
                      return &m->address.get() == &messageToAppend.address.get();
                  return false;
              });

              if(it != state.end())
              {
                  // Merge messages
                  it->target<message>()->value = std::move(messageToAppend.value);
              }
              else
              {
                  state.add(std::move(messageToAppend));
              }
          }

          void operator()(ossia::state&& s)
          {
              for (auto&& e : s)
              {
                  flattenAndFilter(state, std::move(e));
              }
          }

          void operator()(custom_state&& e)
          {
              state.add(std::move(e));
          }
  };

}
