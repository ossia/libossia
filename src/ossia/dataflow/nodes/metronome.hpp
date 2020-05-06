#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/editor/curve/curve.hpp>

namespace ossia::nodes
{
class OSSIA_EXPORT metronome final : public ossia::nonowning_graph_node
{
public:
  metronome()
  {
    m_outlets.push_back(&value_out);
  }

  ~metronome() override
  {
  }

  std::string label() const noexcept override
  {
    return "metronome";
  }

  void set_curve(std::shared_ptr<curve<double, float>> b)
  {
    m_curve = std::move(b);
  }

  void reset()
  {
    m_metroPrevTick = ossia::time_value{};
  }

private:
  void
  run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    ossia::value_port& vp = *value_out;
    const auto date = t.date;
    const auto pos = t.position();

    // TODO use a better temporal accuracy ?

    // Get the "tick speed" at the current position
    // Compute the date of the next tick with a comparison to the last tick.
    // If it is before the current time_value, send it, else wait.

    // cur: Tick speed in time_values.
    // 1 = 1 tick per time_value. 1000 = 1 tick per 1000 time_value.
    time_value cur{int64_t(m_curve->value_at(pos))};

    // TODO we should compute the derivative since the last tick in order to be
    // precise
    if (date > t.prev_date)
    {
      time_value elapsed = date - t.prev_date;
      if (m_metroPrevTick + elapsed < cur)
      {
        // not yet
        m_metroPrevTick += elapsed;
        return;
      }
      else
      {
        m_metroPrevTick = elapsed - cur;
        vp.write_value(
            ossia::impulse{}, t.physical_start(e.modelToSamples())); // TODO offset is wrong here
      }
    }
    else if (date < t.prev_date)
    {
      time_value elapsed = t.prev_date - date;
      if (m_metroPrevTick + elapsed < cur)
      {
        // not yet
        m_metroPrevTick += elapsed;
        return;
      }
      else
      {
        m_metroPrevTick = elapsed - cur;
        vp.write_value(
            ossia::impulse{}, t.physical_start(e.modelToSamples())); // TODO offset is wrong here
      }
    }
  }

  std::shared_ptr<curve<double, float>> m_curve;
  ossia::value_outlet value_out;
  time_value m_metroPrevTick{};
};

class metronome_process final : public ossia::node_process
{
public:
  using ossia::node_process::node_process;
  void start() override
  {
    static_cast<ossia::nodes::metronome*>(node.get())->reset();
  }
};
}
