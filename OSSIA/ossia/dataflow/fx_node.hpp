#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/data.hpp>
namespace ossia
{
inline const ossia::value& last(const ossia::value_vector<ossia::tvalue>& vec)
{
  auto max = vec[0].timestamp;
  const ossia::tvalue* ptr{&vec[0]};
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
