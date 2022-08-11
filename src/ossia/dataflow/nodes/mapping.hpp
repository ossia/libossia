#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/editor/curve/behavior.hpp>
#include <ossia/editor/mapper/detail/mapper_visitor.hpp>
/**
 * \file mapper.hpp
 */

namespace ossia::nodes
{
/**
 * @brief The ossia::nodes::mapping class
 *
 * Allows to map a value to another following a transfer function.
 * The driver address is where the input value is taken from;
 * The driven address is where the output value is sent to.
 */

class mapping final : public ossia::nonowning_graph_node
{
public:
  mapping()
  {
    m_inlets.push_back(&value_in);
    m_outlets.push_back(&value_out);
  }

  [[nodiscard]] std::string label() const noexcept override { return "mapping"; }

  ~mapping() override = default;

  void set_behavior(const ossia::behavior& b) { m_drive = b; }

private:
  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    if(!m_drive)
      return;

    const ossia::value_port& ip = *value_in;
    ossia::value_port& op = *value_out;

    // TODO use correct unit / whatever ?
    for(auto& tv : ip.get_data())
    {
      if(tv.value.valid())
        try
        {
          auto v = ossia::apply(
              ossia::detail::mapper_compute_visitor{}, tv.value, m_drive.v);

          op.write_value(std::move(v), tv.timestamp);
        }
        catch(...)
        {
        }
    }
  }

  ossia::behavior m_drive;
  ossia::value_inlet value_in;
  ossia::value_outlet value_out;
};
}
