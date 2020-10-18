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

  object_attach_byptr_register(x, x, CLASS_BOX);

  ossia_max::instance().searchs.push_back(x);

  if(argc > 1 && argv[0].a_type == A_SYM && argv[1].a_type == A_SYM)
  {
    x->parse_args(argv[0].a_w.w_sym, argc-1, argv+1);
    ossia_check_and_register(x);
  }
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

void search::parse_args(t_symbol* s, long argc, t_atom* argv)
{
  m_method = s;
  m_name = nullptr;
  if(argc > 0 && argv->a_type == A_SYM)
  {
    m_name = argv->a_w.w_sym;
    m_addr_scope = ossia::net::get_address_scope(m_name->s_name);
  }
}

void search::execute_method(search* x, t_symbol* s, long argc, t_atom* argv)
{
  x->parse_args(s, argc, argv);
  ossia_register(x);
}

bool search::register_node(std::vector<std::shared_ptr<t_matcher>>& matchers)
{
  update_path();

  ossia::remove_erase_if(matchers, [&](const std::shared_ptr<t_matcher>& m)
                         { return !filter(*m->get_node()); });

  if(m_method == s_search)
  {
    // only return given nodes addresses
    t_atom a;
    A_SETLONG(&a, matchers.size());
    outlet_anything(m_dumpout, s_size, 1, &a);
    for(const auto& m : matchers)
    {
      std::string addr = ossia::net::address_string_from_node(*m->get_node());
      A_SETSYM(&a, gensym(addr.c_str()));
      outlet_anything(m_dumpout, s_result, 1, &a);
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

bool search::unregister()
{
  m_matchers.clear();

  object_quarantining<search>(this);

  m_parent_node = nullptr;

  return true;
}

ossia::safe_set<search*>& search::quarantine()
{
  return ossia_max::instance().search_quarantine;
}
