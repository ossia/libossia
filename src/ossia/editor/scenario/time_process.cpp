// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/scenario/time_process.hpp>

namespace ossia
{
time_process::~time_process()
{
}

void time_process::offset(time_value date, double pos)
{
  // TODO looping
  offset_impl(date, pos);
}

void time_process::transport(time_value date, double pos)
{
  transport_impl(date, pos);
}

void time_process::state(
    time_value from,
    time_value to,
    ossia::time_value parent_dur,
    time_value tick_offset,
    double gspeed)
{
  if(!m_loops)
  {
    state_impl(from + m_start_offset, to + m_start_offset, parent_dur, tick_offset, gspeed);
  }
  else
  {
    // TODO this breaks relative_position
    ossia::time_value tick_amount = to - from;

    while (tick_amount > 0)
    {
      const auto cur_from = from % m_loop_duration;
      if (cur_from + tick_amount < m_loop_duration)
      {
        state_impl(time_value{cur_from + m_start_offset},
                   time_value{cur_from + tick_amount + m_start_offset},
                   parent_dur,
                   tick_offset,
                   gspeed);
        break;
      }
      else
      {
        auto this_tick = m_loop_duration - cur_from;

        tick_amount -= this_tick;
        from += this_tick;
        state_impl(time_value{cur_from + m_start_offset},
                   time_value{cur_from + this_tick + m_start_offset},
                   parent_dur,
                   tick_offset,
                   gspeed);
        tick_offset += this_tick;
      }
    }
  }
}

void time_process::start()
{
}

void time_process::stop()
{
}

void time_process::pause()
{
}

void time_process::resume()
{
}

void time_process::mute(bool m)
{
  m_unmuted = !m;
  if (node)
    node->set_mute(m);
  mute_impl(m);
}

bool time_process::unmuted() const
{
  return m_unmuted;
}

void time_process::enable(bool m)
{
  m_enabled = m;
}

bool time_process::enabled() const
{
  return m_enabled;
}

void time_process::set_loops(bool b)
{
  m_loops = b;
}

void time_process::set_start_offset(time_value v)
{
  m_start_offset = v;
}

void time_process::set_loop_duration(time_value v)
{
  m_loop_duration = v;
}

void time_process::mute_impl(bool)
{
}
}
