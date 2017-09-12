#pragma once
#include "ext.h"
#include "ext_obex.h"
#undef error
#undef post
#include <ossia/network/common/websocket_log_sink.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_logger structure declaration

struct logger
{
  t_object m_object;

  logger(long argc, t_atom* argv);

  std::shared_ptr<ossia::websocket_threaded_connection> m_con;
  std::shared_ptr<spdlog::logger> m_log;
  std::shared_ptr<ossia::websocket_heartbeat> m_beat;
};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_logger class declaration

extern "C" {
void* ossia_logger_new(t_symbol* s, long argc, t_atom* argv);
void ossia_logger_in_anything(
    ossia::max::logger* x, t_symbol* s, long argc, t_atom* argv);
void ossia_logger_free(ossia::max::logger* x);
}
