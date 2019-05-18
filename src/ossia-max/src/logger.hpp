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
  t_symbol* m_host;
  t_symbol* m_appname;
  t_symbol* m_exec;
  long m_ival{};

  logger(long argc, t_atom* argv);

  void reset();

  static void in_anything(ossia::max::logger* x, t_symbol* s, long argc, t_atom* argv);
  static void free(ossia::max::logger* x);
  static t_max_err notify(logger *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

  static void assist(logger* x, void* b, long m, long a, char* s);

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
}
