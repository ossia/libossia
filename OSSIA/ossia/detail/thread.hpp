#pragma once
#include <thread>
#include <ossia/detail/config.hpp>
#include <ossia_export.h>

namespace ossia
{
OSSIA_EXPORT
void set_thread_realtime(std::thread& t);
}
