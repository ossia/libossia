#include <ossia/editor/scenario/detail/Clock_impl.hpp>

using namespace OSSIA;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  std::shared_ptr<Clock> Clock::create(Clock::ExecutionCallback callback,
                                  TimeValue duration,
                                  TimeValue granularity,
                                  TimeValue offset,
                                  float speed,
                                  Clock::DriveMode driveMode)
  {
    return std::make_shared<impl::JamomaClock>(callback, duration, granularity, offset, speed, driveMode);
  }

  Clock::~Clock() = default;
}
