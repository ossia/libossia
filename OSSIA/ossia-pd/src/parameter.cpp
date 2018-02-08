// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/parameter.hpp>
#include <ossia-pd/src/remote.hpp>
#include <ossia-pd/src/utils.hpp>
#include <sstream>
#include <boost/algorithm/string/case_conv.hpp>

namespace ossia
{
namespace pd
{

parameter::parameter():
  parameter_base{ossia_pd::param_class}
{
}

bool parameter::register_node(const std::vector<t_matcher>& matchers)
{
  bool res = do_registration(matchers);

  // TODO should we put this into device_base::on_parameter_deleted_callback ?
  // the drawback is that when the parameter is created, it is not fully configured

  for (auto remote : ossia::pd::remote::quarantine().copy())
  {
    obj_register(remote);
  }
  for (auto attribute : ossia::pd::attribute::quarantine().copy())
  {
    obj_register(attribute);
  }

  push_default_value(this);
  clock_delay(m_poll_clock,1);

  return res;
}

bool parameter::do_registration(const std::vector<t_matcher>& matchers)
{
  unregister(); // we should unregister here because we may have add a node
                // between the registered node and the parameter

  for (auto& m : matchers)
  {
    auto node = m.get_node();
    m_parent_node = node;

    auto nodes = ossia::net::create_nodes(*node, m_name->s_name);

    for (auto n : nodes)
    {

      auto local_param = ossia::try_setup_parameter(m_type->s_name, *n);

      if (!local_param)
      {
        pd_error(
              this,
              "type should one of: float, symbol, int, vec2f, "
              "vec3f, vec4f, bool, list, char");
        return false;
      }

      local_param->set_repetition_filter(
            m_repetitions ? ossia::repetition_filter::ON
                                : ossia::repetition_filter::OFF);

      ossia::net::set_priority(local_param->get_node(), m_priority);

      ossia::net::set_disabled(local_param->get_node(), !m_enable);

      ossia::net::set_hidden(local_param->get_node(), m_hidden);

      m_matchers.emplace_back(n, this);

    }
  }

  fill_selection();

  set_description();
  set_tags();
  set_access_mode();
  set_unit();
  set_bounding_mode();
  set_range();
  set_minmax();
  set_default();
  set_rate();
  set_repetition_filter();
  set_recall_safe();

  return true;
}

bool parameter::unregister()
{
  clock_unset(m_poll_clock);

  m_node_selection.clear();
  m_matchers.clear();

  for (auto remote : ossia::pd::remote::quarantine().copy())
  {
    obj_register(remote);
  }
  for (auto attribute : ossia::pd::attribute::quarantine().copy())
  {
    obj_register(attribute);
  }

  return true;
}

void* parameter::create(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  parameter* x = new parameter();

  // TODO SANITIZE : memory leak
  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    ossia_pd.params.push_back(x);
    x->m_otype = object_class::param;

    x->m_dataout = outlet_new((t_object*)x, nullptr);
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

    x->m_access_mode = gensym("rw");
    x->m_bounding_mode = gensym("free");
    x->m_unit = gensym("");
    x->m_type = gensym("float");

    x->m_poll_clock = clock_new(x, (t_method)parameter_base::output_value);

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      t_symbol* address = atom_getsymbol(argv);
      std::string name = replace_brackets(address->s_name);
      x->m_name = gensym(name.c_str());
      x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
    }
    else
    {
      pd_error(x, "You have to pass a name as the first argument");
      x->m_name = gensym("untitledParam");
    }

    ebox_attrprocess_viabinbuf(x, d);

    // change some attributes names to lower case
    std::string type = x->m_type->s_name;
    boost::algorithm::to_lower(type);
    x->m_type = gensym(type.c_str());

    if(x->m_type != gensym("string")
       && x->m_min_size == 0
       && x->m_max_size == 0
       && x->m_range_size == 0)
    {
      // set range if not set by attribute min/max or range
      x->m_range_size = 2;
      SETFLOAT(x->m_range,0);
      SETFLOAT(x->m_range+1,1);
    }

#ifdef OSSIA_PD_BENCHMARK
    std::cout << measure<>::execution(obj_register<parameter>, x) / 1000. << " ms "
              << " " << x << " parameter " << x->m_name->s_name
              << " " << x->m_reg_count << std::endl;

#else
    obj_register(x);
#endif
  }

  return (x);
}

void parameter::update_attribute(parameter* x, string_view attribute, const net::node_base* node)
{
  auto matchers = make_matchers_vector(x,node);

  if ( attribute == ossia::net::text_muted() ){
    get_mute(x, matchers);
  } else if ( attribute == ossia::net::text_unit() ){
    get_unit(x, matchers);
  } else
    parameter_base::update_attribute((parameter_base*)x, attribute, node);
}

t_pd_err parameter::notify(parameter*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
{
  if (msg == gensym("attr_modified"))
  {
    if ( s == gensym("unit") )
      x->set_unit();
    else if ( s == gensym("mute") )
      x->set_mute();
    else
      parameter_base::notify((parameter_base*)x,s,msg,sender,data);
  }
  return {};
}

void parameter::destroy(parameter* x)
{
  x->m_dead = true;
  x->unregister();
  ossia_pd::instance().params.remove_all(x);

  outlet_free(x->m_dataout);
  outlet_free(x->m_dumpout);

  clock_free(x->m_poll_clock);

  x->~parameter();
}

extern "C" void setup_ossia0x2eparam(void)
{
  t_eclass* c = eclass_new(
      "ossia.param", (method)parameter::create, (method)parameter::destroy,
      (short)sizeof(parameter), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)parameter::create,gensym("ø.param"), A_GIMME, 0);
    class_addcreator((t_newmethod)parameter::create,gensym("ossia.parameter"), A_GIMME, 0);
    class_addcreator((t_newmethod)parameter::create,gensym("ø.parameter"), A_GIMME, 0);

    parameter_base::class_setup(c);

    eclass_addmethod(c, (method) parameter::notify,    "notify",   A_NULL,  0);
    eclass_addmethod(c, (method) parameter_base::get_mess_cb, "get", A_SYMBOL, 0);
    eclass_addmethod(c, (method) address_mess_cb<parameter>, "address",   A_SYMBOL, 0);

    // special attributes
    CLASS_ATTR_DEFAULT(c, "type", 0, "float");
    CLASS_ATTR_DEFAULT(c, "clip", 0, "free");
    CLASS_ATTR_DEFAULT(c, "mode", 0, "bi");

    eclass_register(CLASS_OBJ, c);
  }

  ossia_pd::param_class = c;
}
} // pd namespace
} // ossia namespace
