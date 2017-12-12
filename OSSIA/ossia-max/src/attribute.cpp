// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/attribute.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <ossia/network/common/path.hpp>

using namespace ossia::max;

extern "C" void ossia_attribute_setup()
{
  auto c = class_new( "ossia.attribute",
      (method)attribute::create,
      (method)attribute::destroy,
      (long)sizeof(attribute), 0L,
      A_GIMME, 0);

  parameter_base::class_setup(c);

  class_addmethod(
        c, (method)attribute::assist,
        "assist", A_CANT, 0);
  class_addmethod(
        c, (method)attribute::notify,
        "notify", A_CANT, 0);

  class_addmethod(c, (method) parameter_base::get_mess_cb, "get",  A_SYM, 0);
  class_addmethod(c, (method) address_mess_cb<attribute>, "address",   A_SYM, 0);


  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_attribute_class = c;
}

namespace ossia
{
namespace max
{


void attribute::assist(attribute* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "All purpose input");
  }
  else
  {
    switch(a)
    {
      case 0:
        sprintf(s, "Dumpout");
        break;
      default:
        ;
    }
  }
}

bool attribute::register_node(const std::vector<ossia::net::node_base*>& node)
{
  if (m_mute) return false;

  update_path();

  bool res = do_registration(node);
  if (res)
  {
    object_dequarantining<attribute>(this);
  }
  else
    object_dequarantining<attribute>(this);

  if (!node.empty() && m_is_pattern){
    // assume all nodes refer to the same device
    auto& dev = node[0]->get_device();
    if (&dev != m_dev)
    {
      if (m_dev) {
          m_dev->on_parameter_created.disconnect<attribute, &attribute::on_parameter_created_callback>(this);
          m_dev->get_root_node().about_to_be_deleted.disconnect<attribute, &attribute::on_device_deleted>(this);
      }
      m_dev = &dev;
      m_dev->on_parameter_created.connect<attribute, &attribute::on_parameter_created_callback>(this);
      m_dev->get_root_node().about_to_be_deleted.connect<attribute, &attribute::on_device_deleted>(this);
    }
  }

  return res;
}

bool attribute::do_registration(const std::vector<ossia::net::node_base*>& _nodes)
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
      nodes = ossia::max::find_global_nodes(name);
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

  fill_selection();

  // do not put it in quarantine if it's a pattern
  // and even if it can't find any matching node
  return (!m_matchers.empty() || m_is_pattern);
}

bool attribute::unregister()
{

  m_matchers.clear();
  m_nodes.clear();

  object_quarantining<attribute>(this);

  m_parent_node = nullptr;
  if(m_dev)
  {
    m_dev->on_parameter_created.disconnect<attribute, &attribute::on_parameter_created_callback>(this);
    m_dev->get_root_node().about_to_be_deleted.disconnect<attribute, &attribute::on_device_deleted>(this);
  }
  m_dev = nullptr;
  return true;
}

void attribute::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();

  if ( m_path && ossia::traversal::match(*m_path, node) )
  {
    // TODO rethink that parent_node
    m_parent_node = node.get_parent();
    m_matchers.emplace_back(&node,this);
    m_nodes.push_back(&node);
    // TODO optimize : don't clear and iterate through all matchers
    // just add it if it matches instead
    fill_selection();
  }
}

void attribute::on_device_deleted(const net::node_base &)
{
  m_dev = nullptr;
}

t_max_err attribute::notify(attribute*x, t_symbol*, t_symbol* msg, void* sender, void* data)
{
    if (msg == gensym("attr_modified"))
    {
      t_symbol* s = (t_symbol *)object_method((t_object *)data, gensym("getname"));

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
        x->set_mute();
  }
  return 0;
}

void* attribute::create(t_symbol* name, int argc, t_atom* argv)
{
  auto x = make_ossia<attribute>();

  if (x)
  {
    x->m_otype = object_class::attribute;
    x->m_dumpout = outlet_new(x, NULL);

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
      }
    }

    if (x->m_name == _sym_nothing)
    {
      object_error((t_object*)x, "needs a name as first argument");
      x->m_name = gensym("untitledParameter");
      return x;
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    // Register object to istself so it can receive notification when attribute changed
    // This is not documented anywhere, please look at :
    // https://cycling74.com/forums/notify-when-attribute-changes
    object_attach_byptr_register(x, x, CLASS_BOX);

    // start registration
    max_object_register<attribute>(x);
    ossia_max::instance().attributes.push_back(x);

    x->update_path();
  }

  return (x);
}

void attribute::destroy(attribute* x)
{
  x->m_dead = true;
  x->unregister();
  object_dequarantining<attribute>(x);
  ossia_max::instance().attributes.remove_all(x);

  if(x->m_is_pattern && x->m_dev)
  {
    x->m_dev->on_parameter_created.disconnect<attribute, &attribute::on_parameter_created_callback>(x);
    x->m_dev->get_root_node().about_to_be_deleted.disconnect<attribute, &attribute::on_device_deleted>(x);
  }

  outlet_delete(x->m_dumpout);
  x->~attribute();
}

ossia::safe_set<attribute*>& attribute::quarantine()
{
  return ossia_max::instance().attribute_quarantine;
}

} // pd namespace
} // ossia namespace
