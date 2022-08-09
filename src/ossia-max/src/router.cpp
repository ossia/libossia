// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/common/websocket_log_sink.hpp>

#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/router.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <regex>

using namespace ossia::max_binding;

#pragma mark -
#pragma mark ossia_router class methods

extern "C" void ossia_router_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.router class
  ossia_library.ossia_router_class = class_new(
      "ossia.router", (method)ossia_router_new, (method)router::free,
      (long)sizeof(router), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_router_class;
  class_addmethod(c, (method)router::in_anything, "anything", A_GIMME, 0);
  class_addmethod(c, (method)router::assist, "assist", A_CANT, 0);

  CLASS_ATTR_LONG(c, "truncate", 0, router, m_truncate);
  CLASS_ATTR_STYLE(c, "truncate", 0, "onoff");
  CLASS_ATTR_LABEL(c, "truncate", 0, "Truncate matching part of address (default on)");

  class_register(CLASS_BOX, ossia_library.ossia_router_class);
}

extern "C" void* ossia_router_new(t_symbol* s, long argc, t_atom* argv)
{
  auto x = make_ossia<router>(argc, argv);

  x->m_patterns.reserve(argc);
  x->m_outlets.reserve(argc + 1);

  // extra outlet for non matching addresses
  x->m_outlets.push_back(outlet_new(x, nullptr));

  long attrstart = attr_args_offset(argc, argv);
  attr_args_process(x, argc - attrstart, argv + attrstart);

  argc = attrstart;

  if(argc == 0)
  {
    x->m_inlets.push_back(proxy_new(x, 2, 0L));
    x->m_outlets.push_back(outlet_new(x, nullptr));
  }
  else
  {
    int inlet_id = 0;
    x->m_patterns.resize(argc);
    while(argc--)
    {
      if(argv[argc].a_type == A_SYM)
      {
        std::string pattern(argv[argc].a_w.w_sym->s_name);
        x->change_pattern(inlet_id++, pattern);
        x->m_inlets.push_back(proxy_new(x, argc + 1, 0L));

        x->m_outlets.push_back(outlet_new(x, nullptr));
      }
      else
      {
        object_error(&x->m_object, "wrong arg type, should be symbol");
      }
    }
  }

  return x;
}

void router::change_pattern(int index, std::string pattern)
{
  if(!pattern.empty() && pattern[0] == '/')
  {
    pattern = pattern.substr(1);
  }
  ossia::net::expand_ranges(pattern);
  pattern = ossia::traversal::substitute_characters(pattern);

  try
  {
    m_patterns[index] = std::regex("^/?" + pattern + "($|/)");
  }
  catch(std::exception& e)
  {
    error("'%s' bad regex: %s", pattern.data(), e.what());
  }
}

void router::in_anything(router* x, t_symbol* s, long argc, t_atom* argv)
{
  std::string address(s->s_name);

  long inlet = proxy_getinlet((t_object*)x);

  if(inlet > 0)
  {
    x->change_pattern(x->m_inlets.size() - inlet, address);
  }
  else
  {
    bool match = false;
    for(int i = 0; i < x->m_patterns.size(); i++)
    {
      const auto& pattern_regex = x->m_patterns[i];

      std::smatch smatch;
      if(std::regex_search(address, smatch, pattern_regex))
      {
        match = true;
        std::string newaddress = address;
        if(x->m_truncate)
          newaddress = smatch.suffix();
        if(newaddress.size() > 0)
        {
          outlet_anything(x->m_outlets[i + 1], gensym(newaddress.c_str()), argc, argv);
        }
        else
        {
          outlet_list(x->m_outlets[i + 1], nullptr, argc, argv);
        }
      }
    }

    if(!match)
      outlet_anything(x->m_outlets[0], s, argc, argv);
  }
}

void router::free(router* x)
{
  if(x)
  {
    for(auto out : x->m_outlets)
      outlet_delete(out);
    for(auto in : x->m_inlets)
      proxy_delete(in);
    x->~router();
  }
}

void router::assist(router* x, void* b, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
  {
    sprintf(s, "Address input");
  }
  else if(m == ASSIST_OUTLET)
  {
    sprintf(s, "Outlet number %ld", a);
  }
}

#pragma mark -
#pragma mark t_router structure functions

router::router(long argc, t_atom* argv)
{
}
