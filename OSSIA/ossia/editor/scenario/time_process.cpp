#include <ossia/editor/scenario/time_process.hpp>

namespace ossia
{
time_process::~time_process()
{
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
  mUnmuted = !m;
  mute_impl(m);
}

bool time_process::unmuted() const
{
  return mUnmuted;
}

time_constraint*time_process::parent() const
{
  return mParent;
}

int32_t time_process::getPriority() const
{
  return mPriority;
}

void time_process::setPriority(int32_t i)
{
  mPriority = i;
}

bool time_process::getPriorityOverride() const
{
  return mPriorityOverride;
}

void time_process::setPriorityOverride(bool o)
{
  mPriorityOverride = o;
}

void time_process::mute_impl(bool)
{

}
}
