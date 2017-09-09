// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "remote.hpp"
#include "device.hpp"
#include "parameter.hpp"
#include <ossia-max/src/utils.hpp>

#include <ossia/network/common/path.hpp>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_remote class methods

extern "C" void ossia_remote_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.remote class
  t_class* c = class_new(
      "ossia.remote", (method)remote::create, (method)remote::destroy,
      (short)sizeof(remote), 0L, A_GIMME, 0);

  if (c)
  {
    parameter_base::class_setup(c);
    class_addmethod(
          c, (method)remote::bind,
          "bind", A_SYM, 0);
    class_addmethod(
        c, (method)remote::assist,
        "assist", A_CANT, 0);
    class_addmethod(
        c, (method)remote::notify,
        "notify", A_CANT, 0);
  }

  class_register(CLASS_BOX, c);
  ossia_library.ossia_remote_class = c;
}

namespace ossia
{
namespace max
{

void* remote::create(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();

  // ugly hack while waiting for C++ Max API
  auto place = object_alloc(ossia_library.ossia_remote_class);
  t_object tmp;
  memcpy(&tmp, place, sizeof(t_object));
  remote* x = new(place) remote();
  memcpy(x, &tmp, sizeof(t_object));

  if (x)
  {
    // make outlets:
    // anything outlet to dump remote state
    x->m_dumpout = outlet_new(x, NULL);
    // anything outlet to output data
    x->m_data_out = outlet_new(x, NULL);
    // anything outlet to output data for ui
    x->m_set_out  = outlet_new(x, NULL);

    x->m_dev = nullptr;
    x->m_clock = clock_new(x, (method)parameter_base::bang);
    x->m_poll_clock = clock_new(x, (method) parameter_base::output_value);

    x->m_otype = object_class::remote;

    // Register object to istself so it can receive notification when attribute changed
    // This is not documented anywhere, please look at :
    // https://cycling74.com/forums/notify-when-attribute-changes
    object_attach_byptr_register(x, x, CLASS_BOX);

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::max::get_address_scope(x->m_name->s_name);
      }
    }

    if (x->m_name == _sym_nothing)
    {
      object_error((t_object*)x, "needs a name as first argument");
      x->m_name = gensym("untitledRemote");
      x->update_path(x->m_name->s_name);
      return x;
    }

    x->update_path(x->m_name->s_name);

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    x->m_is_pattern = ossia::traversal::is_pattern(x->m_name->s_name);

    max_object_register<remote>(x);
    ossia_max::instance().remotes.push_back(x);
  }

  return (x);
}

void remote::destroy(remote* x)
{
  if (x->m_clock) {
    clock_free((t_object*)x->m_clock);
    x->m_clock = nullptr;
  }

  if (x->m_poll_clock)
  {
    clock_free((t_object*)x->m_poll_clock);
    x->m_poll_clock = nullptr;
  }
  x->m_dead = true;
  x->unregister();

  object_dequarantining<remote>(x);
  ossia_max::instance().remotes.remove_all(x);

  if(x->m_is_pattern && x->m_dev)
  {
    x->m_dev->on_parameter_created.disconnect<remote, &remote::on_parameter_created_callback>(x);
  }

  outlet_delete(x->m_dumpout);
  outlet_delete(x->m_set_out);
  outlet_delete(x->m_data_out);  
  x->~remote();
}

void remote::update_path(string_view name)
{
  bool is_pattern = ossia::traversal::is_pattern(name);

  if(is_pattern)
  {
    m_path = ossia::traversal::make_path(name);
  }
  else
  {
    m_path = ossia::none;
  }
}

void remote::assist(remote* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "I am inlet %ld", a);
  }
  else
  {
    switch(a)
    {
    case 0:
      sprintf(s, "deferred outlet with set prefix (for connecting to UI object)", a);
        break;
      case 1:
        sprintf(s, "raw outlet", a);
        break;
      case 2:
        sprintf(s, "dump outlet", a);
        break;
      default:
        break;
    }
  }
}


t_max_err remote::notify(remote *x, t_symbol *s,
                       t_symbol *msg, void *sender, void *data)
{
  t_symbol *attrname;

  if (msg == gensym("attr_modified") && sender == x) {
    attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));

    if( attrname == gensym("range") )
      x->set_range();
    else if ( attrname == gensym("clip") )
      x->set_bounding_mode();
    else if ( attrname == gensym("min") || attrname == gensym("max") )
      x->set_minmax();
    else if ( attrname == gensym("default") )
      x->set_default();
    else if ( attrname == gensym("unit") )
      x->set_unit();
    else if ( attrname == gensym("rate") )
      x->set_rate();
    else if ( attrname == gensym("hidden") )
      x->set_hidden();
    else if ( attrname == gensym("priority") )
      x->set_priority();
    else if ( attrname == gensym("mode") )
      x->set_access_mode();
    else if ( attrname == gensym("repetitions") )
      x->set_repetition_filter();
    else if ( attrname == gensym("tags") )
      x->set_tags();
    else if ( attrname == gensym("description") )
      x->set_description();
    else if ( attrname == gensym("enable") )
      x->set_enable();
    else if ( attrname == gensym("type") )
      x->set_type();
    else if ( attrname == gensym("mute") )
    {
      if (x->m_mute)
        x->unregister();
      else
        max_object_register(x);
    }

  }
  return 0;
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
    {
      object_error((t_object*)this, "wrong unit: %s", m_unit->s_name);
      m_ounit = ossia::none;
      m_unit = gensym("");
      return;
    }

    if ( !m_matchers.empty() )
    {
      auto dst_unit = m_matchers[0].get_node()->get_parameter()->get_unit();
      if (!ossia::check_units_convertible(*m_ounit,dst_unit)){
        auto src = ossia::get_pretty_unit_text(*m_ounit);
        auto dst = ossia::get_pretty_unit_text(dst_unit);
        object_error((t_object*)this, "sorry I don't know how to convert '%s' into '%s'",
                 src.c_str(), dst.c_str() );
        m_ounit = ossia::none;
        m_unit = gensym("");
      }
    }
  } else {
    m_ounit = ossia::none;
  }
}

void remote::set_rate()
{
  m_rate = m_rate < m_rate_min ? m_rate_min : m_rate;
}

/*
extern "C"
void ossia_remote_click(t_remote *x, t_floatarg xpos, t_floatarg ypos,
t_floatarg shift, t_floatarg ctrl, t_floatarg alt)
{
    using namespace std::chrono;

    milliseconds ms = duration_cast< milliseconds >(
system_clock::now().time_since_epoch() );

    milliseconds diff = (ms - x->m_last_click);

    if (diff.count() < 200)
    {
        x->m_last_click = milliseconds(0);

        int l;
        t_device *device = (t_device*) find_parent(&x->m_obj, "ossia.device",
0, &l);
*/
/*
 if (!device || !x->m_node || obj_isQuarantined<t_remote>(x)){
 pd_error(x, "sorry no device found, or not connected or quarantined...");
 return;
 }
 */
/*
        t_canvas *root = x->m_obj.o_canvas;
        while (root->gl_owner)
            root = root->gl_owner;

        if (!find_and_display_friend(x, root))
            pd_error(x,"sorry I can't find a connected friend :-(");
    }
    else
        x->m_last_click = ms;
}
*/

bool remote::register_node(const std::vector<ossia::net::node_base*>& node)
{
  if(m_mute) return false;

  bool res = do_registration(node);

  if (res)
  {
    object_dequarantining<remote>(this);
    parameter_base::bang(this);
    clock_delay(m_poll_clock,1);
  }
  else
    object_quarantining<remote>(this);

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
      nodes = ossia::max::find_global_nodes(name);
    else
      nodes = ossia::net::find_nodes(*node, name);

    m_nodes.reserve(m_nodes.size() + nodes.size());
    m_matchers.reserve(m_matchers.size() + nodes.size());

    for (auto n : nodes){
      if (n->get_parameter()){
        m_matchers.emplace_back(n, this);
        m_nodes.push_back(n);
      } else {
        // if there is a node without address it might be a model
        // then look if that node have an eponyme child
        fmt::MemoryWriter path;
        path << name << "/" << name;
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
  if(m_clock) clock_unset(m_clock);
  m_matchers.clear();
  m_nodes.clear();

  object_quarantining<remote>(this);

  m_parent_node = nullptr;
  return true;
}

void remote::on_parameter_created_callback(const ossia::net::parameter_base& addr)
{
  auto& node = addr.get_node();
  if (!m_name) return;
  update_path(m_name->s_name);

  // FIXME check for path validity
  if ( m_path && ossia::traversal::match(*m_path, node) )
  {
    m_matchers.emplace_back(&node,this);
  }
}

void remote::bind(remote* x, t_symbol* address)
{
  x->m_name = address;
  x->update_path(x->m_name->s_name);
  x->m_addr_scope = ossia::max::get_address_scope(x->m_name->s_name);
  x->unregister();
  max_object_register(x);
}

void remote::update_attribute(remote* x, ossia::string_view attribute)
{
  // @mute and @unit attributes are specific to each remote
  // it makes no sens to sens to change when an attribute changes
  if ( attribute == ossia::net::text_refresh_rate() )
  {
    // assume all matchers have the same bounding_mode
    ossia::max::t_matcher& m = x->m_matchers[0];
    ossia::net::node_base* node = m.get_node();

    auto rate = ossia::net::get_refresh_rate(*node);
    if (rate)
    {
      x->m_rate_min = *rate;
      x->m_rate = x->m_rate < x->m_rate_min ? x->m_rate_min : x->m_rate;
    }
    notify(x, nullptr, gensym("attr_modified"), 0L, 0L);

  } else if ( attribute == ossia::net::text_unit()) {
    // assume all matchers have the same bounding_mode
    ossia::max::t_matcher& m = x->m_matchers[0];
    ossia::net::node_base* node = m.get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    if (x->m_ounit && !ossia::check_units_convertible(param->get_unit(), *x->m_ounit))
    {
      x->m_ounit = param->get_unit();
      std::string unit = ossia::get_pretty_unit_text(param->get_unit());
      x->m_unit = gensym(unit.c_str());
    }

  } else {
    parameter_base::update_attribute(x, attribute);
  }
}

ossia::safe_vector<remote*>& remote::quarantine()
{
    return ossia_max::instance().remote_quarantine;
}


} // max namespace
} // ossia namespace
