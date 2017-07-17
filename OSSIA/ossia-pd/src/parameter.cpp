// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "parameter.hpp"
#include "device.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "utils.hpp"
#include "ossia-pd.hpp"

#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <limits>

namespace ossia
{
namespace pd
{

static void parameter_free(t_param* x);

bool t_param::register_node(ossia::net::node_base* node)
{
  bool res = do_registration(node);
  if (res)
  {
    obj_dequarantining<t_param>(this);
    for (auto remote : t_remote::quarantine().copy())
    {
      obj_register<t_remote>(static_cast<t_remote*>(remote));
    }
  }
  else
    obj_quarantining<t_param>(this);

  return res;
}

static void push_default_value(t_param* x)
{
  int i = 0;
  for (; i < x->x_type_size; i++)
  {
    if (x->x_default[i].a_type == A_NULL)
      break;
  }
  if (i > 0)
    t_obj_base::obj_push(x, nullptr, i, x->x_default);
}

bool t_param::do_registration(ossia::net::node_base* node)
{

  if (x_node && x_node->get_parent() == node)
    return true; // already register to this node;

  unregister(); // we should unregister here because we may have add a node
                // between the registered node and the parameter

  if (!node)
    return false;

  /*
  std::string absolute_path = get_absolute_path<t_param>(this);
  std::string address_string = ossia::net::address_string_from_node(*node);
  if (absolute_path != address_string) return false;
  */

  x_node = &ossia::net::create_node(*node, x_name->s_name);
  if (x_node->get_name() != std::string(x_name->s_name))
    renaming(this);

  x_node->about_to_be_deleted.connect<t_param, &t_param::is_deleted>(this);
  ossia::net::address_base* local_address{};
  std::string type = x_type->s_name;
  ossia::transform(type, type.begin(), ::tolower);
  if (type == "float")
  {
    local_address = x_node->create_address(ossia::val_type::FLOAT);
    if (x_default[0].a_type == A_FLOAT)
      ossia::net::set_default_value(
          local_address->get_node(), x_default[0].a_w.w_float);
  }
  else if (type == "symbol" || type == "string")
  {
    local_address = x_node->create_address(ossia::val_type::STRING);
    if (x_default[0].a_type == A_SYMBOL)
      ossia::net::set_default_value(
          local_address->get_node(),
          std::string(x_default[0].a_w.w_symbol->s_name));
  }
  else if (type == "int")
  {
    local_address = x_node->create_address(ossia::val_type::INT);
    if (x_default[0].a_type == A_FLOAT)
      ossia::net::set_default_value(
          local_address->get_node(), x_default[0].a_w.w_float);
  }
  else if (type == "vec2f")
  {
    local_address = x_node->create_address(ossia::val_type::VEC2F);
    x_type_size = 2;
    if (x_default[0].a_type == A_FLOAT && x_default[1].a_type == A_FLOAT)
    {
      vec2f vec = make_vec(x_default[0].a_w.w_float, x_default[1].a_w.w_float);
      ossia::net::set_default_value(local_address->get_node(), vec);
    }
  }
  else if (type == "vec3f")
  {
    local_address = x_node->create_address(ossia::val_type::VEC3F);
    x_type_size = 3;
    if (x_default[0].a_type == A_FLOAT && x_default[1].a_type == A_FLOAT
        && x_default[2].a_type == A_FLOAT)
    {
      vec3f vec = make_vec(
          x_default[0].a_w.w_float, x_default[1].a_w.w_float,
          x_default[2].a_w.w_float);
      ossia::net::set_default_value(local_address->get_node(), vec);
    }
  }
  else if (type == "vec4f")
  {
    local_address = x_node->create_address(ossia::val_type::VEC4F);
    x_type_size = 4;
    if (x_default[0].a_type == A_FLOAT && x_default[1].a_type == A_FLOAT
        && x_default[2].a_type == A_FLOAT && x_default[3].a_type == A_FLOAT)
    {
      vec4f vec = make_vec(
          x_default[0].a_w.w_float, x_default[1].a_w.w_float,
          x_default[2].a_w.w_float, x_default[3].a_w.w_float);
      ossia::net::set_default_value(local_address->get_node(), vec);
    }
  }
  else if (type == "impulse")
  {
    local_address = x_node->create_address(ossia::val_type::IMPULSE);
    x_type_size = 0;
  }
  else if (type == "bool")
  {
    local_address = x_node->create_address(ossia::val_type::BOOL);
    if (x_default[0].a_type == A_FLOAT)
      ossia::net::set_default_value(
          local_address->get_node(), x_default[0].a_w.w_float);
  }
  else if (type == "tuple")
  {
    local_address = x_node->create_address(ossia::val_type::TUPLE);
    x_type_size = 64;
    std::vector<ossia::value> list;
    for (int i = 0; i < 64 && x_default[i].a_type != A_NULL; i++)
    {
      if (x_default[i].a_type == A_FLOAT)
        list.push_back(atom_getfloat(&x_default[i]));
      else if (x_default[i].a_type == A_SYMBOL)
        list.push_back(std::string(atom_getsymbol(&x_default[i])->s_name));
    }
    if (list.size() > 0)
      ossia::net::set_default_value(local_address->get_node(), list);
  }
  else if (type == "char")
  {
    local_address = x_node->create_address(ossia::val_type::CHAR);
    if (x_default[0].a_type == A_FLOAT)
      ossia::net::set_default_value(
          local_address->get_node(), x_default[0].a_w.w_float);
  }
  else
  {
    pd_error(
        this,
        "type should one of (case sensitive): float, symbol, int, vec2f, "
        "vec3f, vec4f, bool, tuple, char");
  }
  if (!local_address)
    return false;

  local_address->set_domain(ossia::make_domain(x_range[0], x_range[1]));

  std::string bounding_mode = x_bounding_mode->s_name;
  ossia::transform(bounding_mode, bounding_mode.begin(), ::tolower);
  if (bounding_mode == "free")
    local_address->set_bounding(ossia::bounding_mode::FREE);
  else if (bounding_mode == "clip")
    local_address->set_bounding(ossia::bounding_mode::CLIP);
  else if (bounding_mode == "wrap")
    local_address->set_bounding(ossia::bounding_mode::WRAP);
  else if (bounding_mode == "fold")
    local_address->set_bounding(ossia::bounding_mode::FOLD);
  else if (bounding_mode == "low")
    local_address->set_bounding(ossia::bounding_mode::LOW);
  else if (bounding_mode == "high")
    local_address->set_bounding(ossia::bounding_mode::HIGH);
  else
  {
    pd_error(this, "unknown bounding mode: %s", bounding_mode.c_str());
  }

  std::string access_mode = x_access_mode->s_name;
  ossia::transform(access_mode, access_mode.begin(), ::tolower);
  if (access_mode == "bi" || access_mode == "rw")
    local_address->set_access(ossia::access_mode::BI);
  else if (access_mode == "get" || access_mode == "r")
    local_address->set_access(ossia::access_mode::GET);
  else if (access_mode == "set" || access_mode == "w")
    local_address->set_access(ossia::access_mode::SET);
  else
  {
    pd_error(this, "unknown access mode: %s", access_mode.c_str());
  }

  local_address->set_repetition_filter(
      x_repetition_filter ? ossia::repetition_filter::ON
                          : ossia::repetition_filter::OFF);

  ossia::unit_t unit = ossia::parse_pretty_unit(x_unit->s_name);
  local_address->set_unit(unit);

  ossia::net::set_description(local_address->get_node(), x_description->s_name);
  ossia::net::set_tags(local_address->get_node(), parse_tags_symbol(x_tags));

  ossia::net::set_priority(local_address->get_node(), x_priority);

  ossia::net::set_hidden(local_address->get_node(), x_hidden);

  local_address->add_callback([=](const ossia::value& v) { setValue(v); });

  clock_delay(x_clock, 0);

  return true;
}

bool t_param::unregister()
{
  clock_unset(x_clock);
  if (x_node)
  {
    x_node->about_to_be_deleted.disconnect<t_param, &t_param::is_deleted>(this);

    if (x_node->get_parent())
      x_node->get_parent()->remove_child(*x_node);

    x_node = nullptr;

    for (auto remote : t_remote::quarantine().copy())
    {
      obj_register<t_remote>(static_cast<t_remote*>(remote));
    }
  }

  obj_quarantining<t_param>(this);

  derenaming(this);

  for (auto param : t_param::rename().copy())
  {
    if (strcmp(param->x_name->s_name, x_name->s_name) == 0)
    {
      param->unregister();
      obj_register<t_param>(param);
    }
  }

  return true;
}

ossia::safe_vector<t_param*>& t_param::quarantine()
{
  static ossia::safe_vector<t_param*> quarantine;
  return quarantine;
}

void t_param::is_deleted(const net::node_base& n)
{
  x_node->about_to_be_deleted.disconnect<t_param, &t_param::is_deleted>(this);
  x_node = nullptr;
  obj_quarantining<t_param>(this);
}

ossia::safe_vector<t_param*>& t_param::rename()
{
  static ossia::safe_vector<t_param*> rename;
  return rename;
}

static void* parameter_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_param* x = (t_param*)eobj_new(ossia_pd.param);

  // TODO SANITIZE : memory leak
  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    ossia_pd::params().push_back(x);
    x->x_otype = Type::param;
    x->x_range[0] = 0.;
    x->x_range[1] = 1.;

    x->x_setout = nullptr;
    x->x_dataout = outlet_new((t_object*)x, nullptr);
    x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
    x->x_node = nullptr;

    x->x_access_mode = gensym("rw");
    x->x_bounding_mode = gensym("free");
    x->x_unit = gensym("");
    x->x_type = gensym("tuple");
    x->x_type_size = 1;
    x->x_tags = gensym("");
    x->x_description = gensym("");
    x->x_priority = 0;
    x->x_hidden = false;

    x->x_clock = clock_new(x, (t_method)push_default_value);

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->x_name = atom_getsymbol(argv);
      x->x_absolute = std::string(x->x_name->s_name) != ""
                      && x->x_name->s_name[0] == '/';
    }
    else
    {
      pd_error(x, "You have to pass a name as the first argument");
      x->x_name = gensym("untitledParam");
    }

    ebox_attrprocess_viabinbuf(x, d);

    obj_register<t_param>(x);
  }

  return (x);
}

static void parameter_free(t_param* x)
{
  x->x_dead = true;
  x->unregister();
  obj_dequarantining<t_param>(x);
  ossia_pd::params().remove_all(x);
  outlet_free(x->x_dataout);
  outlet_free(x->x_dumpout);
}

extern "C" void setup_ossia0x2eparam(void)
{
  t_eclass* c = eclass_new(
      "ossia.param", (method)parameter_new, (method)parameter_free,
      (short)sizeof(t_param), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)parameter_new,gensym("ø.param"), A_GIMME, 0);

    eclass_addmethod(c, (method)t_obj_base::obj_push, "anything", A_GIMME, 0);
    eclass_addmethod(c, (method)t_obj_base::obj_bang, "bang", A_NULL, 0);
    eclass_addmethod(c, (method)obj_dump<t_param>, "dump", A_NULL, 0);

    CLASS_ATTR_SYMBOL(c, "type", 0, t_param, x_type);
    CLASS_ATTR_SYMBOL(c, "unit", 0, t_param, x_unit);
    CLASS_ATTR_SYMBOL(c, "bounding_mode", 0, t_param, x_bounding_mode);
    CLASS_ATTR_SYMBOL(c, "access_mode", 0, t_param, x_access_mode);
    CLASS_ATTR_SYMBOL(c, "description", 0, t_param, x_description);
    CLASS_ATTR_SYMBOL(c, "tags", 0, t_param, x_tags);

    CLASS_ATTR_ATOM_ARRAY(c, "default", 0, t_param, x_default, 64);
    CLASS_ATTR_FLOAT_ARRAY(c, "range", 0, t_param, x_range, 2);
    CLASS_ATTR_FLOAT(c, "min", 0, t_param, x_range);
    CLASS_ATTR_FLOAT(c, "repetition_filter", 0, t_param, x_repetition_filter);
    CLASS_ATTR_INT(c, "priority", 0, t_param, x_priority);
    CLASS_ATTR_INT(c, "hidden", 0, t_param, x_hidden);
    // CLASS_ATTR_FLOAT(c, "max", 0, t_parameter, range+1);
    eclass_new_attr_typed(
        c, "max", "float", 1, 0, 0,
        calcoffset(t_param, x_range) + sizeof(float));

    CLASS_ATTR_DEFAULT(c, "type", 0, "float");

    // eclass_register(CLASS_OBJ, c); // disable property dialog since it's
    // buggy
  }

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.param = c;
}
} // pd namespace
} // ossia namespace
