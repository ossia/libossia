// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "context.hpp"

#include <ossia/detail/logger.hpp>

#include <spdlog/spdlog.h>
#if defined(QT_QML_LIB)
#include <ossia-qt/qml_plugin.hpp>
#endif
#include <ossia/detail/any_map.hpp>
#include <ossia/detail/callback_container.hpp>

#include <smallfun.hpp>

#include <iostream>
#include <memory>

// https://svn.boost.org/trac10/ticket/3605
#if defined(_MSC_VER)
#include <asio/detail/winsock_init.hpp>
#pragma warning(push)
#pragma warning(disable : 4073)
#pragma init_seg(lib)
asio::detail::winsock_init<>::manual manual_winsock_init;
#pragma warning(pop)
#endif

#if !defined(_MSC_VER)
template class OSSIA_EXTERN_EXPORT_CPP(OSSIA_EXPORT) tsl::hopscotch_map<
    std::string, ossia::any, ossia::string_hash, ossia::string_equal,
    std::allocator<std::pair<std::string, ossia::any>>, 4>;
#endif
namespace ossia
{
invalid_callback_error::~invalid_callback_error()
{
}

static void ossia_global_init()
{
  static bool init = false;
  if (!init)
  {
    // Create a logger for the library.
    logger();

    // Init WinSock
#if defined(_MSC_VER)
    WSADATA wsa_data;
    ::WSAStartup(MAKEWORD(2, 0), &wsa_data);
#endif

// Register QML types
#if defined(QT_QML_LIB)
    qt::qml_plugin::reg("Ossia");
#endif
  }
}

context::context()
{
  ossia_global_init();
}

context::context(const std::vector<spdlog::sink_ptr>& sinks)
{
  spdlog::create("ossia", sinks.begin(), sinks.end());
  ossia_global_init();
}

context::~context() = default;

spdlog::logger& logger() noexcept
{
  static spdlog::logger& init
      = []() -> spdlog::logger& { return *logger_ptr(); }();

  return init;
}

std::shared_ptr<spdlog::logger> logger_ptr() noexcept
{
  if (auto logger = spdlog::get("ossia"))
    return logger;
  else
    return spdlog::stderr_logger_mt("ossia");
}
}
