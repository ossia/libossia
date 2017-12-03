#pragma once
#include <ossia/dataflow/graph_node.hpp>
namespace ossia
{
class OSSIA_EXPORT audio_fx_node : public ossia::graph_node
{
  public:
    using ossia::graph_node::graph_node;
    virtual ~audio_fx_node();
    virtual void all_notes_off() = 0;
};

static const ossia::value& last(const ossia::value_vector<ossia::tvalue>& vec)
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
