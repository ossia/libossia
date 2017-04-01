#pragma once
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph_edge.hpp>

namespace ossia
{

class OSSIA_EXPORT graph_node
{
protected:
  // Note : pour QtQuick : Faire View et Model qui hérite de View, puis faire binding automatique entre propriétés de la vue et du modèle
  // Utiliser... DSPatch ? Pd ?
  // Ports : midi, audio, value

public:
  inlets in_ports;
  outlets out_ports;

  double previous_time{};
  double time{};

  bool m_enabled{};
  bool m_executed{};
  virtual ~graph_node()
  {
    // TODO moveme in cpp
  }

  graph_node()
  {
  }

  bool enabled() const { return m_enabled; }
  void set_enabled(bool b) { m_enabled = b; }

  bool executed() const { return m_executed; }
  void set_executed(bool b) { m_executed = b; }

  virtual bool consumes(const std::string&)
  {
    return false;
  }
  virtual bool consumes(const Destination&)
  {
    return false;
  }
  virtual bool consumes(execution_state&)
  {
    return false;
  }

  virtual void run(execution_state&)
  {

  }

  void set_time(double d)
  {
    previous_time = time;
    time = d;
  }

  bool can_execute(const execution_state&) const
  {
    return ossia::all_of(in_ports, [] (const auto& inlet) {
      // A port can execute if : - it has source ports and all its source ports have executed
      bool previous_nodes_executed = ossia::all_of(inlet->sources, [&] (graph_edge* edge) {
          return edge->out_node->executed() || (!edge->out_node->enabled() && bool(inlet->address)
                                                /* TODO check that it's in scope */);
      });

      // it does not have source ports ; we have to check for variables :
      // find if address available in local / global scope
      return !inlet->sources.empty()
          ? previous_nodes_executed
          : true // TODO
            ;
    });
  }



  const auto& inputs() const { return in_ports; }
  const auto& outputs() const { return out_ports; }
};

}
