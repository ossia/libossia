// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-pd/src/logger.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>
#include "ossia-pd.hpp"
#include <ossia/detail/thread.hpp>

namespace ossia
{
namespace pd
{

logger::logger(int argc, t_atom* argv) :
  object_base{ossia_pd::logger_class}
{
  m_host = gensym("ws://127.0.0.1:1337");
  m_appname = gensym("pd");
  m_ival = 5;

  reset();
}

void* logger::create(t_symbol* s, int argc, t_atom* argv)
{
  return new logger(argc, argv);
}

void logger::in_anything(logger* x, t_symbol* s, int argc, t_atom* argv)
{
  if (x && x->m_log && s && s->s_name && argc > 0)
  {
    ossia::string_view type = s->s_name;

    std::string txt;
    for(int i = 0; i < argc; i++)
    {
      switch(argv[i].a_type) {
      case A_FLOAT:
        txt += std::to_string(atom_getfloat(&argv[i]));
        break;
      case A_SYMBOL:
        txt += std::string(atom_getsym(&argv[i])->s_name);
        break;
      }
      txt += " ";
    }

    if(type == "info")
    {
      x->m_log->info("{}", txt);
    }
    else if(type == "warn")
    {
      x->m_log->warn("{}", txt);
    }
    else if(type == "debug")
    {
      x->m_log->debug("{}", txt);
    }
    else if(type == "error")
    {
      x->m_log->error("{}", txt);
    }
    else if(type == "critical")
    {
      x->m_log->critical("{}", txt);
    }
    else if(type == "trace")
    {
      x->m_log->trace("{}", txt);
    }
  }
}

void logger::destroy(logger* x)
{
  if (x)
  {
    x->~logger();
  }
}

t_max_err logger::notify(
    logger* x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
  x->reset();
  return 0;
}

void logger::reset()
{
  logpost(&m_obj, 4, "logger reset %s %s", m_host->s_name, m_appname->s_name);
  m_log.reset();
  m_beat.reset();
  m_con.reset();

  if(!m_host)
    return;
  if(!m_appname)
    return;

  ossia::string_view host = m_host->s_name;
  std::string appname = m_appname->s_name;
  m_con = std::make_shared<ossia::websocket_threaded_connection>(std::string(host));

  logpost(&m_obj, 4, "logger connected");
  m_log = std::make_shared<spdlog::logger>(
       "max_logger", std::make_shared<websocket_log_sink>(m_con, appname));

  logpost(&m_obj, 4, "logger created");
  m_beat = std::make_shared<websocket_heartbeat>(
        m_con,
        appname,
        std::chrono::seconds(ossia::clamp((int)m_ival, (int)0, (int)1000)));

  logpost(&m_obj, 4, "heartbeat connected");
  m_beat->send_init({
                        {"pid", ossia::get_pid()}
                      , {"cmd", ossia::get_exe_path()}
                    });

  logpost(&m_obj, 4, "heartbeat init");
}


extern "C" void setup_ossia0x2elogger(void)
{
  // instantiate the ossia.logger class

  t_eclass* c = eclass_new(
      "ossia.logger", (method)logger::create, (method)logger::destroy,
      (short)sizeof(logger), CLASS_DEFAULT, A_GIMME, 0);


  class_addcreator((t_newmethod)logger::create,gensym("ø.logger"), A_GIMME, 0);

  eclass_addmethod(
      c, (method)logger::in_anything,
      "anything", A_GIMME, 0);
  eclass_addmethod(
      c, (method)logger::notify,
      "notify", A_NULL, 0);

  CLASS_ATTR_SYMBOL(c, "host", 0, logger, m_host);
  CLASS_ATTR_LABEL(c, "host", 0, "Logger host (eg ws://127.0.0.1:1337)");

  CLASS_ATTR_SYMBOL(c, "appname", 0, logger, m_appname);
  CLASS_ATTR_LABEL(c, "appname", 0, "App name");

  CLASS_ATTR_LONG(c, "heartbeat", 0, logger, m_ival);
  CLASS_ATTR_LABEL(c, "heartbeat", 0, "Heartbeat interval, in seconds");
  CLASS_ATTR_FILTER_CLIP(c, "heartbeat", 1, 1000);


  // eclass_register(CLASS_OBJ, c);
  ossia_pd::logger_class = c;
}

} // pd namespace
} // ossia namespace
