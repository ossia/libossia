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

bool t_param::do_registration(ossia::net::node_base* node)
{
  unregister(); // we should unregister here because we may have add a node
                // between the registered node and the parameter

  if (!node)
    return false;

  /*
  std::string absolute_path = get_absolute_path<t_param>(this);
  std::string address_string = ossia::net::address_string_from_node(*node);
  if (absolute_path != address_string) return false;
  */

  x_parent_node = node;

  auto nodes = ossia::net::create_nodes(*node, x_name->s_name);

  for (auto n : nodes)
  {
    ossia::net::parameter_base* local_param{};
    std::string type = x_type->s_name;

    if (type == "float")
    {
      local_param = n->create_parameter(ossia::val_type::FLOAT);
      if (x_default[0].a_type == A_FLOAT)
        ossia::net::set_default_value(
              local_param->get_node(), x_default[0].a_w.w_float);

      float min = 0.;
      float max = 1.;

      if (x_range[0].a_type == A_FLOAT && x_range[1].a_type == A_FLOAT)
      {
        min = x_range[0].a_w.w_float;
        max = x_range[1].a_w.w_float;
      }

      if (x_min[0].a_type == A_FLOAT)
        min = x_min[0].a_w.w_float;
      else
        min = std::numeric_limits<float>::lowest();

      if (x_max[0].a_type == A_FLOAT)
        max = x_max[0].a_w.w_float;
      else
        max = std::numeric_limits<float>::max();

      local_param->set_domain(ossia::make_domain(min, max));
    }
    else if (type == "symbol" || type == "string")
    {
      local_param = n->create_parameter(ossia::val_type::STRING);
      if (x_default[0].a_type == A_SYMBOL)
        ossia::net::set_default_value(
              local_param->get_node(),
              std::string(x_default[0].a_w.w_symbol->s_name));

      std::vector<std::string> senum;
      for ( int i = 0; i < OSSIA_PD_MAX_ATTR_SIZE; i++)
      {
        if (x_range[i].a_type == A_SYMBOL)
          senum.push_back(x_range[i].a_w.w_symbol->s_name);
        else if (x_range[i].a_type == A_FLOAT)
        {
          std::stringstream ss;
          ss << x_range[i].a_w.w_float;
          senum.push_back(ss.str());
        }
        else
          break;
      }
      if (!senum.empty())
        local_param->set_domain(make_domain(senum));
    }
    else if (type == "int")
    {
      local_param = n->create_parameter(ossia::val_type::INT);
      if (x_default[0].a_type == A_FLOAT)
        ossia::net::set_default_value(
              local_param->get_node(), x_default[0].a_w.w_float);

      int min = 0.;
      int max = 1.;

      if (x_range[0].a_type == A_FLOAT && x_range[1].a_type == A_FLOAT)
      {
        min = x_range[0].a_w.w_float;
        max = x_range[1].a_w.w_float;
      }

      if (x_min[0].a_type == A_FLOAT)
        min = x_min[0].a_w.w_float;
      else
        min = std::numeric_limits<int>::lowest();

      if (x_max[0].a_type == A_FLOAT)
        max = x_max[0].a_w.w_float;
      else
        max = std::numeric_limits<int>::max();

      local_param->set_domain(ossia::make_domain(min, max));
    }
    else if (type == "vec2f")
    {
      local_param = n->create_parameter(ossia::val_type::VEC2F);
      x_type_size = 2;
      if (x_default[0].a_type == A_FLOAT && x_default[1].a_type == A_FLOAT)
      {
        vec2f vec = make_vec(x_default[0].a_w.w_float, x_default[1].a_w.w_float);
        ossia::net::set_default_value(local_param->get_node(), vec);
      }

      vec2f min;
      vec2f max;

      if (x_range[0].a_type == A_FLOAT && x_range[1].a_type == A_FLOAT)
      {
        min.fill(x_range[0].a_w.w_float);
        max.fill(x_range[1].a_w.w_float);
      } else {
        min.fill(0.);
        max.fill(1.);
      }

      for (int i=0; i<2; i++){
        if (x_min[i].a_type == A_FLOAT)
          min[i] = x_min[i].a_w.w_float;
        else
          min[i] = std::numeric_limits<float>::lowest();

        if (x_max[i].a_type == A_FLOAT)
          max[i] = x_max[i].a_w.w_float;
        else
          max[i] = std::numeric_limits<float>::max();
      }
      local_param->set_domain(ossia::make_domain(min, max));
    }
    else if (type == "vec3f")
    {
      local_param = n->create_parameter(ossia::val_type::VEC3F);
      x_type_size = 3;
      if (x_default[0].a_type == A_FLOAT && x_default[1].a_type == A_FLOAT
          && x_default[2].a_type == A_FLOAT)
      {
        vec3f vec = make_vec(
              x_default[0].a_w.w_float, x_default[1].a_w.w_float,
            x_default[2].a_w.w_float);
        ossia::net::set_default_value(local_param->get_node(), vec);
      }

      vec3f min;
      vec3f max;

      if (x_range[0].a_type == A_FLOAT && x_range[1].a_type == A_FLOAT)
      {
        min.fill(x_range[0].a_w.w_float);
        max.fill(x_range[1].a_w.w_float);
      } else {
        min.fill(0.);
        max.fill(1.);
      }

      for (int i=0; i<3; i++){
        if (x_min[i].a_type == A_FLOAT)
          min[i] = x_min[i].a_w.w_float;
        else
          min[i] = std::numeric_limits<float>::lowest();

        if (x_max[i].a_type == A_FLOAT)
          max[i] = x_max[i].a_w.w_float;
        else
          max[i] = std::numeric_limits<float>::max();
      }
      local_param->set_domain(ossia::make_domain(min, max));
    }
    else if (type == "vec4f")
    {
      local_param = n->create_parameter(ossia::val_type::VEC4F);
      x_type_size = 4;
      if (x_default[0].a_type == A_FLOAT && x_default[1].a_type == A_FLOAT
          && x_default[2].a_type == A_FLOAT && x_default[3].a_type == A_FLOAT)
      {
        vec4f vec = make_vec(
              x_default[0].a_w.w_float, x_default[1].a_w.w_float,
            x_default[2].a_w.w_float, x_default[3].a_w.w_float);
        ossia::net::set_default_value(local_param->get_node(), vec);
      }

      vec4f min;
      vec4f max;

      if (x_range[0].a_type == A_FLOAT && x_range[1].a_type == A_FLOAT)
      {
        min.fill(x_range[0].a_w.w_float);
        max.fill(x_range[1].a_w.w_float);
      } else {
        min.fill(0.);
        max.fill(1.);
      }

      for (int i=0; i<4; i++){
        if (x_min[i].a_type == A_FLOAT)
          min[i] = x_min[i].a_w.w_float;
        else
          min[i] = std::numeric_limits<float>::lowest();

        if (x_max[i].a_type == A_FLOAT)
          max[i] = x_max[i].a_w.w_float;
        else
          max[i] = std::numeric_limits<float>::max();
      }
      local_param->set_domain(ossia::make_domain(min, max));

    }
    else if (type == "impulse")
    {
      local_param = n->create_parameter(ossia::val_type::IMPULSE);
      x_type_size = 0;
    }
    else if (type == "bool")
    {
      local_param = n->create_parameter(ossia::val_type::BOOL);
      if (x_default[0].a_type == A_FLOAT)
        ossia::net::set_default_value(
              local_param->get_node(), x_default[0].a_w.w_float);
    }
    else if (type == "list")
    {
      local_param = n->create_parameter(ossia::val_type::TUPLE);
      x_type_size = OSSIA_PD_MAX_ATTR_SIZE;

      auto list = attribute2value(x_default);
      if (list.size() > 0)
        ossia::net::set_default_value(local_param->get_node(), list);

      std::vector<ossia::value> min = attribute2value(x_min);
      std::vector<ossia::value> max = attribute2value(x_max);

      if (!min.empty() && !max.empty())
        local_param->set_domain(ossia::make_domain(min, max));
      else
      {
        if (x_range[0].a_type == A_FLOAT && x_range[1].a_type == A_FLOAT)
        {
          std::vector<ossia::value> omin, omax;
          std::array<float, OSSIA_PD_MAX_ATTR_SIZE> min, max;
          min.fill(x_range[0].a_w.w_float);
          max.fill(x_range[1].a_w.w_float);
          omin.assign(min.begin(), min.end());
          omax.assign(max.begin(), max.end());
          local_param->set_domain(ossia::make_domain(omin,omax));
        }
      }
    }
    else if (type == "char")
    {
      local_param = n->create_parameter(ossia::val_type::CHAR);
      if (x_default[0].a_type == A_FLOAT)
        ossia::net::set_default_value(
              local_param->get_node(), x_default[0].a_w.w_float);
    }
    else
    {
      pd_error(
            this,
            "type should one of: float, symbol, int, vec2f, "
            "vec3f, vec4f, bool, list, char");
    }
    if (!local_param)
      return false;

    std::string bounding_mode = x_bounding_mode->s_name;

    if (bounding_mode == "free")
      local_param->set_bounding(ossia::bounding_mode::FREE);
    else if (bounding_mode == "clip")
      local_param->set_bounding(ossia::bounding_mode::CLIP);
    else if (bounding_mode == "wrap")
      local_param->set_bounding(ossia::bounding_mode::WRAP);
    else if (bounding_mode == "fold")
      local_param->set_bounding(ossia::bounding_mode::FOLD);
    else if (bounding_mode == "low")
      local_param->set_bounding(ossia::bounding_mode::LOW);
    else if (bounding_mode == "high")
      local_param->set_bounding(ossia::bounding_mode::HIGH);
    else
    {
      pd_error(this, "unknown bounding mode: %s", bounding_mode.c_str());
    }

    std::string access_mode = x_access_mode->s_name;

    if (access_mode == "bi" || access_mode == "rw")
      local_param->set_access(ossia::access_mode::BI);
    else if (access_mode == "get" || access_mode == "r")
      local_param->set_access(ossia::access_mode::GET);
    else if (access_mode == "set" || access_mode == "w")
      local_param->set_access(ossia::access_mode::SET);
    else
    {
      pd_error(this, "unknown access mode: %s", access_mode.c_str());
    }

    local_param->set_repetition_filter(
          x_repetition_filter ? ossia::repetition_filter::ON
                              : ossia::repetition_filter::OFF);

    ossia::unit_t unit = ossia::parse_pretty_unit(x_unit->s_name);
    local_param->set_unit(unit);

    ossia::net::set_description(local_param->get_node(), x_description->s_name);
    ossia::net::set_tags(local_param->get_node(), parse_tags_symbol(x_tags));

    ossia::net::set_priority(local_param->get_node(), x_priority);

    ossia::net::set_hidden(local_param->get_node(), x_hidden);

    ossia::net::set_disabled(local_param->get_node(), !x_enable);

    t_matcher matcher{n,this};
    x_matchers.push_back(std::move(matcher));
  }

  clock_delay(x_clock, 0);

  return true;
}

bool t_param::unregister()
{
  clock_unset(x_clock);

  x_matchers.clear();

  x_node = nullptr;

  for (auto remote : t_remote::quarantine().copy())
  {
    obj_register<t_remote>(static_cast<t_remote*>(remote));
  }

  obj_quarantining<t_param>(this);

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

static void* parameter_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_param* x = (t_param*)eobj_new(ossia_pd.param);

  // TODO SANITIZE : memory leak
  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    ossia_pd.params.push_back(x);
    x->x_otype = Type::param;

    x->x_setout = nullptr;
    x->x_dataout = outlet_new((t_object*)x, nullptr);
    x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
    x->x_node = nullptr;
    x->x_parent_node = nullptr;

    x->x_access_mode = gensym("rw");
    x->x_bounding_mode = gensym("free");
    x->x_unit = gensym("");
    x->x_type = gensym("float");
    x->x_type_size = 1;
    x->x_tags = gensym("");
    x->x_description = gensym("");
    x->x_priority = 0;
    x->x_hidden = false;

    x->x_clock = clock_new(x, (t_method)push_default_value);

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->x_name = atom_getsymbol(argv);
      x->x_addr_scope = get_address_scope(x->x_name->s_name);
    }
    else
    {
      pd_error(x, "You have to pass a name as the first argument");
      x->x_name = gensym("untitledParam");
    }

    ebox_attrprocess_viabinbuf(x, d);

    // change some attributes names to lower case
    std::string type = x->x_type->s_name;
    ossia::transform(type, type.begin(), ::tolower);
    x->x_type = gensym(type.c_str());

    std::string bounding_mode = x->x_bounding_mode->s_name;
    ossia::transform(bounding_mode, bounding_mode.begin(), ::tolower);
    x->x_bounding_mode = gensym(bounding_mode.c_str());

    std::string access_mode = x->x_access_mode->s_name;
    ossia::transform(access_mode, access_mode.begin(), ::tolower);
    x->x_access_mode = gensym(access_mode.c_str());

    obj_register<t_param>(x);
  }

  return (x);
}

t_pd_err parameter_notify(t_param*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
{
  logpost(x, 2, "received notification !");
  if (msg == gensym("attr_modified"))
  {
    if( s == gensym("range") )
    {
      logpost(x, 2, "please update range");
      outlet_anything(x->x_dumpout, gensym("range"), x->x_range_size, x->x_range);
    }
    else if ( s == gensym("bounding_mode") )
    {
      logpost(x, 2, "please update bounding_mode");
    }
  }
  return 0;
}

static void parameter_free(t_param* x)
{
  x->x_dead = true;
  x->unregister();
  obj_dequarantining<t_param>(x);
  ossia_pd::instance().params.remove_all(x);

  outlet_free(x->x_dataout);
  outlet_free(x->x_dumpout);

  x->~t_param();
}

extern "C" void setup_ossia0x2eparam(void)
{
  t_eclass* c = eclass_new(
      "ossia.param", (method)parameter_new, (method)parameter_free,
      (short)sizeof(t_param), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)parameter_new,gensym("ø.param"), A_GIMME, 0);

    eclass_addmethod(c, (method) t_obj_base::obj_push, "anything", A_GIMME, 0);
    eclass_addmethod(c, (method) t_obj_base::obj_bang, "bang",     A_NULL,  0);
    eclass_addmethod(c, (method) obj_dump<t_param>,    "dump",     A_NULL,  0);
    eclass_addmethod(c, (method) parameter_notify,     "notify",   A_NULL,  0);

    CLASS_ATTR_SYMBOL(c, "type", 0, t_param, x_type);
    CLASS_ATTR_SYMBOL(c, "unit", 0, t_param, x_unit);
    CLASS_ATTR_SYMBOL(c, "bounding_mode", 0, t_param, x_bounding_mode);
    CLASS_ATTR_SYMBOL(c, "access_mode", 0, t_param, x_access_mode);
    CLASS_ATTR_SYMBOL(c, "description", 0, t_param, x_description);
    CLASS_ATTR_SYMBOL(c, "tags", 0, t_param, x_tags);

    CLASS_ATTR_ATOM_VARSIZE(c, "default", 0, t_param, x_default, x_default_size, OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_ATOM_VARSIZE(c, "range",   0, t_param, x_range,   x_range_size,   OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_ATOM_VARSIZE(c, "min",     0, t_param, x_min,     x_min_size,     OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_ATOM_VARSIZE(c, "max",     0, t_param, x_max,     x_max_size,     OSSIA_PD_MAX_ATTR_SIZE);

    CLASS_ATTR_FLOAT(c, "repetition_filter", 0, t_param, x_repetition_filter);
    CLASS_ATTR_INT(c, "priority", 0, t_param, x_priority);
    CLASS_ATTR_INT(c, "hidden", 0, t_param, x_hidden);
    CLASS_ATTR_INT(c, "enable", 0, t_param, x_enable);

    CLASS_ATTR_DEFAULT(c, "type", 0, "float");

    // eclass_register(CLASS_OBJ, c); // disable property dialog since it's
    // buggy
  }

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.param = c;
}
} // pd namespace
} // ossia namespace
