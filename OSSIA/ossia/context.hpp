#pragma once
#include <ossia_export.h>
#include <ossia/detail/logger.hpp>

namespace ossia
{
/**
 * @brief If using the library, you should create this class at some point.
 *
 * It is not mandatory but some features won't work.
 */
struct OSSIA_EXPORT context
{
  context();

  ~context();
};
}
