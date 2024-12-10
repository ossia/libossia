// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "context.hpp"

#include <ossia/detail/logger.hpp>

#if defined(OSSIA_HAS_FMT)
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#endif

#if defined(QT_QML_LIB)
#include <ossia-qt/qml_plugin.hpp>
#endif
#include <ossia/detail/any_map.hpp>
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/thread.hpp>

#include <smallfun.hpp>

#include <memory>

#if defined(_MSC_VER)
#include <boost/asio/impl/src.hpp>
#endif

// https://svn.boost.org/trac10/ticket/3605
#if defined(_MSC_VER)
#include <boost/asio/detail/winsock_init.hpp>
#pragma warning(push)
#pragma warning(disable : 4073)
#pragma init_seg(lib)
boost::asio::detail::winsock_init<2, 2>::manual manual_winsock_init;
#pragma warning(pop)
#elif defined(_WIN32)
#include <boost/asio/detail/winsock_init.hpp>
#endif

#if !defined(__cpp_exceptions)
#include <boost/throw_exception.hpp>
namespace boost
{
void throw_exception(std::exception const& e)
{
  std::terminate();
}
void throw_exception(std::exception const& e, boost::source_location const& loc)
{
  std::terminate();
}
}
#endif
namespace ossia
{
#if defined(__cpp_exceptions)
invalid_callback_error::invalid_callback_error() = default;
invalid_callback_error::~invalid_callback_error() = default;
const char* invalid_callback_error::what() const noexcept
{
  return "Bad callback";
}
#endif

static void ossia_global_init()
{
  static bool init = false;
  if(!init)
  {
    // Create a logger for the library.
    logger();

    // Init WinSock
#if defined(_WIN32) && !defined(_MSC_VER)
    static boost::asio::detail::winsock_init<>::manual manual_winsock_init;
#endif

// Register QML types
#if defined(QT_QML_LIB)
    qt::qml_plugin::reg("Ossia");
#endif

    ossia::set_thread_pinned(thread_type::Ui, 0);
  }
}

context::context()
{
  ossia_global_init();
}

context::context(const std::vector<spdlog::sink_ptr>& sinks)
{
  auto log = logger_ptr();
  log->sinks() = sinks;
  ossia_global_init();
}

context::~context() = default;

#if defined(OSSIA_BRUH_LOGGER)
bruh_logger& logger() noexcept
{
  static bruh_logger b;
  return b;
}

std::shared_ptr<bruh_logger> logger_ptr() noexcept
{
  return std::make_shared<bruh_logger>();
}
std::vector<std::shared_ptr<spdlog::sinks::sink>>& bruh_logger::sinks()
{
  static thread_local std::vector<std::shared_ptr<spdlog::sinks::sink>> s;
  return s;
}
#else
spdlog::logger& logger() noexcept
{
  static spdlog::logger& init = []() -> spdlog::logger& { return *logger_ptr(); }();

  return init;
}

std::shared_ptr<spdlog::logger> logger_ptr() noexcept
try
{
  if(auto logger = spdlog::get("ossia"))
    return logger;
  else
    return spdlog::stderr_logger_mt("ossia");
}
catch(...)
{
  return spdlog::null_logger_mt("ossia");
}
#endif
}
