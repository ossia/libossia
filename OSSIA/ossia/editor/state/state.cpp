#include <Editor/StateElement.h>
#include <Editor/State/detail/StateExecutionVisitor.h>
#include <Editor/State/detail/StateFlattenVisitor.h>
#include <Editor/State/detail/StatePrintVisitor.h>
#include <Network/Address.h>
#include <Network/Node.h>
#include <Misc/Util.h>
#include <iostream>

namespace OSSIA
{
  bool operator==(const State& lhs, const State& rhs)
  {
      return lhs.children == rhs.children;
  }

  bool operator!=(const State& lhs, const State& rhs)
  {
      return lhs.children != rhs.children;
  }


  std::ostream& print(std::ostream& out, const StateElement& e)
  {
      if(e)
          eggs::variants::apply(StatePrintVisitor{out}, e);
      else
          out << "no state";
      return out;
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

  void launch(const StateElement& s)
  {
      if(s)
          eggs::variants::apply(StateExecutionVisitor{}, s);
  }

  std::size_t State::size() const { return children.size(); }

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
