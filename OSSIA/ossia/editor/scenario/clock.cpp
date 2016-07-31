#include <ossia/editor/scenario/detail/Clock_impl.hpp>
namespace ossia
{
  std::shared_ptr<clock> clock::create(
      clock::ExecutionCallback callback,
                                  time_value duration,
                                  time_value granularity,
                                  time_value offset,
                                  float speed,
                                  clock::DriveMode driveMode)
  {
    return std::make_shared<detail::clock_impl>(callback, duration, granularity, offset, speed, driveMode);
  }

  clock::~clock() = default;
}
