#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/editor/curve/behavior.hpp>
#include <ossia/editor/automation/curve_value_visitor.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia_export.h>

/**
 * \file automation.hpp
 */
namespace ossia
{

/**
 * \brief The automation_node class
 *
 * An automation is a time process that represents a curve of one
 * or multiple parameters.
 *
 * It drives (i.e. sends messages) to a parameter, according to a driving value.
 * Its driven \ref net::parameter_base can be either of numeric type (\ref
 * val_type::INT, \ref val_type::FLOAT...)
 * or of \ref val_type::LIST, if the list only has numeric elements (e.g.
 * List{Float, Float}).
 *
 * The driving \ref value can either be a single \ref Behavior or a \ref List
 * of \ref Behavior,
 * in accordance to the type of the driven \ref net::parameter_base.
 *
 * The automation has a "source" domain, i.e. the data space in which the
 * transformation
 * happens. The target domain is taken from the driven parameter_base.
 * The unit is stored in m_lastMessage.unit.
 *
 *
 * \see \ref behavior \ref curve \ref curve_segment
 */
class OSSIA_EXPORT automation_node final :
    public ossia::graph_node
{
  public:
    automation_node()
    {
      m_outlets.push_back(ossia::make_outlet<ossia::value_port>());
    }

    ~automation_node() override
    {

    }
    std::string label() const override
    {
      return "automation";
    }

    void set_destination(optional<ossia::destination> d)
    {
      auto& port = *m_outlets.front();
      auto& vp = *port.data.target<ossia::value_port>();
      if(d)
      {
        port.address = &d->address();
        vp.type = d->unit;
        vp.index = d->index;
      }
      else
      {
        port.address = {};
        vp.type = ossia::val_type::FLOAT;
        vp.index = {};
      }
    }

    void set_behavior(const ossia::behavior& b)
    {
      m_drive = b;
    }

    void reset_drive()
    {
      m_drive.reset();
    }

  private:
    void run(ossia::token_request t, ossia::execution_state& e) override
    {
      if(!m_drive)
        return;

      auto& outlet = *m_outlets[0];
      ossia::value_port* vp = outlet.data.target<ossia::value_port>();
      vp->add_value(
            ossia::apply(
              ossia::detail::compute_value_visitor{t.position, ossia::val_type::FLOAT},
              m_drive), t.date);
    }

    ossia::behavior m_drive;
};

class automation_process : public ossia::node_process
{
public:
    using ossia::node_process::node_process;
  void start() override
  {
      static_cast<automation_node*>(node.get())->reset_drive();
  }
};
}
