#include <ossia-max/src/parameter.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>


//! Utility : converts a max string to a type used in the api
static ossia::val_type name_to_type(ossia::string_view name)
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
struct ossia_value_to_outlet
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
  using namespace ossia::max;
  parameter* x = (parameter *) object_alloc(singleton::instance().ossia_parameter_class);

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
          ossia::max::singleton::device_instance().getRootNode(),
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
      val.apply(ossia_value_to_outlet{x->outlet});
    });
  }
  return x;
}

extern "C"
void ossia_parameter_assist(ossia::max::parameter *x, void *b, long m, long a, char *s)
{
  if (m == ASSIST_INLET) { // inlet
    sprintf(s, "I am inlet %ld", a);
  }
  else {	// outlet
    sprintf(s, "I am outlet %ld", a);
  }
}

template<typename T>
void ossia_parameter_in(ossia::max::parameter* x, T f)
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
void ossia_parameter_in_float(ossia::max::parameter* x, double f)
{ ossia_parameter_in(x, f); }
extern "C"
void ossia_parameter_in_int(ossia::max::parameter* x, long int f)
{ ossia_parameter_in(x, (int32_t)f); }
extern "C"
void ossia_parameter_in_bang(ossia::max::parameter* x)
{ ossia_parameter_in(x, ossia::impulse{}); }
extern "C"
void ossia_parameter_in_symbol(ossia::max::parameter* x, t_symbol* f)
{ ossia_parameter_in(x, std::string(f->s_name)); }
extern "C"
void ossia_parameter_in_char(ossia::max::parameter* x, char f)
{ ossia_parameter_in(x, f); }
extern "C"
void ossia_parameter_in_anything(ossia::max::parameter* x, t_symbol *s, long argc, t_atom *argv)
{ /* todo */ }

extern "C"
void ossia_parameter_free(ossia::max::parameter* x)
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

