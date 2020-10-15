// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/explorer.hpp>
#include <ossia/detail/thread.hpp>

#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <ossia/network/common/path.hpp>

#include <boost/algorithm/string/predicate.hpp>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_explorer class methods

t_symbol* explorer::s_explore = gensym("explore");
t_symbol* explorer::s_search = gensym("search");
t_symbol* explorer::s_monitor = gensym("monitor");
t_symbol* explorer::s_size = gensym("size");
t_symbol* explorer::s_namespace = gensym("namespace");
t_symbol* explorer::s_open = gensym("open");

t_symbol* explorer::s_parameter = gensym("parameter");
t_symbol* explorer::s_node = gensym("node");
t_symbol* explorer::s_created = gensym("created");
t_symbol* explorer::s_removing = gensym("removing");
t_symbol* explorer::s_renamed = gensym("renamed");

extern "C" void ossia_explorer_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.explorer class
  ossia_library.ossia_explorer_class = class_new(
      "ossia.explorer", (method)ossia_explorer_new, (method)explorer::free,
      (long)sizeof(explorer), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_explorer_class;
  class_addmethod(
      c, (method)explorer::execute_method,
      "explore", A_GIMME, 0);
  class_addmethod(
      c, (method)explorer::execute_method,
      "search", A_GIMME, 0);
  class_addmethod(
      c, (method)explorer::execute_method,
      "open", A_GIMME, 0);
  class_addmethod(
      c, (method)explorer::execute_method,
      "monitor", A_GIMME, 0);
  class_addmethod(
      c, (method)explorer::assist,
      "assist", A_CANT, 0);
  class_addmethod(
      c, (method)explorer::notify,
      "notify", A_CANT, 0);

  CLASS_ATTR_SYM_VARSIZE(c, "tags", 0, explorer, m_tags, m_tags_size, MAX_NUM_ITEMS);
  CLASS_ATTR_LABEL(c, "tags", 0, "Filter by tags");

  CLASS_ATTR_SYM_VARSIZE(c, "mode", 0, explorer, m_modes, m_modes_size, MAX_NUM_ITEMS);
  CLASS_ATTR_LABEL(c, "mode", 0, "Filter by access mode");

  CLASS_ATTR_LONG(c, "visible", 0, explorer, m_visible);
  CLASS_ATTR_LABEL(c, "visible", 0, "Filter by visibility (0 means no filtering, 1 only visible, 2 only hidden");
  CLASS_ATTR_FILTER_CLIP(c, "visible", 0, 2);

  /*
  CLASS_ATTR_SYM(c, "sort", 0, explorer, m_sort);
  CLASS_ATTR_LABEL(c, "sort", 0, "Sort method");

  CLASS_ATTR_LONG(c, "highlight", 0, explorer, m_highlight);
  CLASS_ATTR_LABEL(c, "highlight", 0, "Highlight objects returned by search");
  CLASS_ATTR_FILTER_CLIP(c, "highlight", 0, 1);
  */

  CLASS_ATTR_LONG(c, "depth", 0, explorer, m_depth);
  CLASS_ATTR_LABEL(c, "depth", 0, "Limit exploration depth");
  CLASS_ATTR_FILTER_MIN(c, "depth", 0);

  class_register(CLASS_BOX, ossia_library.ossia_explorer_class);
}

extern "C" void* ossia_explorer_new(t_symbol*, long argc, t_atom* argv)
{
  auto x = make_ossia<explorer>(argc, argv);
  x->m_dumpout = outlet_new(x, NULL);

  object_attach_byptr_register(x, x, CLASS_BOX);

  ossia_max::instance().explorers.push_back(x);

  if(argc > 1 && argv[0].a_type == A_SYM && argv[1].a_type == A_SYM)
  {
    x->parse_args(argv[0].a_w.w_sym, argc-1, argv+1);
    ossia_check_and_register(x);
  }
  return x;
}

void explorer::free(explorer* x)
{
  if (x)
  {
    ossia_max::instance().explorers.remove_all(x);
    x->~explorer();
  }
}

t_max_err explorer::notify(
    explorer* x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
  return 0;
}

void explorer::assist(explorer *x, void *b, long m, long a, char *s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "Log messages inlet");
  }
}

#pragma mark -
#pragma mark t_explorer structure functions

explorer::explorer(long argc, t_atom *argv)
{

}

explorer::~explorer()
{
  stop_monitoring();
  outlet_delete(m_dumpout);
}

void explorer::stop_monitoring()
{
  for(const auto& dev : m_devices)
  {
    dev->on_node_created.disconnect<&explorer::on_node_created_callback>(this);
    dev->on_node_removing.disconnect<&explorer::on_node_removing_callback>(this);
    dev->on_node_renamed.disconnect<&explorer::on_node_renamed_callback>(this);
    dev->on_parameter_created.disconnect<&explorer::on_parameter_created_callback>(this);
    dev->on_parameter_removing.disconnect<&explorer::on_parameter_removing_callback>(this);
    dev->get_root_node().about_to_be_deleted.disconnect<&explorer::on_device_deleted>(this);
  }
  m_devices.clear();
}

void explorer::parse_args(t_symbol* s, long argc, t_atom* argv)
{
  stop_monitoring();
  m_method = s;
  m_name = nullptr;
  if(argc > 0 && argv->a_type == A_SYM)
  {
    m_name = argv->a_w.w_sym;
    m_addr_scope = ossia::net::get_address_scope(m_name->s_name);
  }
}

void explorer::execute_method(explorer* x, t_symbol* s, long argc, t_atom* argv)
{
  x->parse_args(s, argc, argv);
  ossia_register(x);
}

bool explorer::register_node(const std::vector<std::shared_ptr<t_matcher>>& matchers)
{
  update_path();

  if(m_method == s_explore)
  {
    // get namespace of given nodes
    std::vector<ossia::net::node_base*> nodes;
    for(const auto& m : matchers)
    {
      auto node = m->get_node();
      std::string address(m_name->s_name);
      std::vector<ossia::net::node_base*> found_nodes;
      if(m_addr_scope == ossia::net::address_scope::global)
      {
        found_nodes = {node};
      }
      else
      {
        found_nodes = ossia::net::find_nodes(*node, address);
      }

      for(const auto& n : found_nodes)
      {
        auto vec = ossia::net::list_all_children(n, m_depth);
        nodes.insert(nodes.end(), vec.begin(), vec.end());
      }
    }

    // TODO use ossia::remove_erase_if here ?
    std::vector<ossia::net::node_base*> filtered;
    filtered.reserve(nodes.size());
    for(const auto& n : nodes)
    {
      if(filter(*n))
        filtered.push_back(n);
    }

    t_atom a;
    A_SETLONG(&a, filtered.size());
    outlet_anything(m_dumpout, s_size, 1, &a);
    for(const auto& n : filtered)
    {
      auto s = ossia::net::address_string_from_node(*n);
      A_SETSYM(&a, gensym(s.c_str()));
      outlet_anything(m_dumpout, s_namespace, 1, &a);
    }
  }
  else if(m_method == s_search)
  {
    // only return given nodes addresses
    t_atom a;
    A_SETLONG(&a, matchers.size());
    outlet_anything(m_dumpout, s_size, 1, &a);
    for(const auto& m : matchers)
    {
      std::string addr = ossia::net::address_string_from_node(*m->get_node());
      A_SETSYM(&a, gensym(addr.c_str()));
      outlet_anything(m_dumpout, s_namespace, 1, &a);
    }
  }
  else if(m_method == s_monitor)
  {
    for(const auto& n : matchers)
    {
      m_devices.insert(&n->get_node()->get_device());
    }

    for(const auto& dev : m_devices)
    {
      // TODO register other signals according to attributes' values
      dev->on_node_created.connect<&explorer::on_node_created_callback>(this);
      dev->on_node_removing.connect<&explorer::on_node_removing_callback>(this);
      dev->on_node_renamed.connect<&explorer::on_node_renamed_callback>(this);
      dev->on_parameter_created.connect<&explorer::on_parameter_created_callback>(this);
      dev->on_parameter_removing.connect<&explorer::on_parameter_removing_callback>(this);
      dev->get_root_node().about_to_be_deleted.connect<&explorer::on_device_deleted>(this);
    }
  }
  else if(m_method == s_open)
  {

    auto open_parent = [](object_base* x, ossia::net::node_base* node)
    {
      for ( auto& om : x->m_matchers )
      {
        if ( om->get_node() == node )
        {
          t_object *jp{};

          // get the object's parent patcher
          object_obex_lookup(x, gensym("#P"), (t_object **)&jp);

          if (jp)
            typedmess(jp, gensym("front"), 0, NULL);	// opens the subpatcher

          x->highlight();
        }
      }
    };

    for(const auto& m : matchers)
    {
      auto node = m->get_node();

      if(node->get_parameter())
      {
        // if node has a paremeter, search only for ossia.parameter,
        // ossia.remote & ossia.attribute object
        for ( auto param : ossia_max::instance().parameters.reference() )
        {
          open_parent(param, node);
        }

        for ( auto remote : ossia_max::instance().remotes.reference() )
        {
          open_parent(remote, node);
        }

        for ( auto attr : ossia_max::instance().attributes.reference() )
        {
          open_parent(attr, node);
        }
      }
      else if(!node->get_parent())
      {
        // if node doesn't have a parent node, then search only
        // ossia.device and ossia.client objects
        for ( auto dev : ossia_max::instance().devices.reference() )
        {
          open_parent(dev, node);
        }

        for ( auto client : ossia_max::instance().clients.reference() )
        {
          open_parent(client, node);
        }
      }
      else
      {
        // if node has a parent but no paremeter,
        // search only for ossia.model and ossia.view object
        for ( auto model : ossia_max::instance().models.reference() )
        {
          open_parent(model, node);
        }

        for ( auto view : ossia_max::instance().views.reference() )
        {
          open_parent(view, node);
        }
      }

    }
  }

  return true;
}

bool explorer::filter(const ossia::net::node_base& node)
{
  if(m_tags_size > 0)
  {
    bool match = false;
    auto tags_opt = ossia::net::get_tags(node.get_extended_attributes());
    if(!tags_opt.has_value())
      return false;

    for(int i = 0; i < m_tags_size; i++)
    {
      if(ossia::find(tags_opt.value(), std::string(m_tags[i]->s_name)) != tags_opt.value().end())
      {
        match = true;
        break;
      }
    }
    if(!match)
      return false;
  }

  if(m_visible > 0)
  {
    auto hidden = ossia::net::get_hidden(node.get_extended_attributes());
    switch(m_visible)
    {
      case 1:
        if(hidden) return false;
        break;
      case 2:
        if(!hidden) return false;
        break;
      default:
          ;
    }
  }

  if(m_modes_size > 0)
  {
    auto access_opt = ossia::net::get_access_mode(node);
    if(!access_opt.has_value())
      return false;
    bool match = false;
    for(int i = 0; i<m_modes_size; i++)
    {
      if(symbol2access_mode(m_modes[i]) == access_opt.value())
      {
        match = true;
        break;
      }
    }
    if(!match)
      return false;
  }

  return true;
}

bool explorer::unregister()
{
  m_matchers.clear();

  object_quarantining<explorer>(this);

  m_parent_node = nullptr;

  stop_monitoring();
  return true;
}

void explorer::on_node_created_callback(const ossia::net::node_base& node)
{
  handle_modification(node, s_node, s_created);
}

void explorer::on_node_removing_callback(const ossia::net::node_base& node)
{
  handle_modification(node, s_node, s_created);
}

void explorer::on_node_renamed_callback(const ossia::net::node_base& node, const std::string&)
{
  handle_modification(node, s_node, s_created);
}

void explorer::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  handle_modification(param.get_node(), s_parameter, s_created);
}

void explorer::on_parameter_removing_callback(const ossia::net::parameter_base& param)
{
  handle_modification(param.get_node(), s_parameter, s_removing);
}

void explorer::handle_modification(const ossia::net::node_base& node, t_symbol* type, t_symbol* action)
{
  if ( m_path )
  {
    if( ossia::traversal::match(*m_path, node) )
    {
      t_atom a[3];
      A_SETSYM(a, s_parameter);
      A_SETSYM(a+1, s_created);
      std::string address = ossia::net::osc_parameter_string_with_device(node);
      A_SETSYM(a+2, gensym(address.c_str()));
      outlet_anything(m_dumpout, s_monitor, 3, a);
    }
  }
  else
  {
    std::string address = ossia::net::osc_parameter_string_with_device(node);
    if( boost::algorithm::ends_with(address, m_name->s_name) )
    {
      t_atom a[3];
      A_SETSYM(a, s_parameter);
      A_SETSYM(a+1, s_created);
      A_SETSYM(a+2, gensym(address.c_str()));
      outlet_anything(m_dumpout, s_monitor, 3, a);
    }
  }
}

void explorer::on_device_deleted(const net::node_base & node)
{
  auto it = m_devices.find(&node.get_device());
  if(it != m_devices.end())
  {
    m_devices.erase(it);
  }
}

ossia::safe_set<explorer*>& explorer::quarantine()
{
  return ossia_max::instance().explorer_quarantine;
}
