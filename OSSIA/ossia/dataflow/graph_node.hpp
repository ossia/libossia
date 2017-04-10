#pragma once
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph_edge.hpp>

namespace ossia
{

class OSSIA_EXPORT graph_node
{
public:
  graph_node();
  virtual ~graph_node();

  bool enabled() const;
  void set_enabled(bool b);

  bool executed() const;
  void set_executed(bool b);

  virtual bool consumes(const std::string&) const;
  virtual bool consumes(const Destination&) const;
  virtual bool consumes(const execution_state&) const;

  virtual void run(execution_state&);

  void set_time(double d);
  double time() const { return m_time; }

  bool can_execute(const execution_state&) const;

  bool has_port_inputs() const;
  bool has_global_inputs() const;
  bool has_local_inputs(const execution_state&) const;


  const inlets& inputs() const;
  const outlets& outputs() const;

  void clear();

protected:
  // Note : pour QtQuick : Faire View et Model qui hérite de View,
  // puis faire binding automatique entre propriétés de la vue et du modèle
  // Utiliser... DSPatch ? Pd ?
  // Ports : midi, audio, value

  inlets m_inlets;
  outlets m_outlets;

  double m_previous_time{};
  double m_time{};

  bool m_enabled{};
  bool m_executed{};

};

}
