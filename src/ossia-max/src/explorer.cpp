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
t_symbol* explorer::s_size = gensym("size");
t_symbol* explorer::s_namespace = gensym("namespace");

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
      c, (method)explorer::assist,
      "assist", A_CANT, 0);
  class_addmethod(
      c, (method)explorer::notify,
      "notify", A_CANT, 0);

  search_filter::setup_attribute<explorer>(c);

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
    // need to schedule a loadbang because objects only receive a loadbang when patcher loads.
    x->m_reg_clock = clock_new(x, (method) object_base::loadbang);
    clock_set(x->m_reg_clock, 1);
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
  outlet_delete(m_dumpout);
}

void explorer::parse_args(t_symbol* s, long argc, t_atom* argv)
{
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
  auto matchers = x->find_parent_nodes();
  x->register_node(matchers);
}

bool explorer::register_node(std::vector<std::shared_ptr<matcher>>& matchers)
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

  ossia::remove_erase_if(nodes, [&](const ossia::net::node_base* m){
    return filter(*m);
  });

  t_atom a;
  A_SETLONG(&a, nodes.size());
  outlet_anything(m_dumpout, s_size, 1, &a);
  for(const auto& n : nodes)
  {
    auto s = ossia::net::address_string_from_node(*n);
    A_SETSYM(&a, gensym(s.c_str()));
    outlet_anything(m_dumpout, s_namespace, 1, &a);
  }

  return true;
}

bool explorer::unregister()
{
  m_matchers.clear();

  return true;
}
