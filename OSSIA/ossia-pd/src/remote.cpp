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

static void remote_free(t_remote* x);

bool t_remote::register_node(std::vector<ossia::net::node_base*> node)
{
  bool res = do_registration(node);
  if (res)
  {
    obj_dequarantining<t_remote>(this);
  }
  else
    obj_quarantining<t_remote>(this);

  if (!node.empty() && m_is_pattern){
    // assume all nodes refer to the same device
    auto& dev = node[0]->get_device();
    if (&dev != m_dev)
    {
      if (m_dev) m_dev->on_parameter_created.disconnect<t_remote, &t_remote::on_parameter_created_callback>(this);
      m_dev = &dev;
      m_dev->on_parameter_created.connect<t_remote, &t_remote::on_parameter_created_callback>(this);
    }
  }

  return res;
}

bool t_remote::do_registration(std::vector<ossia::net::node_base*> _nodes)
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
    clock_delay(m_regclock, 0);
  }

  // do not put it in quarantine if it's a pattern
  // and even if it can't find any matching node
  return (!m_matchers.empty() || m_is_pattern);
}

bool t_remote::unregister()
{
  clock_unset(m_regclock);
  m_matchers.clear();
  m_nodes.clear();

  obj_quarantining<t_remote>(this);

  m_parent_node = nullptr;
  return true;
}


void t_remote::on_parameter_created_callback(const ossia::net::parameter_base& param)
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

void t_remote::set_unit()
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

void t_remote::set_mute()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_muted(*node, m_mute);
  }
}

void t_remote::set_enable()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_disabled(*node, !m_enable);
  }
}

void remote_get_unit(t_remote*x)
{
  t_atom a;
  if (x->m_unit)
  {
    SETSYMBOL(&a,x->m_unit);
    outlet_anything(x->m_dumpout, gensym("unit"), 1, &a);
  } else
    outlet_anything(x->m_dumpout, gensym("unit"), 0, NULL);
}

void remote_get_mute(t_remote*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_mute);
  outlet_anything(x->m_dumpout, gensym("mute"), 1, &a);
}

void remote_get_enable(t_remote*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_enable);
  outlet_anything(x->m_dumpout, gensym("mute"), 1, &a);
}

t_pd_err remote_notify(t_remote*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
{
  if (msg == gensym("attr_modified"))
  {
    if ( s == gensym("unit") )
      x->set_unit();
    else if ( s == gensym("mute") )
      x->set_mute();
    else if ( s == gensym("enable") )
      x->set_enable();
  }
}

static void remote_click(
    t_remote* x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift,
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

    t_device* device
        = (t_device*)find_parent(&x->m_obj, "ossia.device", 0, &l);

    if (!find_and_display_friend(x))
      pd_error(x, "sorry I can't find a connected friend :-(");
  }
  else
  {
    x->m_last_click = ms;
  }
}

static void remote_bind(t_remote* x, t_symbol* address)
{
  x->m_name = address;
  x->m_addr_scope = ossia::pd::get_address_scope(x->m_name->s_name);
  x->unregister();
  obj_register(x);
}

static void* remote_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_remote* x = (t_remote*)eobj_new(ossia_pd.remote);

  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    x->m_otype = object_class::remote;
    x->m_setout = outlet_new((t_object*)x, nullptr);
    x->m_dataout = outlet_new((t_object*)x, nullptr);
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
    new (&x->m_matchers) decltype(x->m_matchers);
    x->m_dev = nullptr;
    x->m_ounit = ossia::none;
    x->m_mute = false;
    x->m_enable = true;

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->m_name = atom_getsymbol(argv);
      x->m_addr_scope = ossia::pd::get_address_scope(x->m_name->s_name);
    }
    else
    {
      error("You have to pass a name as the first argument");
      x->m_name = gensym("untitledRemote");
    }

    x->m_is_pattern = ossia::traversal::is_pattern(x->m_name->s_name);

    x->m_clock = nullptr;
    x->m_regclock = clock_new(x, (t_method)t_object_base::obj_bang);

    x->m_parent_node = nullptr;

    ebox_attrprocess_viabinbuf(x, d);

    obj_register<t_remote>(x);
    ossia_pd.remotes.push_back(x);
  }

  return (x);
}

static void remote_free(t_remote* x)
{
  x->m_dead = true;
  x->unregister();
  obj_dequarantining<t_remote>(x);
  ossia_pd::instance().remotes.remove_all(x);

  if(x->m_is_pattern && x->m_dev)
  {
    x->m_dev->on_parameter_created.disconnect<t_remote, &t_remote::on_parameter_created_callback>(x);
  }

  outlet_free(x->m_setout);
  outlet_free(x->m_dataout);
  outlet_free(x->m_dumpout);
  x->~t_remote();
}

extern "C" void setup_ossia0x2eremote(void)
{
  t_eclass* c = eclass_new(
      "ossia.remote", (method)remote_new, (method)remote_free,
      (short)sizeof(t_remote), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)remote_new,gensym("ø.remote"), A_GIMME, 0);

    eclass_addmethod(c, (method) t_object_base::obj_push,   "anything",    A_GIMME,  0);
    eclass_addmethod(c, (method) t_object_base::obj_bang,   "bang",        A_NULL,   0);
    eclass_addmethod(c, (method) obj_dump<t_remote>,     "dump",        A_NULL,   0);
    eclass_addmethod(c, (method) remote_click,           "click",       A_NULL,   0);
    eclass_addmethod(c, (method) remote_notify,          "notify",      A_NULL,  0);
    eclass_addmethod(c, (method) remote_bind,            "bind",        A_SYMBOL, 0);
    eclass_addmethod(c, (method) obj_get_address,        "getaddress",  A_NULL,   0);

    CLASS_ATTR_SYMBOL(c, "unit",   0, t_remote, m_unit);
    CLASS_ATTR_INT   (c, "mute",   0, t_remote, m_mute);
    CLASS_ATTR_INT   (c, "enable", 0, t_remote, m_enable);

    CLASS_ATTR_DEFAULT(c, "unit", 0, "");

    eclass_addmethod(c, (method) remote_get_unit,        "getunit",     A_NULL, 0);
    eclass_addmethod(c, (method) remote_get_mute,        "getmute",     A_NULL, 0);
    eclass_addmethod(c, (method) remote_get_enable,      "getenable",   A_NULL, 0);

  }

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.remote = c;
}
} // pd namespace
} // ossia namespace
