// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-pd/src/remote.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/utils.hpp>

#include <ossia/network/common/path.hpp>

namespace ossia
{
namespace pd
{

#pragma mark t_remote

remote::remote():
  parameter_base{ossia_pd::remote_class}
{ }

bool remote::register_node(const std::vector<ossia::net::node_base*>& node)
{
  if (m_mute) return false;

  bool res = do_registration(node);
  if (res)
  {
    fill_selection();
    obj_dequarantining<remote>(this);
    parameter_base::bang(this);
    clock_set(m_poll_clock,1);
  }
  else
    obj_quarantining<remote>(this);

  if (!node.empty() && m_is_pattern){
    // assume all nodes refer to the same device
    auto& dev = node[0]->get_device();
    if (&dev != m_dev)
    {
      if (m_dev) {
          m_dev->on_parameter_created.disconnect<remote, &remote::on_parameter_created_callback>(this);
          m_dev->get_root_node().about_to_be_deleted.disconnect<remote, &remote::on_device_deleted>(this);
      }
      m_dev = &dev;
      m_dev->on_parameter_created.connect<remote, &remote::on_parameter_created_callback>(this);
      m_dev->get_root_node().about_to_be_deleted.connect<remote, &remote::on_device_deleted>(this);
    }
  }

  return res;
}

bool remote::do_registration(const std::vector<ossia::net::node_base*>& _nodes)
{
  unregister();

  std::string name = m_name->s_name;

  for (auto node : _nodes)
  {
    if (m_addr_scope == net::address_scope::absolute)
    {
      // get root node
      node = &node->get_device().get_root_node();
      // and remove starting '/'
      name = name.substr(1);
    }

    m_parent_node = node;

    std::vector<ossia::net::node_base*> nodes{};

    if (m_addr_scope == net::address_scope::global)
      nodes = ossia::pd::find_global_nodes(name);
    else
      nodes = ossia::net::find_nodes(*node, name);

    m_nodes.reserve(m_nodes.size() + nodes.size());
    m_matchers.reserve(m_matchers.size() + nodes.size());

    for (auto n : nodes){
      if (n->get_parameter()){
        m_matchers.emplace_back(n,this);
        m_nodes.push_back(n);
      } else {
        // if there is a node without address it might be a model
        // then look if that node have an eponyme child
        fmt::MemoryWriter path;
        fmt::BasicStringRef<char> name_fmt(name.data(), name.size());
        path << name_fmt << "/" << name_fmt;
        auto node = ossia::net::find_node(*n, path.str());
        if (node){
          m_matchers.emplace_back(node, this);
          m_nodes.push_back(n);
        }
      }
    }
  }

  // do not put it in quarantine if it's a pattern
  // and even if it can't find any matching node
  return (!m_matchers.empty() || m_is_pattern);
}

bool remote::unregister()
{
  clock_unset(m_clock);
  clock_unset(m_poll_clock);

  m_matchers.clear();
  m_nodes.clear();

  obj_quarantining<remote>(this);

  m_parent_node = nullptr;
  if(m_dev)
  {
    m_dev->on_parameter_created.disconnect<remote, &remote::on_parameter_created_callback>(this);
    m_dev->get_root_node().about_to_be_deleted.disconnect<remote, &remote::on_device_deleted>(this);
  }
  m_dev = nullptr;
  return true;
}

void remote::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();

  // FIXME check for path validity
  if ( m_path && ossia::traversal::match(*m_path, node) )
  {
    m_matchers.emplace_back(&node,this);
    m_nodes.push_back(&node);
    fill_selection();
  }
}

void remote::set_unit()
{
  if ( m_unit !=  gensym("") )
  {
    ossia::unit_t unit = ossia::parse_pretty_unit(m_unit->s_name);
    if (unit)
      m_ounit = unit;
    else
    {
      pd_error(this, "wrong unit: %s", m_unit->s_name);
      m_ounit = ossia::none;
      m_unit = gensym("");
      return;
    }

    bool break_flag = false;
    for (auto& m : m_matchers)
    {
      auto dst_unit = m.get_node()->get_parameter()->get_unit();
      if (!ossia::check_units_convertible(*m_ounit,dst_unit)){
        auto src = ossia::get_pretty_unit_text(*m_ounit);
        auto dst = ossia::get_pretty_unit_text(dst_unit);
        pd_error(this, "sorry I don't know how to convert '%s' into '%s'",
                 src.c_str(), dst.c_str() );
        m_ounit = ossia::none;
        m_unit = gensym("");
        break_flag = true;
        break;
      }
    }
    if (!break_flag)
      parameter_base::bang(this);

  } else {
    m_ounit = ossia::none;
  }
}

void remote::set_rate()
{
  m_rate = m_rate < m_rate_min ? m_rate_min : m_rate;
}

void remote::get_unit(remote*x)
{
  t_atom a;
  if (x->m_unit)
  {
    SETSYMBOL(&a,x->m_unit);
    outlet_anything(x->m_dumpout, gensym("unit"), 1, &a);
  } else
    outlet_anything(x->m_dumpout, gensym("unit"), 0, NULL);
}

void remote::get_mute(remote*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_mute);
  outlet_anything(x->m_dumpout, gensym("mute"), 1, &a);
}

void remote::get_rate(remote*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_rate);
  outlet_anything(x->m_dumpout, gensym("rate"), 1, &a);
}

void remote::on_device_deleted(const net::node_base &)
{
  m_dev = nullptr;
}

void remote::update_path(string_view name)
{
    m_is_pattern = ossia::traversal::is_pattern(name);

    if(m_is_pattern)
    {
        m_path = ossia::traversal::make_path(name);
    }
    else
    {
        m_path = ossia::none;
    }
}

t_pd_err remote::notify(remote*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
{
    // TODO : forward notification to parent class
    if (msg == gensym("attr_modified"))
    {
        if( s == gensym("range") )
            x->set_range();
        else if ( s == gensym("clip") )
        x->set_bounding_mode();
      else if ( s == gensym("min") || s == gensym("max") )
        x->set_minmax();
      else if ( s == gensym("default") )
        x->set_default();
      else if ( s == gensym("unit") )
        x->set_unit();
      else if ( s == gensym("hidden") )
        x->set_hidden();
      else if ( s == gensym("priority") )
        x->set_priority();
      else if ( s == gensym("mode") )
        x->set_access_mode();
      else if ( s == gensym("repetitions") )
        x->set_repetition_filter();
      else if ( s == gensym("tags") )
        x->set_tags();
      else if ( s == gensym("description") )
        x->set_description();
      else if ( s == gensym("enable") )
        x->set_enable();
      else if ( s == gensym("type") )
        x->set_type();
      else if ( s == gensym("rate") )
        x->set_rate();
      else if ( s == gensym("mute") )
      {
        if (x->m_mute)
          x->unregister();
        else
          obj_register(x);
      }
  }
  return {};
}

void remote::click(
    remote* x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift,
    t_floatarg ctrl, t_floatarg alt)
{

  using namespace std::chrono;
  milliseconds ms
      = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  milliseconds diff = (ms - x->m_last_click);
  if (diff.count() < 200)
  {
    x->m_last_click = milliseconds(0);

    int l;

    ossia::pd::device* device
        = (ossia::pd::device*)find_parent(&x->m_obj, "ossia.device", 0, &l);

    if (!object_base::find_and_display_friend(x))
      pd_error(x, "sorry I can't find a connected friend :-(");
  }
  else
  {
    x->m_last_click = ms;
  }
}

void remote::bind(remote* x, t_symbol* address)
{
  // TODO maybe instead use a temporary local char array.
  std::string name = replace_brackets(address->s_name);
  x->m_name = gensym(name.c_str());
  x->update_path(name);
  x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
  x->unregister();
  obj_register(x);
}

void* remote::create(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  remote* x = new remote();

  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    x->m_otype = object_class::remote;
    x->m_setout = outlet_new((t_object*)x, nullptr);
    x->m_dataout = outlet_new((t_object*)x, nullptr);
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      t_symbol* address = atom_getsymbol(argv);
      std::string name = replace_brackets(address->s_name);
      x->m_name = gensym(name.c_str());
      x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
    }
    else
    {
      error("You have to pass a name as the first argument");
      x->m_name = gensym("untitledRemote");
    }

    x->update_path(x->m_name->s_name);

    x->m_clock = clock_new(x, (t_method)parameter_base::bang);
    x->m_poll_clock = clock_new(x, (t_method)parameter_base::output_value);

    ebox_attrprocess_viabinbuf(x, d);

    obj_register<remote>(x);
    ossia_pd.remotes.push_back(x);
  }

  return (x);
}

void remote::destroy(remote* x)
{
  x->m_dead = true;
  x->unregister();
  obj_dequarantining<remote>(x);
  ossia_pd::instance().remotes.remove_all(x);

  if(x->m_is_pattern && x->m_dev)
  {
    x->m_dev->on_parameter_created.disconnect<remote, &remote::on_parameter_created_callback>(x);
    x->m_dev->get_root_node().about_to_be_deleted.disconnect<remote, &remote::on_device_deleted>(x);
  }

  clock_free(x->m_clock);
  clock_free(x->m_poll_clock);

  outlet_free(x->m_setout);
  outlet_free(x->m_dataout);
  outlet_free(x->m_dumpout);

  x->~remote();
}

void remote::update_attribute(remote* x, ossia::string_view attribute, const ossia::net::node_base* node)
{
  // @mute and @unit attributes are specific to each remote
  // it makes no sens to sens to change when an attribute changes
  if ( attribute == ossia::net::text_refresh_rate() )
  {
    for (auto m : x->m_node_selection)
    {
      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

      auto rate = ossia::net::get_refresh_rate(*m->get_node());
      if (rate)
      {
        x->m_rate_min = *rate;
        x->m_rate = x->m_rate < x->m_rate_min ? x->m_rate_min : x->m_rate;
      }

      t_atom a;
      SETFLOAT(&a,x->m_rate);
      outlet_anything(x->m_dumpout, gensym("rate"), 1, &a);
    }
  } else if ( attribute == ossia::net::text_unit()) {
    for (auto m : x->m_node_selection)
    {
      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      ossia::net::parameter_base* param = m->get_node()->get_parameter();

      if (x->m_ounit && !ossia::check_units_convertible(param->get_unit(), *x->m_ounit))
      {
        x->m_ounit = param->get_unit();
        std::string unit = ossia::get_pretty_unit_text(param->get_unit());
        x->m_unit = gensym(unit.c_str());
      }
    }
  } else {
    parameter_base::update_attribute(x, attribute, node);
  }
}

extern "C" void setup_ossia0x2eremote(void)
{
  t_eclass* c = eclass_new("ossia.remote",
      (method)ossia::pd::remote::create,
      (method)ossia::pd::remote::destroy,
      (short)sizeof(remote), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)remote::create,gensym("ø.remote"), A_GIMME, 0);

    eclass_addmethod(c, (method) remote::click,           "click",       A_NULL,   0);
    eclass_addmethod(c, (method) remote::notify,          "notify",      A_NULL,   0);
    eclass_addmethod(c, (method) remote::bind,            "bind",        A_SYMBOL, 0);

    CLASS_ATTR_DEFAULT(c, "unit", 0, "");

    parameter_base::class_setup(c);

    // remote special attributes
    eclass_addmethod(c, (method) remote::get_unit,        "getunit",     A_NULL, 0);
    eclass_addmethod(c, (method) remote::get_mute,        "getmute",     A_NULL, 0);
    eclass_addmethod(c, (method) remote::get_rate,        "rate",        A_NULL, 0);

  }

  ossia_pd::remote_class = c;
}

ossia::safe_set<remote*>& remote::quarantine()
{
  return ossia_pd::instance().remote_quarantine;
}

} // pd namespace
} // ossia namespace
