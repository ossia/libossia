// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/editor/scenario/time_process.hpp>

namespace ossia
{
time_process::~time_process() = default;

// The process is about to play from somewhere else: what its node holds from
// where it was is not going to be stopped by what comes next.
static void release_notes_on_jump(time_process& p)
{
#if defined(OSSIA_SCENARIO_DATAFLOW)
  if(p.node)
    p.node->request_note_release();
#endif
}

void time_process::offset(time_value date)
{
  // TODO looping
  offset_impl(date);
  release_notes_on_jump(*this);
}

void time_process::transport(time_value date)
{
  transport_impl(date, ossia::tick_transport_info{});
  release_notes_on_jump(*this);
}

void time_process::transport(time_value date, const tick_transport_info& info)
{
  transport_impl(date, info);
  release_notes_on_jump(*this);
}

void time_process::start() { }

void time_process::stop() { }

void time_process::pause() { }

void time_process::resume() { }

void time_process::mute(bool m)
{
  m_unmuted = !m;
#if defined(OSSIA_SCENARIO_DATAFLOW)
  if(node)
    node->set_mute(m);
#endif
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

void time_process::mute_impl(bool) { }
}
