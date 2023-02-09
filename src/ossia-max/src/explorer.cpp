// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/common/path.hpp>

#include <ossia-max/src/explorer.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <boost/algorithm/string/predicate.hpp>

using namespace ossia::max_binding;

#pragma mark -
#pragma mark ossia_explorer class methods

extern "C" void ossia_explorer_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.explorer class
  ossia_library.ossia_explorer_class = class_new(
      "ossia.explorer", (method)ossia_explorer_new, (method)explorer::free,
      (long)sizeof(explorer), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_explorer_class;
  class_addmethod(c, (method)explorer::explore_mess_cb, "explore", A_GIMME, 0);
  class_addmethod(c, (method)explorer::assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)explorer::notify, "notify", A_CANT, 0);

  search_sort_filter::setup_attribute<explorer>(c);

  class_register(CLASS_BOX, ossia_library.ossia_explorer_class);
}

extern "C" void* ossia_explorer_new(t_symbol*, long argc, t_atom* argv)
{
  auto x = make_ossia<explorer>();
  x->m_otype = object_class::explorer;
  x->m_dumpout = outlet_new(x, NULL);

  object_attach_byptr_register(x, x, CLASS_BOX);
  attr_args_process(x, (short)argc, argv);
  return x;
}

void explorer::free(explorer* x)
{
  if(x)
  {
    x->~explorer();
  }
}

t_max_err
explorer::notify(explorer* x, t_symbol* s, t_symbol* msg, void* sender, void* data)
{
  return 0;
}

void explorer::assist(explorer* x, void* b, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
  {
    sprintf(s, "Log messages inlet");
  }
}

#pragma mark -
#pragma mark t_explorer structure functions

explorer::explorer() = default;

explorer::~explorer()
{
  outlet_delete(m_dumpout);
}

void explorer::explore_mess_cb(explorer* x, t_symbol* s, long argc, t_atom* argv)
{
  x->m_name = _sym_nothing;
  if(argc > 0 && argv->a_type == A_SYM)
  {
    x->m_name = argv->a_w.w_sym;
    x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
    x->update_path();
  }

  std::vector<std::shared_ptr<matcher>> matchers;

  if(x->m_name == _sym_nothing)
  {
    auto obj = x->find_parent_object();
    if(obj)
    {
      // Explore at the current hierarchy level of ossia.explorer
      matchers = obj->m_matchers;
      x->m_addr_scope = obj->m_addr_scope;
      x->m_name = obj->m_name;
    }
    else
    {
      // ossia.explorer is not in a hierarchy: explore from / of the device instead
      matchers = {std::make_shared<matcher>(
          &ossia_max::instance().get_default_device()->get_root_node(), nullptr)};
      x->m_name = gensym("/");
      x->m_addr_scope = ossia::net::address_scope::absolute;
    }
  }
  else
  {
    // Exploring from a specified address, e.g. explore /foo.*
    matchers = x->find_or_create_matchers();
    if(matchers.empty())
    {
      matchers = {std::make_shared<matcher>(
          &ossia_max::instance().get_default_device()->get_root_node(), nullptr)};
    }
  }

  // get namespace of given nodes
  auto nodes = x->sort_and_filter(matchers);

  // Pretty print it
  dump_node_list(x->m_dumpout, nodes, x->m_format);
}

bool explorer::unregister()
{
  m_node_selection.clear();
  m_matchers.clear();

  return true;
}
