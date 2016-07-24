#include "Editor/State_impl.h"
#include <Editor/StateElement.h>
#include <Misc/Util.h>

namespace OSSIA
{
  bool operator==(const State& lhs, const State& rhs)
  {
      return lhs.children == rhs.children;
  }

  bool operator!=(const State& lhs, const State& rhs)
  {
      return lhs.children == rhs.children;
  }

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
                      return m->address == messageToAppend.address;
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
                      return m->address == messageToAppend.address;
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

  void launch(const StateElement& s)
  {
      if(s)
          eggs::variants::apply(StateExecutionVisitor{}, s);
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

}
