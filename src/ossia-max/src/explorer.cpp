// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/explorer.hpp>
#include <ossia/detail/thread.hpp>

#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <ossia/network/common/path.hpp>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_explorer class methods

t_symbol* explorer::s_explore = gensym("explore");
t_symbol* explorer::s_search = gensym("search");
t_symbol* explorer::s_monitor = gensym("monitor");

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
      "monitor", A_GIMME, 0);
  class_addmethod(
      c, (method)explorer::assist,
      "assist", A_CANT, 0);
  class_addmethod(
      c, (method)explorer::notify,
      "notify", A_CANT, 0);

  CLASS_ATTR_SYM(c, "type", 0, explorer, m_type);
  CLASS_ATTR_LABEL(c, "type", 0, "Type of nodes to search");

  CLASS_ATTR_SYM(c, "appname", 0, explorer, m_sort);
  CLASS_ATTR_LABEL(c, "appname", 0, "Sort method");

  CLASS_ATTR_LONG(c, "highlight", 0, explorer, m_highlight);
  CLASS_ATTR_LABEL(c, "highlight", 0, "Highlight objects returned by search");
  CLASS_ATTR_FILTER_CLIP(c, "heartbeat", 1, 1000);


  class_register(CLASS_BOX, ossia_library.ossia_explorer_class);
}

extern "C" void* ossia_explorer_new(t_symbol* s, long argc, t_atom* argv)
{
  return make_ossia<explorer>(argc, argv);
}

void explorer::free(explorer* x)
{
  if (x)
  {
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
  object_attach_byptr_register(this, this, CLASS_BOX);

  ossia_check_and_register(this);

  ossia_max::instance().explorers.push_back(this);
}

void explorer::stop_monitoring()
{
  for(const auto& dev : m_devices)
  {
    dev->on_parameter_created.disconnect<&explorer::on_parameter_created_callback>(this);
    dev->get_root_node().about_to_be_deleted.disconnect<&explorer::on_device_deleted>(this);
  }
  m_devices.clear();
}

void explorer::execute_method(ossia::max::explorer* x, t_symbol* s, long argc, t_atom* argv)
{
  x->stop_monitoring();
  x->m_method = s;
  x->m_node_name = nullptr;
  if(argc > 0 && argv->a_type == A_SYM)
  {
    x->m_node_name = argv->a_w.w_sym;
  }
}

bool explorer::register_node(const std::vector<std::shared_ptr<t_matcher>>& nodes)
{
  update_path();

  if(m_method == s_explore)
  {
    // TODO return namespace of given nodes
  }

  if(m_method == s_search)
  {
    // TODO only return given nodes
    for(const auto& n : nodes)
    {
      // output address
    }
  }

  if(m_method == s_monitor)
  {
    for(const auto& n : nodes)
    {
      m_devices.insert(&n->get_node()->get_device());
    }

    for(const auto& dev : m_devices)
    {
      // TODO register other signals according to attributes' values
      dev->on_parameter_created.connect<&explorer::on_parameter_created_callback>(this);
      dev->get_root_node().about_to_be_deleted.connect<&explorer::on_device_deleted>(this);
    }
  }

  return true;
}

/*
bool explorer::do_registration(const std::vector<std::shared_ptr<t_matcher>>& matchers)
{
  unregister();

  std::string name = m_name->s_name;

  for (auto& m : matchers)
  {
    auto node = m->get_node();
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

    m_matchers.reserve(m_matchers.size() + nodes.size());

    for (auto n : nodes){
      if (n->get_parameter()){
        m_matchers.emplace_back(std::make_shared<t_matcher>(n,this));
      } else {
        // if there is a node without address it might be a model
        // then look if that node have an eponyme child
        auto node = ossia::net::find_node(*n, fmt::format("{}/{}", name, name));
        if (node){
          m_matchers.emplace_back(std::make_shared<t_matcher>(node, this));
        }
      }
    }
  }

  fill_selection();

  // do not put it in quarantine if it's a pattern
  // and even if it can't find any matching node
  return (!m_matchers.empty() || m_is_pattern);
}
*/

bool explorer::unregister()
{
  m_matchers.clear();

  object_quarantining<explorer>(this);

  m_parent_node = nullptr;

  stop_monitoring();
  return true;
}

void explorer::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();

  if ( m_path )
  {
    if( ossia::traversal::match(*m_path, node) )
    {
      if(m_addr_scope == net::address_scope::relative)
      {
        m_parent_node = node.get_parent();
        std::string name(m_name->s_name);
        size_t pos = name.find('/', 0);
        while(pos != std::string::npos)
        {
          m_parent_node = node.get_parent();
          pos = name.find('/',pos+1);
        }
      }
      m_matchers.emplace_back(std::make_shared<t_matcher>(&node,this));
      fill_selection();
    }
  }
}

void explorer::on_device_deleted(const net::node_base &)
{
  m_dev = nullptr;
}
