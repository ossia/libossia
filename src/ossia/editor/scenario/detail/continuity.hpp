#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/loop/loop.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
namespace ossia
{

template <void (graph_node::*fun_T)(bool)>
struct mark_discontinuous
{
  void operator()(scenario& s) const
  {
#if defined(OSSIA_SCENARIO_DATAFLOW)
    (s.node.get()->*fun_T)(true);
    for(auto& itv : s.get_time_intervals())
    {
      if(!itv->node->requested_tokens.empty())
      {
        (*this)(*itv);
      }
    }
#endif
  }
  void operator()(time_interval& itv) const
  {
#if defined(OSSIA_SCENARIO_DATAFLOW) && defined(__cpp_rtti)
    (itv.node.get()->*fun_T)(true);
    for(auto& proc : itv.get_time_processes())
    {
      if(proc->node)
      {
        (proc->node.get()->*fun_T)(true);
        auto ptr = proc->node.get();
        if(auto scenar = dynamic_cast<scenario*>(ptr))
          (*this)(*scenar);
      }
    }
#endif
  }
};
using mark_start_discontinuous
    = mark_discontinuous<&graph_node::set_start_discontinuous>;
using mark_end_discontinuous = mark_discontinuous<&graph_node::set_end_discontinuous>;
}
