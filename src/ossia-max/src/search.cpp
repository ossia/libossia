// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/search.hpp>
#include <ossia/detail/thread.hpp>

#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <ossia/network/common/path.hpp>

#include <boost/algorithm/string/predicate.hpp>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_search class methods

t_symbol* search::s_search = gensym("search");
t_symbol* search::s_size = gensym("size");
t_symbol* search::s_result = gensym("result");
t_symbol* search::s_open = gensym("open");

extern "C" void ossia_search_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.search class
  ossia_library.ossia_search_class = class_new(
      "ossia.search", (method)ossia_search_new, (method)search::free,
      (long)sizeof(search), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_search_class;
  class_addmethod(
      c, (method)search::execute_method,
      "search", A_GIMME, 0);
  class_addmethod(
      c, (method)search::execute_method,
      "open", A_GIMME, 0);
  class_addmethod(
      c, (method)search::assist,
      "assist", A_CANT, 0);
  class_addmethod(
      c, (method)search::notify,
      "notify", A_CANT, 0);

  search_filter::setup_attribute<search>(c);

  class_register(CLASS_BOX, ossia_library.ossia_search_class);
}

extern "C" void* ossia_search_new(t_symbol*, long argc, t_atom* argv)
{
  auto x = make_ossia<search>(argc, argv);
  x->m_dumpout = outlet_new(x, NULL);
  x->m_otype = object_class::search;

  object_attach_byptr_register(x, x, CLASS_BOX);

  ossia_max::instance().searchs.push_back(x);

  return x;
}

void search::free(search* x)
{
  if (x)
  {
    ossia_max::instance().searchs.remove_all(x);
    x->~search();
  }
}

t_max_err search::notify(
    search* x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
  return 0;
}

void search::assist(search *x, void *b, long m, long a, char *s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "Log messages inlet");
  }
}

#pragma mark -
#pragma mark t_search structure functions

search::search(long argc, t_atom *argv)
{

}

search::~search()
{
  outlet_delete(m_dumpout);
}

void search::execute_method(search* x, t_symbol* s, long argc, t_atom* argv)
{
  x->m_name = _sym_nothing;
  if(argc > 0 && argv->a_type == A_SYM)
  {
    x->m_name = argv->a_w.w_sym;
  }
  else
  {
    x->m_name = gensym("/");
  }
  x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);

  auto matchers = x->find_or_create_matchers();

  ossia::remove_erase_if(matchers, [&](const std::shared_ptr<matcher>& m)
                         { return x->filter(*m->get_node()); });

  if(s == s_search)
  {
    std::vector<matcher*> condidates;
    condidates.reserve(128);

    for(const auto& match : matchers)
    {
      auto node = match->get_node();
      auto all = ossia_max::s_node_matchers_map[node].reference();

      for(const auto& c : all)
      {
        if(c->get_owner() != x)
        {
          condidates.push_back(c);
        }
      }
    }

    // only return given nodes addresses
    t_atom a[2];
    A_SETLONG(a, condidates.size());
    outlet_anything(x->m_dumpout, s_size, 1, a);

    for(const auto& m : condidates)
    {
      object_base* obj = m->get_owner();
      if(obj != x)
      {
        std::string addr = ossia::net::address_string_from_node(*m->get_node());
        A_SETSYM(a, object_classname(m->get_owner()));
        A_SETSYM(a+1, gensym(addr.c_str()));
        outlet_anything(x->m_dumpout, s_result, 2, a);
      }
    }
  }
  else if(s == s_open)
  {
    for(const auto& m : matchers)
    {
      auto node = m->get_node();
      auto candidates = ossia_max::s_node_matchers_map[node].reference();
      for(const auto& m : candidates)
      {
        object_base* obj = m->get_owner();
        if(obj != x)
        {
          typedmess(obj->m_patcher, gensym("front"), 0, NULL);	// opens the subpatcher
          obj->highlight();
        }
      }
    }
  }
}

bool search::unregister()
{
  m_node_selection.clear();
  m_matchers.clear();

  ossia_max::instance().searchs.push_back(this);

  return true;
}
