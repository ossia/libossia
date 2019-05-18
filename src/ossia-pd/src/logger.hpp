#pragma once
#include <ossia-pd/src/object_base.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>

namespace ossia
{
namespace pd
{

class logger : public object_base
{
public:

  t_symbol* m_host;
  t_symbol* m_appname;
  t_symbol* m_exec;
  int m_ival{};

  logger(int argc, t_atom* argv);

  void reset();

  static void in_anything(ossia::pd::logger* x, t_symbol* s, int argc, t_atom* argv);
  static void destroy(ossia::pd::logger* x);
  static t_pd_err notify(logger *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

  static void* create(t_symbol* s, int argc, t_atom* argv);

  std::shared_ptr<ossia::websocket_threaded_connection> m_con;
  std::shared_ptr<spdlog::logger> m_log;
  std::shared_ptr<ossia::websocket_heartbeat> m_beat;

};
} // pd namespace
} // ossia namespace

