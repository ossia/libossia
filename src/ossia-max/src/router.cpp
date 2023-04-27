// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/common/websocket_log_sink.hpp>

#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/router.hpp>

#include <ossia/network/common/path.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <regex>

using namespace ossia::max_binding;

static void print_atom_list(std::string name, long argc, t_atom* argv)
{
  name += ": ";
  t_atom t;
  t.a_type = A_SYM;
  t.a_w.w_sym = gensym(name.data());
  postatom(&t);
  for(int i = 0; i < argc; i++)
  {
    postatom(argv + i);
  }
  post("\n");
}

extern "C" void ossia_router_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.router class
  ossia_library.ossia_router_class = class_new(
      "ossia.router", (method)ossia_router_new, (method)router::free,
      (long)sizeof(router), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_router_class;
  class_addmethod(c, (method)router::in_anything, "anything", A_GIMME, 0);

  class_addmethod(c, (method)router::in_int, "int", A_LONG, A_GIMME, 0);
  class_addmethod(c, (method)router::in_float, "float", A_FLOAT, 0);
  class_addmethod(c, (method)router::in_symbol, "symbol", A_SYM, 0);
  class_addmethod(c, (method)router::in_list, "list", A_GIMME, 0);

  class_addmethod(c, (method)router::assist, "assist", A_CANT, 0);

  CLASS_ATTR_LONG(c, "truncate", 0, router, m_truncate);
  CLASS_ATTR_STYLE(c, "truncate", 0, "onoff");
  CLASS_ATTR_LABEL(c, "truncate", 0, "Truncate matching part of address (default on)");

  CLASS_ATTR_LONG(c, "leading-slash", 0, router, m_leadslash);
  CLASS_ATTR_STYLE(c, "leading-slash", 0, "onoff");
  CLASS_ATTR_LABEL(c, "leading-slash", 0, "Add / to the outputs (default off)");

  class_register(CLASS_BOX, ossia_library.ossia_router_class);
}

extern "C" void* ossia_router_new(t_symbol* s, long argc, t_atom* argv)
{
  auto x = make_ossia<router>();

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
        x->change_pattern(inlet_id++, std::string(argv[argc].a_w.w_sym->s_name));
        x->m_inlets.push_back(proxy_new(x, argc + 1, 0L));

        x->m_outlets.push_back(outlet_new(x, nullptr));
      }
      else if(argv[argc].a_type == A_LONG)
      {
        x->change_pattern(inlet_id++, std::to_string(argv[argc].a_w.w_long));
        x->m_inlets.push_back(proxy_new(x, argc + 1, 0L));

        x->m_outlets.push_back(outlet_new(x, nullptr));
      }
      else if(argv[argc].a_type == A_FLOAT)
      {
        x->change_pattern(inlet_id++, std::to_string(argv[argc].a_w.w_float));
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

void router::change_pattern(int index, std::string&& pattern)
{
  if(!pattern.empty() && pattern[0] == '/')
  {
    pattern = pattern.substr(1);
  }

  const bool simple = ossia::traversal::is_pattern(pattern);
  ossia::net::expand_ranges(pattern); // FIXME check if this shouldn't be "canonicalize_str" ??
  pattern = ossia::traversal::substitute_characters(pattern);

  try
  {
    m_patterns[index].pattern = pattern;
    m_patterns[index].simple = simple;
    if(!simple)
      m_patterns[index].regex = std::regex("^/?" + pattern + "($|/)");
  }
  catch(std::exception& e)
  {
    error("'%s' bad regex: %s", pattern.data(), e.what());
  }
}

static bool process_simple(router* x, const std::string& address, int i, long argc, t_atom* argv)
{
  const auto& pattern_regex = x->m_patterns[i].regex;

  std::smatch smatch;
  if(std::regex_search(address, smatch, pattern_regex))
  {
    const std::string& newaddress = (x->m_truncate) ? smatch.suffix() : address;
    if(newaddress.size() > 0)
    {
      outlet_anything(x->m_outlets[i + 1], gensym(newaddress.c_str()), argc, argv);
    }
    else
    {
      outlet_list(x->m_outlets[i + 1], nullptr, argc, argv);
    }

    return true;
  }

  return false;
}

static bool process_regex(router* x, const std::string& address, int i, long argc, t_atom* argv)
{
  const auto& pattern_regex = x->m_patterns[i].regex;

  std::smatch smatch;
  if(std::regex_search(address, smatch, pattern_regex))
  {
    const std::string& newaddress = (x->m_truncate) ? smatch.suffix() : address;
    if(newaddress.size() > 0)
    {
      outlet_anything(x->m_outlets[i + 1], gensym(newaddress.c_str()), argc, argv);
    }
    else
    {
      outlet_list(x->m_outlets[i + 1], nullptr, argc, argv);
    }

    return true;
  }

  return false;
}

void router::in_anything(router* x, t_symbol* s, long argc, t_atom* argv)
{
  long inlet = proxy_getinlet((t_object*)x);

  if(inlet > 0)
  {
    x->change_pattern(x->m_inlets.size() - inlet, std::string(s->s_name));
  }
  else
  {
    std::string_view address = s->s_name;

    auto process = [x, argc, argv](std::string_view str, void* outlet) {
      if(str.size() > 0)
      {
        // Normal routing case
        t_atom* l = (t_atom*)alloca(sizeof(t_atom) * (argc + 1));
        if(x->m_leadslash && str[0] != '/')
        {
          static thread_local std::string r;
          r.clear();
          r.reserve(str.size() + 1);
          r.push_back('/');
          r.append(str);
          atom_setsym(&l[0], gensym(r.c_str()));
        }
        else
        {
          atom_setsym(&l[0], gensym(str.data()));
        }
        for(int i = 1; i < argc + 1; i++)
        {
          l[i] = argv[i - 1];
        }

        outlet_list(outlet, _sym_list, argc + 1, l);
      }
      else
      {
        // Here we are at the end of the string, the only thing remaining to route
        // are the arguments, e.g. from "/foo 123" we only get "123"
        outlet_list(outlet, _sym_list, argc, argv);
      }
    };

    bool match = false;
    for(std::size_t i = 0; i < x->m_patterns.size(); i++)
    {
      auto& pat = x->m_patterns[i];
      auto outlet = x->m_outlets[i + 1];

      if(pat.simple)
      {
        if(address.starts_with('/'))
        {
          auto aa = address.substr(1);
          if(aa.starts_with(pat.pattern))
          {
            auto aaa = aa.substr(pat.pattern.size());
            if(aaa.empty() || aaa[0] == '/') {
              match = true;
              if(x->m_truncate)
                process(aaa, outlet);
              else
                process(address, outlet);
              break;
            }
          }
        }
        else
        {
          if(address.starts_with(pat.pattern))
          {
            auto aaa = address.substr(pat.pattern.size());
            if(aaa.empty() || aaa[0] == '/') {
              match = true;
              if(x->m_truncate)
                process(aaa, outlet);
              else
                process(address, outlet);
              break;
            }
          }
        }
      }
      else
      {
        const auto& pattern_regex = x->m_patterns[i].regex;

        std::cmatch smatch;
        if(std::regex_search(
               address.data(), address.data() + address.size(), smatch, pattern_regex))
        {
          match = true;
          if(x->m_truncate)
            process(std::string_view(
                        smatch.suffix().first, smatch.suffix().second - smatch.suffix().first), outlet);
          else
            process(address, outlet);
          break;
        }
      }
    }

    if(!match)
    {
      t_atom* l = (t_atom*)alloca(sizeof(t_atom) * (argc + 1));
      atom_setsym(&l[0], s);
      for(int i = 1; i < argc + 1; i++)
      {
        l[i] = argv[i - 1];
      }

      // Here the input message does not match any pattern
      // registered for the router
      outlet_list(x->m_outlets[0], _sym_list, argc + 1, l);
    }
  }

}

void router::in_float(router* x, double f) { }

void router::in_int(router* x, long f) { }

void router::in_symbol(router* x, t_symbol* f) { }

void router::in_list(router* x, t_symbol* f, int argc, t_atom* argv)
{
  if(argc == 0)
    return;
  if(argv[0].a_type != A_SYM)
    return;

  return in_anything(x, argv[0].a_w.w_sym, argc - 1, argv + 1);
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

router::router() = default;
router::~router() = default;
