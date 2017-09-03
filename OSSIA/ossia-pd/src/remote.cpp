// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "remote.hpp"
#include "device.hpp"
#include "parameter.hpp"
#include "utils.hpp"

#include <ossia/network/common/path.hpp>

namespace ossia
{
namespace pd
{

#pragma mark t_remote

static void remote_free(remote* x);

remote::remote():
  parameter_base{ossia_pd::remote_class}
{ }

bool remote::register_node(const std::vector<ossia::net::node_base*>& node)
{
  if (m_mute) return false;

  bool res = do_registration(node);
  if (res)
  {
    obj_dequarantining<remote>(this);

    clock_set(m_poll_clock,1);
  }
  else
    obj_quarantining<remote>(this);

  if (!node.empty() && m_is_pattern){
    // assume all nodes refer to the same device
    auto& dev = node[0]->get_device();
    if (&dev != m_dev)
    {
      if (m_dev) m_dev->on_parameter_created.disconnect<remote, &remote::on_parameter_created_callback>(this);
      m_dev = &dev;
      m_dev->on_parameter_created.connect<remote, &remote::on_parameter_created_callback>(this);
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

    if (m_addr_scope == address_scope::absolute)
    {
      // get root node
      node = &node->get_device().get_root_node();
      // and remove starting '/'
      name = name.substr(1);
    }

    m_parent_node = node;

    std::vector<ossia::net::node_base*> nodes{};

    if (m_addr_scope == address_scope::global)
      nodes = ossia::pd::find_global_nodes(name);
    else
      nodes = ossia::net::find_nodes(*node, name);

    for (auto n : nodes){
      if (n->get_parameter()){
        t_matcher matcher{n,this};
        m_matchers.push_back(std::move(matcher));
        m_nodes.push_back(n);
      } else {
        // if there is a node without address it might be a model
        // then look if that node have an eponyme child
        fmt::MemoryWriter path;
        path << name << "/" << name;
        auto node = ossia::net::find_node(*n, path.str());
        if (node){
          t_matcher matcher{node,this};
          m_matchers.push_back(std::move(matcher));
          m_nodes.push_back(n);
        }
      }
    }
    clock_delay(m_clock, 0);
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
  return true;
}

void remote::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();
  auto path = ossia::traversal::make_path(m_name->s_name);

  // FIXME check for path validity
  if ( path && ossia::traversal::match(*path, node) )
  {
    m_matchers.emplace_back(&node,this);
    m_nodes.push_back(&node);
  }
}

void remote::set_unit()
{
  if ( m_unit !=  gensym("") )
  {
    // TODO check for unit compatibility with parameter
    ossia::unit_t unit = ossia::parse_pretty_unit(m_unit->s_name);
    if (unit)
      m_ounit = unit;
    else
      pd_error(this, "wrong unit: %s", m_unit->s_name);
  }
}

void remote_get_unit(remote*x)
{
  t_atom a;
  if (x->m_unit)
  {
    SETSYMBOL(&a,x->m_unit);
    outlet_anything(x->m_dumpout, gensym("unit"), 1, &a);
  } else
    outlet_anything(x->m_dumpout, gensym("unit"), 0, NULL);
}

void remote_get_mute(remote*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_mute);
  outlet_anything(x->m_dumpout, gensym("mute"), 1, &a);
}

void remote_get_rate(remote*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_rate);
  outlet_anything(x->m_dumpout, gensym("rate"), 1, &a);
}

void remote_get_enable(remote*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_enable);
  outlet_anything(x->m_dumpout, gensym("enable"), 1, &a);
}

t_pd_err remote_notify(remote*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
{
  if (msg == gensym("attr_modified"))
  {
    if ( s == gensym("unit") )
      x->set_unit();
    else if ( s == gensym("mute") )
      if (x->m_mute)
        x->unregister();
      else
        obj_register(x);
    else if ( s == gensym("enable") )
      x->set_enable();
  }
  return {};
}

static void remote_click(
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

static void remote_bind(remote* x, t_symbol* address)
{
  std::string name = replace_brackets(address->s_name);
  x->m_name = gensym(name.c_str());
  x->m_addr_scope = ossia::pd::get_address_scope(x->m_name->s_name);
  x->unregister();
  obj_register(x);
}

static void* remote_new(t_symbol* name, int argc, t_atom* argv)
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
      x->m_addr_scope = ossia::pd::get_address_scope(x->m_name->s_name);
    }
    else
    {
      error("You have to pass a name as the first argument");
      x->m_name = gensym("untitledRemote");
    }

    x->m_is_pattern = ossia::traversal::is_pattern(x->m_name->s_name);

    x->m_clock = clock_new(x, (t_method)parameter_base::bang);
    x->m_poll_clock = clock_new(x, (t_method)parameter_base::output_value);

    ebox_attrprocess_viabinbuf(x, d);

    obj_register<remote>(x);
    ossia_pd.remotes.push_back(x);
  }

  return (x);
}

static void remote_free(remote* x)
{
  x->m_dead = true;
  x->unregister();
  obj_dequarantining<remote>(x);
  ossia_pd::instance().remotes.remove_all(x);

  if(x->m_is_pattern && x->m_dev)
  {
    x->m_dev->on_parameter_created.disconnect<remote, &remote::on_parameter_created_callback>(x);
  }

  clock_free(x->m_clock);
  clock_free(x->m_poll_clock);

  outlet_free(x->m_setout);
  outlet_free(x->m_dataout);
  outlet_free(x->m_dumpout);

  x->~remote();
}

void remote::update_attribute(remote* x, ossia::string_view attribute)
{
  // @mute and @unit attributes are specific to each remote
  // it makes no sens to sens to change when an attribute changes
  if ( attribute == ossia::net::text_refresh_rate() )
  {

  }
  parameter_base::update_attribute(x, attribute);
}

extern "C" void setup_ossia0x2eremote(void)
{
  t_eclass* c = eclass_new(
      "ossia.remote", (method)remote_new, (method)remote_free,
      (short)sizeof(remote), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)remote_new,gensym("ø.remote"), A_GIMME, 0);

    eclass_addmethod(c, (method) parameter_base::push,   "anything",    A_GIMME,  0);
    eclass_addmethod(c, (method) parameter_base::bang,   "bang",        A_NULL,   0);
    eclass_addmethod(c, (method) obj_dump<remote>,       "dump",        A_NULL,   0);
    eclass_addmethod(c, (method) remote_click,           "click",       A_NULL,   0);
    eclass_addmethod(c, (method) remote_notify,          "notify",      A_NULL,   0);
    eclass_addmethod(c, (method) remote_bind,            "bind",        A_SYMBOL, 0);
    eclass_addmethod(c, (method) object_base::get_address,        "getaddress",  A_NULL,   0);

    CLASS_ATTR_SYMBOL(c, "unit",          0, remote, m_unit);
    CLASS_ATTR_INT   (c, "mute",          0, remote, m_mute);
    CLASS_ATTR_INT   (c, "rate",          0, remote, m_rate);

    CLASS_ATTR_DEFAULT(c, "unit", 0, "");

    parameter_base::declare_attributes(c);

    // remote special attributes
    eclass_addmethod(c, (method) remote_get_unit,        "getunit",     A_NULL, 0);
    eclass_addmethod(c, (method) remote_get_mute,        "getmute",     A_NULL, 0);
    eclass_addmethod(c, (method) remote_get_rate,        "rate",        A_NULL, 0);

  }

  ossia_pd::remote_class = c;
}

ossia::safe_vector<remote*>& remote::quarantine()
{
    return ossia_pd::instance().remote_quarantine;
}

} // pd namespace
} // ossia namespace
