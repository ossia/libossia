#pragma once
#include <ossia/detail/config.hpp>

namespace ossia
{
OSSIA_EXPORT
void disable_fpe();
OSSIA_EXPORT
void reset_default_fpu_state();
}
