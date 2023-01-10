// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/thread.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>

#include <ossia-max/src/logger.hpp>
#include <ossia-max/src/ossia-max.hpp>

#include <git_info.h>

using namespace ossia::max_binding;

extern "C" void ossia_logger_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.logger class
  ossia_library.ossia_logger_class = class_new(
      "ossia.logger", (method)ossia_logger_new, (method)logger::free,
      (long)sizeof(logger), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_logger_class;
  class_addmethod(c, (method)logger::in_anything, "anything", A_GIMME, 0);
  class_addmethod(c, (method)logger::assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)logger::notify, "notify", A_CANT, 0);

  CLASS_ATTR_SYM(c, "host", 0, logger, m_host);
  CLASS_ATTR_LABEL(c, "host", 0, "Logger host (eg ws://127.0.0.1:1337)");

  CLASS_ATTR_SYM(c, "appname", 0, logger, m_appname);
  CLASS_ATTR_LABEL(c, "appname", 0, "App name");

  CLASS_ATTR_LONG(c, "heartbeat", 0, logger, m_ival);
  CLASS_ATTR_LABEL(c, "heartbeat", 0, "Heartbeat interval, in seconds");
  CLASS_ATTR_FILTER_CLIP(c, "heartbeat", 0, 1000);

  class_register(CLASS_BOX, ossia_library.ossia_logger_class);
}

extern "C" void* ossia_logger_new(t_symbol* s, long argc, t_atom* argv)
{
  critical_enter(0);
  auto x = make_ossia<logger>();

  if(x)
  {
    x->m_polling_clock = clock_new(x, (method)logger::check_connection_status);
    x->m_dumpout = outlet_new(x, nullptr);

    // default attributes
    x->m_host = gensym("ws://127.0.0.1:1337");
    x->m_appname = _sym_max;
    x->m_ival = 5;

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    attr_args_process(x, argc - attrstart, argv + attrstart);

    if(attrstart > 0 && argv)
    {
      if(atom_gettype(argv) == A_SYM)
      {
        x->m_appname = atom_getsym(argv);
      }
    }

    object_attach_byptr_register(x, x, CLASS_BOX);

    x->reset();
    ossia_max::instance().loggers.push_back(x);
  }
  critical_exit(0);

  return x;
}

void logger::in_anything(logger* x, t_symbol* s, long argc, t_atom* argv)
{
  if(x && x->m_log && s && s->s_name && argc > 0)
  {
    ossia::string_view type = s->s_name;

    std::string txt;
    for(int i = 0; i < argc; i++)
    {
      switch(argv[i].a_type)
      {
        case A_FLOAT:
          txt += std::to_string(atom_getfloat(&argv[i]));
          break;
        case A_LONG:
          txt += std::to_string(atom_getlong(&argv[i]));
          break;
        case A_SYM:
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

void logger::free(logger* x)
{
  critical_enter(0);
  if(x)
  {
    ossia_max::instance().loggers.remove_all(x);
    clock_unset(x->m_polling_clock);
    clock_free((t_object*)x->m_polling_clock);
    outlet_delete(x->m_dumpout);
    x->~logger();
  }
  critical_exit(0);
}

t_max_err logger::notify(logger* x, t_symbol* s, t_symbol* msg, void* sender, void* data)
{
  if(msg == gensym("attr_modified"))
  {
    x->reset();
  }
  return 0;
}

void logger::assist(logger* x, void* b, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
  {
    sprintf(s, "Log messages inlet");
  }
}

#pragma mark -
#pragma mark t_logger structure functions

void logger::reset()
{
  clock_unset(m_polling_clock);
  object_post(
      &m_object, "logger connect to %s with name %s", m_host->s_name, m_appname->s_name);

  m_log.reset();
  m_beat.reset();
  if(!m_host)
    return;
  if(!m_appname)
    return;

  ossia::string_view host = m_host->s_name;
  std::string appname = m_appname->s_name;

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_con.reset();

    m_con = std::make_shared<ossia::websocket_threaded_connection>(std::string(host));

    m_log = std::make_shared<spdlog::logger>(
        "max_logger", std::make_shared<websocket_log_sink>(m_con, appname));
  }

  check_connection_status(this);

  if(m_ival > 0)
  {
    m_beat = std::make_shared<websocket_heartbeat>(
        m_con, appname,
        std::chrono::seconds(ossia::clamp((int)m_ival, (int)0, (int)1000)));

    m_beat->send_init(
        {{"pid", ossia::get_pid()},
         {"cmd", ossia::get_exe_path()},
         {"version", ossia::get_commit_sha()}});
  }
}

void logger::check_connection_status(logger* x)
{
  std::lock_guard<std::mutex> lock(x->m_mutex);
  if(x->m_con && x->m_con->socket.connected())
  {
    if(x->m_status == DISCONNECTED)
    {
      x->m_status = CONNECTED;
      t_atom a;
      A_SETLONG(&a, 1);
      outlet_anything(x->m_dumpout, gensym("connected"), 1, &a);
    }
  }
  else if(x->m_status == CONNECTED)
  {
    x->m_status = DISCONNECTED;
    t_atom a;
    A_SETLONG(&a, 0);
    outlet_anything(x->m_dumpout, gensym("connected"), 1, &a);
  }
  clock_delay(x->m_polling_clock, 100);
}
