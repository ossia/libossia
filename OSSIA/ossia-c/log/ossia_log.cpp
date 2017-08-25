// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_log.h"
#include <ossia-c/ossia-c.h>
#include <cstdio>
#include <memory>
#include <ossia/network/common/websocket_log_sink.hpp>


extern "C" {
void basic_log(const char* error)
{
  std::fprintf(stderr, "%s\n", error);
}

static ossia_log_fun_t log_fun = basic_log;
void ossia_set_debug_logger(ossia_log_fun_t fp)
{
  log_fun = fp;
}

void ossia_log_error(const char* error)
{
  if (log_fun)
    log_fun(error);
}

struct ossia_logger
{
    ossia_logger(std::string host, std::string name)
      : connection{std::make_shared<ossia::websocket_threaded_connection>(host)}
      , logger{"", std::make_shared<ossia::websocket_log_sink>(connection, name)}
      , heartbeat{connection, "name", std::chrono::seconds(5)}
    {

    }

    std::shared_ptr<ossia::websocket_threaded_connection> connection;
    spdlog::logger logger;
    ossia::websocket_heartbeat heartbeat;
};

ossia_logger_t ossia_logger_create(const char* host, const char* app)
{
  return new ossia_logger{host, app};
}
void ossia_logger_init_heartbeat(ossia_logger_t log, int pid, const char* cmdline)
{
  log->heartbeat.send_init({
                             {"pid", pid}
                           , {"cmd", cmdline}
                           });
}

void ossia_logger_set_level(ossia_logger_t log, log_level lvl)
{
  log->logger.set_level((spdlog::level::level_enum)lvl);
}

void ossia_log(ossia_logger_t log, log_level lvl, const char* message)
{
  switch(lvl)
  {
    case log_level::trace: log->logger.trace("{}", message); break;
    case log_level::debug: log->logger.debug("{}", message); break;
    case log_level::info: log->logger.info("{}", message); break;
    case log_level::warn: log->logger.warn("{}", message); break;
    case log_level::error: log->logger.error("{}", message); break;
    case log_level::critical: log->logger.critical("{}", message); break;
    case log_level::off: break;
  }
}

void ossia_logger_free(ossia_logger_t log)
{
  delete log;
}
}
