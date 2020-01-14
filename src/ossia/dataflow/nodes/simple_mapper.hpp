#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/editor/curve/behavior.hpp>
#include <ossia/editor/curve/curve_segment/easing.hpp>
#include <ossia/editor/mapper/detail/mapper_visitor.hpp>

#include <ossia_export.h>
namespace ossia
{
template <typename Y, typename Easing>
struct value_mapping
{
  Y operator()(double ratio, Y start, Y end) const
  {
    return easing::ease{}(start, end, Easing{}(ratio));
  }
};
}
namespace ossia::nodes
{
class OSSIA_EXPORT simple_mapping final : public ossia::nonowning_graph_node
{
public:
  simple_mapping()
  {
    m_inlets.push_back(&value_in);
    m_inlets.push_back(&input_min);
    m_inlets.push_back(&input_max);
    m_inlets.push_back(&output_min);
    m_inlets.push_back(&output_max);
    m_inlets.push_back(&curve_in);
    m_outlets.push_back(&value_out);
  }

  std::string label() const noexcept override
  {
    return "simple_mapping";
  }

  ~simple_mapping() override
  {
  }

private:
  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    if (!m_drive)
      return;

    // TODO use correct unit / whatever ?
    for (auto& tv : value_in.get_data())
    {
      if (tv.value.valid())
      {
        auto v = ossia::apply(
            ossia::detail::mapper_compute_visitor{}, tv.value, m_drive.v);

        op.write_value(std::move(v), tv.timestamp);
      }
    }
  }

  ossia::behavior m_drive;

  ossia::value_inlet value_in;
  ossia::value_inlet input_min;
  ossia::value_inlet input_max;
  ossia::value_inlet output_min;
  ossia::value_inlet output_max;
  ossia::value_inlet curve_kind;

  ossia::value_outlet value_out;

  using map_func = float(*)(float in, float min_in, float max_in, float min_out, float max_out);
  ossia::flat_map<std::string, map_func> m_funcs;
};
}
