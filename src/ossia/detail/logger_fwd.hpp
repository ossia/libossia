#pragma once
// Important defines :
#include <ossia/detail/config.hpp>

#include <memory>

#if !defined(OSSIA_HAS_FMT)
#define OSSIA_BRUH_LOGGER 1
#endif

#if !defined(OSSIA_BRUH_LOGGER)
namespace spdlog
{
class logger;
}

namespace ossia
{
using logger_type = spdlog::logger;
}
#else
#include <memory>
#include <vector>
namespace spdlog
{
namespace sinks
{
class sink;
}
}
namespace ossia
{
struct bruh_logger;
using logger_type = bruh_logger;
}
#endif

namespace ossia
{
/**
 * @brief Where the errors will be logged. Default is stderr.
 *
 * It is also accessible through spdlog::get("ossia");
 *
 * The logger is first created in ossia::context construction.
 */
OSSIA_EXPORT logger_type& logger() noexcept;
OSSIA_EXPORT std::shared_ptr<logger_type> logger_ptr() noexcept;

}
