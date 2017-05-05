#include <ossia-max/src/ossia-max.hpp>
#include <ossia/context.hpp>
#include <ossia/network/oscquery/detail/client.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/logger.hpp>
// Types
using namespace ossia::max;
extern "C"
void ext_main(void *r)
{
  auto& instance = singleton::instance();

  // Instantiate the parameter class
  instance.ossia_parameter_class = class_new(
        "ossia.parameter",
        (method)ossia_parameter_new, (method)ossia_parameter_free, (long)sizeof(ossia::max::parameter),
        0L, A_GIMME, 0);

  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_assist, "assist", A_CANT, 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_bang, "bang", 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_int, "int", A_LONG, 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_float, "float", A_FLOAT, 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_symbol, "symbol", A_SYM, 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_anything, "anything", A_GIMME, 0);
  class_register(CLASS_BOX, instance.ossia_parameter_class);

  instance.ossia_logger_class = class_new("ossia.logger", (method) ossia_logger_new, (method) ossia_logger_free,
                                          (long) sizeof(logger), 0L, A_GIMME, 0);

  class_addmethod(instance.ossia_logger_class, (method) ossia_logger_in_anything, "anything", A_GIMME, 0);
  class_register(CLASS_BOX, instance.ossia_logger_class);

  post("Welcome to OSSIA library for Max");
}

singleton& singleton::instance()
{
  static singleton dev;
  return dev;
}

ossia::net::device_base& singleton::device_instance()
{
  return instance().m_device;
}

std::shared_ptr<ossia::websocket_threaded_connection> singleton::get_connection(std::string ip)
{
  // Look if there is already a connection open
  std::shared_ptr<ossia::websocket_threaded_connection> ws;
  auto conn_it = m_connections.find(ip);
  if(conn_it != m_connections.end())
  {
    ws = conn_it.value();
  }
  else
  {
    ws = std::make_shared<ossia::websocket_threaded_connection>(ip);
    m_connections.insert({ip, ws});
  }
  return ws;
}

void singleton::collect_garbage()
{
  auto it = ossia::find_if(m_connections, [] (const auto& val) {
    return val.second.unique();
  });
  if(it != m_connections.end())
  {
    m_connections.erase(it);
  }
}

singleton::singleton():
  m_localProtocol{new ossia::net::local_protocol},
  m_device{std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_device"}
{
  m_localProtocol->expose_to(
        std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678));
}
