#include "ext.h"
#include "ext_obex.h"
#undef error
#undef post
#include <ossia/context.hpp>
#include <ossia/ossia.hpp>
#include <ossia/network/oscquery/detail/client.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>

// Types
struct websocket_log_sink final : public spdlog::sinks::sink
{
  ossia::oscquery::websocket_client s;
  std::thread thread;
  websocket_log_sink(std::string ip):
    s([] (auto&&...) {})
  {
    thread = std::thread( [=] { s.connect(ip); } );
  }

  ~websocket_log_sink()
  {
    s.stop();
    thread.join();
  }

  void log(const spdlog::details::log_msg& msg) override
  {
    rapidjson::StringBuffer b;
    rapidjson::Writer<rapidjson::StringBuffer> w{b};
    w.StartObject();
    w.Key(spdlog::level::level_names[msg.level]);
    w.String(msg.formatted.data(), msg.formatted.size());
    w.EndObject();

    s.send_message(b);
  }

  void flush() override
  {
  }
};

struct ossia_max_parameter
{
  t_object ob; // Always first
  void* outlet{};
  ossia::net::node_base* node{};
};
struct ossia_max_logger
{
  t_object ob;
  std::shared_ptr<spdlog::logger> logger;
};

// Return a singleton local device that can be used if the user has not made any device.
class ossia_max_singleton
{
public:
  static ossia_max_singleton& instance();

  static ossia::net::device_base& device_instance();

  t_class* ossia_parameter_class{};
  t_class* ossia_logger_class{};

private:
  ossia_max_singleton():
    m_localProtocol{new ossia::net::local_protocol},
    m_device{std::unique_ptr<ossia::net::protocol_base>(m_localProtocol), "ossia_device"}
  {
    m_localProtocol->exposeTo(
          std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678));
  }

  ossia::context ctx;
  ossia::net::local_protocol* m_localProtocol{};
  ossia::net::generic_device m_device;
};

//! Utility : converts a max string to a type used in the api
ossia::val_type name_to_type(ossia::string_view name)
{
  if(name == "integer") return ossia::val_type::INT;
  if(name == "float") return ossia::val_type::FLOAT;
  if(name == "numeric") return ossia::val_type::FLOAT;
  if(name == "array") return ossia::val_type::TUPLE;
  if(name == "impulse") return ossia::val_type::IMPULSE;
  if(name == "bool") return ossia::val_type::BOOL;
  if(name == "boolean") return ossia::val_type::BOOL;
  if(name == "string") return ossia::val_type::STRING;
  if(name == "symbol") return ossia::val_type::STRING;
  if(name == "vec2f") return ossia::val_type::VEC2F;
  if(name == "vec3f") return ossia::val_type::VEC3F;
  if(name == "vec4f") return ossia::val_type::VEC4F;
  if(name == "char") return ossia::val_type::CHAR;
  return ossia::val_type::FLOAT;
}

// One function per type in the OSSIA API (more or less like a switch)
struct send_message
{
  void* outlet{};
  void operator()(ossia::impulse) const { outlet_bang(outlet); }
  void operator()(int32_t v) const { outlet_int(outlet, v); }
  void operator()(float v) const { outlet_float(outlet, v); }
  void operator()(char v) const { }
  void operator()(ossia::vec2f v) const { }
  void operator()(ossia::vec3f v) const { }
  void operator()(ossia::vec4f v) const { }
  void operator()(const std::string& v) const { }
  void operator()(const std::vector<ossia::value>& v) const { }
  void operator()(const ossia::Destination&) const { }
  void operator()() { }
};


extern "C"
void* ossia_parameter_new(t_symbol *s, long argc, t_atom *argv)
{
  ossia_max_parameter* x = (ossia_max_parameter *) object_alloc(ossia_max_singleton::instance().ossia_parameter_class);

  if (x)
  {
    // Look for the arguments we want in the max constructor
    ossia::string_view name_to_create;
    ossia::string_view type_to_set;
    ossia::unit_t unit;

    ossia::optional<int32_t> int_min, int_max;
    ossia::optional<float> float_min, float_max;

    for (int i = 0; i < argc; i++)
    {
      switch((argv + i)->a_type)
      {
        case A_LONG:
          break;
        case A_FLOAT:
          break;
        case A_SYM:
        {
          ossia::string_view param = atom_getsym(argv+i)->s_name;
          if(param == "@name")
          {
            if((i + 1) < argc && (argv+i+1)->a_type == A_SYM)
            {
              name_to_create = atom_getsym(argv+i+1)->s_name;
              i++;
            }
          }
          else if(param == "@type")
          {
            if((i + 1) < argc && (argv+i+1)->a_type == A_SYM)
            {
              type_to_set = atom_getsym(argv+i+1)->s_name;
              i++;
            }
          }
          else if(param == "@min")
          {
            if((i + 1) < argc)
            {
              if((argv+i+1)->a_type == A_LONG)
              {
                int_min = atom_getlong(argv+i+1);
              }
              else if((argv+i+1)->a_type == A_FLOAT)
              {
                float_min = atom_getfloat(argv+i+1);
              }
              i++;
            }
          }
          else if(param == "@max")
          {
            if((i + 1) < argc)
            {
              if((argv+i+1)->a_type == A_LONG)
              {
                int_max = atom_getlong(argv+i+1);
              }
              else if((argv+i+1)->a_type == A_FLOAT)
              {
                float_max = atom_getfloat(argv+i+1);
              }
              i++;
            }
          }
          else if(param == "@unit")
          {
            if((i + 1) < argc && (argv+i+1)->a_type == A_SYM)
            {
              unit = ossia::parse_pretty_unit(atom_getsym(argv+i+1)->s_name);
              i++;
            }
          }
        }
          break;
        default:
          break;
      }
    }

    // Create the node with what we found
    x->node = &ossia::net::find_or_create_node(
          ossia_max_singleton::device_instance().getRootNode(),
          name_to_create
          );

    auto addr = x->node->createAddress(name_to_type(type_to_set));

    // Handle domain
    if(int_min && int_max)
      addr->setDomain(ossia::make_domain(*int_min, *int_max));
    else if(int_min)
      addr->setDomain(ossia::make_domain(*int_min, ossia::value{}));
    else if(int_max)
      addr->setDomain(ossia::make_domain(ossia::value{}, *int_max));
    else if(float_min && float_max)
      addr->setDomain(ossia::make_domain(*float_min, *float_max));
    else if(float_min)
      addr->setDomain(ossia::make_domain(*float_min, ossia::value{}));
    else if(float_max)
      addr->setDomain(ossia::make_domain(ossia::value{}, *float_max));

    if(unit)
      addr->setUnit(unit);
    // etc...
    // { level: 4 ; message : "foo" }
    // Create the outlet
    x->outlet = outlet_new(x, nullptr);

    addr->add_callback([=] (const ossia::value& val) {
      val.apply(send_message{x->outlet});
    });
  }
  return x;
}

extern "C"
void ossia_parameter_assist(ossia_max_parameter *x, void *b, long m, long a, char *s)
{
  if (m == ASSIST_INLET) { // inlet
    sprintf(s, "I am inlet %ld", a);
  }
  else {	// outlet
    sprintf(s, "I am outlet %ld", a);
  }
}

template<typename T>
void ossia_parameter_in(ossia_max_parameter* x, T f)
{
  if(x && x->node)
  {
    if(auto addr = x->node->getAddress())
    {
      addr->pushValue(f);
    }
  }
}

extern "C"
void ossia_parameter_in_float(ossia_max_parameter* x, double f)
{ ossia_parameter_in(x, f); }
extern "C"
void ossia_parameter_in_int(ossia_max_parameter* x, long int f)
{ ossia_parameter_in(x, (int32_t)f); }
extern "C"
void ossia_parameter_in_bang(ossia_max_parameter* x)
{ ossia_parameter_in(x, ossia::impulse{}); }
extern "C"
void ossia_parameter_in_symbol(ossia_max_parameter* x, t_symbol* f)
{ ossia_parameter_in(x, std::string(f->s_name)); }
extern "C"
void ossia_parameter_in_char(ossia_max_parameter* x, char f)
{ ossia_parameter_in(x, f); }
extern "C"
void ossia_parameter_in_anything(ossia_max_parameter* x, t_symbol *s, long argc, t_atom *argv)
{ /* todo */ }

extern "C"
void ossia_parameter_free(ossia_max_parameter* x)
{
  if(x)
  {
    if(x->node)
    {
      if(auto par = x->node->getParent())
      {
        par->removeChild(*x->node);
        x->node = nullptr;
      }
    }
  }
}



extern "C"
void* ossia_logger_new(t_symbol *s, long argc, t_atom *argv)
{
  ossia_max_logger* x = (ossia_max_logger *) object_alloc(ossia_max_singleton::instance().ossia_logger_class);

  if (x)
  {
    for (int i = 0; i < argc; i++)
    {
      if((argv + i)->a_type == A_SYM)
      {
        x->logger =
            std::make_shared<spdlog::logger>(
              "max_logger",
              std::make_shared<websocket_log_sink>(atom_getsym(argv+i)->s_name));
        break;
      }
    }
  }

  return x;
}

extern "C"
void ossia_logger_in_anything(ossia_max_logger* x, t_symbol *s, long argc, t_atom *argv)
{
  x->logger->info("{}", s->s_name);
}

extern "C"
void ossia_logger_free(ossia_max_logger* x)
{
  x->logger.reset();
}

extern "C"
void ext_main(void *r)
{
  auto& instance = ossia_max_singleton::instance();

  // Instantiate the parameter class
  instance.ossia_parameter_class = class_new(
        "ossia.parameter",
        (method)ossia_parameter_new, (method)ossia_parameter_free, (long)sizeof(ossia_max_parameter),
        0L, A_GIMME, 0);

  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_assist, "assist", A_CANT, 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_bang, "bang", 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_int, "int", A_LONG, 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_float, "float", A_FLOAT, 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_symbol, "symbol", A_SYM, 0);
  class_addmethod(instance.ossia_parameter_class, (method) ossia_parameter_in_anything, "anything", A_GIMME, 0);
  class_register(CLASS_BOX, instance.ossia_parameter_class);

  instance.ossia_logger_class = class_new("ossia.logger", (method) ossia_logger_new, (method) ossia_logger_free,
                                          (long) sizeof(ossia_max_logger), 0L, A_GIMME, 0);

  class_addmethod(instance.ossia_logger_class, (method) ossia_logger_in_anything, "anything", A_GIMME, 0);
  class_register(CLASS_BOX, instance.ossia_logger_class);

}

ossia_max_singleton& ossia_max_singleton::instance()
{
  static ossia_max_singleton dev;
  return dev;
}

ossia::net::device_base& ossia_max_singleton::device_instance()
{
  return instance().m_device;
}
