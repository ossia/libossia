#pragma once
#include <ossia/editor/dataspace/dataspace.hpp>

#include <vector>
#include <functional>
#include <ossia_export.h>

/**
 * @file functional_state.hpp
 *
 * This file describes a lazy way to perform chains of computations
 * in the state paradigm.
 *
 * The goal is to have calls chained at the time of executions.
 * The input will be given by the first one.
 * Each state has a priority and an override status.
 *
 * States of lesser priority will happen sooner in the call chain.
 *
 * e.g. if f1 has priority 1 and f2 has priority 2,
 *
 * @see \ref ossia::functional_state
 * @see \ref ossia::functional_state_composition
 */

namespace ossia
{
class OSSIA_EXPORT functional_state
{
public:
  std::vector<ossia::Destination> inputs;
  std::vector<ossia::Destination> outputs;
  int priority = 0;
  bool override = false;

  std::function<void (std::vector<ossia::value_with_unit>&)> func;

  void launch() const
  {
    // 1. Get the value of the first ones, maybe with a pull ?
    std::vector<ossia::value_with_unit> vals;

    vals.reserve(inputs.size());
    for(auto& dest : inputs)
    {
      vals.push_back(ossia::net::get_value(dest));
    }

    // 2. Apply
    if(func)
      func(vals);

    // 3. Push the values of the last one.
    auto n_min = std::min(vals.size(), outputs.size());

    for(int i = 0 ; i < n_min ; i++)
    {
      ossia::net::push_value(outputs[i], vals[i]);
    }

  }

  friend bool operator==(const functional_state& lhs, const functional_state& rhs)
  {
    return false;
  }
  friend bool operator!=(const functional_state& lhs, const functional_state& rhs)
  {
    return true;
  }
};


class OSSIA_EXPORT functional_state_composition
{
public:
  std::vector<functional_state> call_chain;
  // priority ? priority relative to messages ?

  void launch() const
  {
    if(call_chain.size() > 0)
    {
      // 1. Get the value of the first ones, maybe with a pull ?
      std::vector<ossia::value_with_unit> vals;
      auto& inputs = call_chain.front().inputs;
      vals.reserve(inputs.size());
      for(auto& dest : inputs)
      {
        vals.push_back(ossia::net::get_value(dest));
      }

      // 2. Apply recursively
      for(auto& e : call_chain)
      {
        if(e.func)
          e.func(vals);
      }

      // 3. Push the values of the last one.
      auto& outputs = call_chain.back().outputs;
      auto n_min = std::min(vals.size(), outputs.size());

      for(int i = 0 ; i < n_min ; i++)
      {
        ossia::net::push_value(outputs[i], vals[i]);
      }
    }
  }
};
}
}
