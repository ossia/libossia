#include <ossia/network/common/websocket_log_sink.hpp>
#include <ossia-max/src/logger.hpp>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_logger class methods

extern "C" void ossia_logger_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.logger class
  ossia_library.ossia_logger_class = class_new(
      "ossia.logger", (method)ossia_logger_new, (method)ossia_logger_free,
      (long)sizeof(t_logger), 0L, A_GIMME, 0);

  class_addmethod(
      ossia_library.ossia_logger_class, (method)ossia_logger_in_anything,
      "anything", A_GIMME, 0);

  class_register(CLASS_BOX, ossia_library.ossia_logger_class);
}

extern "C" void* ossia_logger_new(t_symbol* s, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  t_logger* x = (t_logger*)object_alloc(ossia_library.ossia_logger_class);

  if (x)
  {
    new (&x->m_log) std::shared_ptr<spdlog::logger>();
    for (int i = 0; i < argc; i++)
    {
      if ((argv + i)->a_type == A_SYM)
      {
        std::string ip = atom_getsym(argv + i)->s_name;
        // x->m_log = std::make_shared<spdlog::logger>(
        //      "max_logger",
        //      std::make_shared<ossia::websocket_log_sink>(ossia_library.get_connection(ip)));
        break;
      }
    }
  }

  return x;
}

extern "C" void
ossia_logger_in_anything(t_logger* x, t_symbol* s, long argc, t_atom* argv)
{
  if (x && x->m_log)
    x->m_log->info("{}", s->s_name);
}

extern "C" void ossia_logger_free(t_logger* x)
{
  if (x)
  {
    x->m_log.reset();
    x->m_log.~shared_ptr();
    // ossia_max::instance().collect_garbage();
  }
}

#pragma mark -
#pragma mark t_logger structure functions
