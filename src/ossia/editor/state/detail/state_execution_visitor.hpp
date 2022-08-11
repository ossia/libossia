#pragma once
#include <ossia/detail/nullable_variant.hpp>

#include <utility>

namespace ossia
{
struct state_execution_visitor
{
  template <typename T>
  void operator()(T&& m)
  {
    std::forward<T>(m).launch();
  }

  void operator()(ossia::monostate) { }
  void operator()() { }
};
}
