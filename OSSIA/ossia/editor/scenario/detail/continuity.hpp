#pragma once
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/loop/loop.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/dataflow/graph_node.hpp>
namespace ossia
{

template<void (graph_node::*fun_T)(bool)>
struct mark_discontinuous
{
    void operator()(loop& l) const
    {
      (l.node.get()->*fun_T)(true);
      (*this)(l.get_time_interval());
    }
    void operator()(scenario& s) const
    {
      (s.node.get()->*fun_T)(true);
      for(auto& itv : s.get_time_intervals()) {
        if(!itv->node->requested_tokens.empty()) {
          (*this)(*itv);
        }
      }
    }
    void operator()(time_interval& itv) const
    {
      (itv.node.get()->*fun_T)(true);
      for(auto& proc : itv.get_time_processes()) {
        if(proc->node) {
          (proc->node.get()->*fun_T)(true);
          auto ptr = proc->node.get();
          if(auto scenar = dynamic_cast<scenario*>(ptr))
            (*this)(*scenar);
          if(auto l = dynamic_cast<loop*>(ptr))
            (*this)(*l);
        }
      }
    }
};
using mark_start_discontinuous = mark_discontinuous<&graph_node::set_start_discontinuous>;
using mark_end_discontinuous = mark_discontinuous<&graph_node::set_end_discontinuous>;

}
