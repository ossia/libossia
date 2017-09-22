// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/automation/metronome.hpp>
#include <ossia/editor/curve/curve_segment/easing.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia
{
metronome::metronome()
{
}

void metronome::set_destination(destination a)
{
  m_parameter = std::move(a);
}

void metronome::set_curve(std::shared_ptr<curve<double, float>> b)
{
  m_curve = std::move(b);
}

template<typename Fun>
struct fun_on_destruction
{
  Fun f;
  ~fun_on_destruction() { f(); }
};
template<typename Fun>
static auto on_destruction(Fun fun) { return fun_on_destruction<Fun>{std::move(fun)}; }

state_element metronome::state(ossia::time_value date, double pos)
{
  auto _{on_destruction([&] { m_lastDate = date; })};
  if (m_parameter)
  {
    auto& addr = *m_parameter;
    // Get the "tick speed" at the current position
    // Compute the date of the next tick with a comparison to the last tick.
    // If it is before the current time_value, send it, else wait.

    // cur: Tick speed in time_values.
    // 1 = 1 tick per time_value. 1000 = 1 tick per 1000 time_value.
    time_value cur{m_curve->value_at(pos)};

    // TODO we should compute the derivative since the last tick in order to be precise
    if(date > m_lastDate) {
      time_value elapsed = date - m_lastDate;
      if(m_lastTick + elapsed < cur) {
        // not yet
        m_lastTick += elapsed;
        return {};
      }
      else
      {
        m_lastTick = elapsed - cur;
        return ossia::message{addr, ossia::impulse{}};
      }
    }
    else if(date < m_lastDate) {
      time_value elapsed = m_lastDate - date;
      if(m_lastTick + elapsed < cur) {
        // not yet
        m_lastTick += elapsed;
        return {};
      }
      else
      {
        m_lastTick = elapsed - cur;
        return ossia::message{addr, ossia::impulse{}};
      }
    }
  }
  return {};
}

void metronome::start(ossia::state& st)
{
  m_lastDate = 0_tv;
  m_lastTick = 0_tv;
}

void metronome::stop()
{
}

void metronome::pause()
{
}

void metronome::resume()
{
}
}
