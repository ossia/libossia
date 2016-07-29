#pragma once
#include <ossia/editor/state/state_element.hpp>
#include <ossia/detail/algorithms.hpp>
namespace ossia
{
  struct StateFlattenVisitor
  {
          State& state;

          // Const reference overloads
          void operator()(const Message& messageToAppend)
          {
              // find message with the same address to replace it
              auto it = find_if(state, [&] (const StateElement& e)
              {
                  if(auto m = e.target<Message>())
                      return &m->address == &messageToAppend.address;
                  return false;
              });

              if(it != state.end())
              {
                  // Merge messages
                  it->target<Message>()->value = messageToAppend.value;
              }
              else
              {
                  state.add(messageToAppend);
              }
          }

          void operator()(const State& s)
          {
              for (const auto& e : s)
              {
                  flattenAndFilter(state, e);
              }
          }

          void operator()(const CustomState& e)
          {
              state.add(e);
          }

          // rvalue reference overloads
          void operator()(Message&& messageToAppend)
          {
              // find message with the same address to replace it
              auto it = find_if(state, [&] (const StateElement& e)
              {
                  if(auto m = e.target<Message>())
                      return &m->address == &messageToAppend.address;
                  return false;
              });

              if(it != state.end())
              {
                  // Merge messages
                  it->target<Message>()->value = std::move(messageToAppend.value);
              }
              else
              {
                  state.add(std::move(messageToAppend));
              }
          }

          void operator()(State&& s)
          {
              for (auto&& e : s)
              {
                  flattenAndFilter(state, std::move(e));
              }
          }

          void operator()(CustomState&& e)
          {
              state.add(std::move(e));
          }
  };

}
