#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/data.hpp>
namespace ossia
{
inline const ossia::value& last(const ossia::value_vector<ossia::timed_value>& vec)
{
  auto max = vec[0].timestamp;
  const ossia::timed_value* ptr{&vec[0]};
  for(auto& e : vec)
  {
    if(e.timestamp < max)
    {
      max = e.timestamp;
      ptr = &e;
    }
  }
  return ptr->value;
}

}
