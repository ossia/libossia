// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/logger.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>
#include "ossia-max.hpp"

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_logger class methods

extern "C" void ossia_logger_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.logger class
  ossia_library.ossia_logger_class = class_new(
      "ossia.logger", (method)ossia_logger_new, (method)ossia_logger_free,
      (long)sizeof(logger), 0L, A_GIMME, 0);

  class_addmethod(
      ossia_library.ossia_logger_class, (method)ossia_logger_in_anything,
      "anything", A_GIMME, 0);

  class_register(CLASS_BOX, ossia_library.ossia_logger_class);
}

extern "C" void* ossia_logger_new(t_symbol* s, long argc, t_atom* argv)
{
  return make_ossia<logger>(argc, argv);
}

extern "C" void
ossia_logger_in_anything(logger* x, t_symbol* s, long argc, t_atom* argv)
{
  if (x && x->m_log)
    x->m_log->info("{}", s->s_name);
}

extern "C" void ossia_logger_free(logger* x)
{
  if (x)
  {
    x->~logger();
  }
}

#pragma mark -
#pragma mark t_logger structure functions

logger::logger(long argc, t_atom *argv)
{
  for (int i = 0; i < argc; i++)
  {
    if ((argv + i)->a_type == A_SYM)
    {
      ossia::string_view ip = atom_getsym(argv + i)->s_name;
      m_con = std::make_shared<ossia::websocket_threaded_connection>(std::string(ip));
      m_log = std::make_shared<spdlog::logger>(
           "max_logger", std::make_shared<websocket_log_sink>(m_con, "max_logger"));
      break;
    }
  }
}
