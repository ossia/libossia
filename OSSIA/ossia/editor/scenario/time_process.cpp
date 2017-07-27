// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia
{
time_process::~time_process()
{
}

state_element time_process::offset(time_value date, double pos)
{
  return state(date, pos);
}

void time_process::start(ossia::state& st)
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

void time_process::mute_impl(bool)
{
}
}
