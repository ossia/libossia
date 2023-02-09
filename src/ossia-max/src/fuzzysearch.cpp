// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/fuzzysearch.hpp>
#include <ossia-max/src/ossia-max.hpp>

using namespace ossia::max_binding;

#pragma mark -
#pragma mark ossia_fuzzysearch class methods

static t_symbol* s_sym_global = gensym("Global");
static t_symbol* s_sym_absolute = gensym("Absolute");
static t_symbol* s_sym_relative = gensym("Relative");

extern "C" void ossia_fuzzysearch_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.fuzzysearch class
  ossia_library.ossia_fuzzysearch_class = class_new(
      "ossia.fuzzysearch", (method)ossia_fuzzysearch_new, (method)fuzzysearch::free,
      (long)sizeof(fuzzysearch), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_fuzzysearch_class;
  class_addmethod(c, (method)fuzzysearch::search, "search", A_GIMME, 0);
  class_addmethod(c, (method)fuzzysearch::assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)fuzzysearch::notify, "notify", A_CANT, 0);

  search_filter::setup_attribute<fuzzysearch>(c);

  CLASS_ATTR_SYM(c, "scope", 0, fuzzysearch, m_scope);
  CLASS_ATTR_LABEL(c, "scope", 0, "Search scope");
  CLASS_ATTR_ENUMINDEX3(c, "scope", 0, "Global", "Absolute", "Relative");

  CLASS_ATTR_LONG(c, "insensitive", 0, fuzzysearch, m_case_sensitive);
  CLASS_ATTR_STYLE_LABEL(
      c, "insensitive", 0, "onoff", "Make fuzzy-search case-sensitive");

  class_register(CLASS_BOX, ossia_library.ossia_fuzzysearch_class);
}

extern "C" void* ossia_fuzzysearch_new(t_symbol* s, long argc, t_atom* argv)
{
  auto x = make_ossia<fuzzysearch>();

  object_attach_byptr_register(x, x, CLASS_BOX);
  x->m_scope = s_sym_global;
  x->m_outlet = outlet_new(x, nullptr);
  return x;
}

void fuzzysearch::search(fuzzysearch* x, t_symbol* s, long argc, t_atom* argv)
{
  if(argc == 0)
    return;

  auto& patterns = x->patterns;
  auto& matches = x->matches;

  patterns.clear();
  patterns.reserve(argc);
  for(int i = 0; i < argc; i++)
  {
    if(argv[i].a_type == A_SYM)
    {
      patterns.emplace_back(argv[i].a_w.w_sym->s_name);
    }
  }
  if(patterns.empty())
    return;

  if(x->m_scope == s_sym_global)
  {
    // gather all known devices
    // first all ZeroConf devices
    x->m_roots.clear();
    auto oscq_devices = ZeroconfOscqueryListener::get_devices();
    auto minuit_devices = ZeroconfMinuitListener::get_devices();

    for(const auto& dev : oscq_devices)
    {
      x->m_roots.insert(dev.get());
    }
    for(const auto& dev : minuit_devices)
    {
      x->m_roots.insert(dev.get());
    }

    // then all devices and client that Max already knows
    for(const auto& dev : ossia_max::instance().devices.reference())
    {
      x->m_roots.insert(&dev->m_device->get_root_node());
    }
    for(const auto& clt : ossia_max::instance().clients.reference())
    {
      if(clt->m_device)
      {
        x->m_roots.insert(&clt->m_device->get_root_node());
      }
    }
    x->m_roots.insert(&ossia_max::instance().get_default_device()->get_root_node());
  }
  else if(x->m_scope == s_sym_absolute)
  {
  }
  else // relative
  {
  }

  matches.clear();

  ossia::net::fuzzysearch_options opt{};
  opt.case_sensitive = x->m_case_sensitive != 0;
  ossia::net::fuzzysearch(
      {x->m_roots.begin(), x->m_roots.end()}, patterns, matches, opt);

  ossia::remove_erase_if(matches, [&](const ossia::net::fuzzysearch_result& m) {
    return x->filter(*m.node);
  });

  t_atom size;
  A_SETLONG(&size, matches.size());
  outlet_anything(x->m_outlet, gensym("size"), 1, &size);

  for(const auto& m : matches)
  {
    t_atom a[2];
    A_SETFLOAT(a, m.score);
    A_SETSYM(a + 1, gensym(m.oscname.c_str()));
    outlet_anything(x->m_outlet, gensym("match"), 2, a);
  }

  patterns.clear();
  matches.clear();
}

void fuzzysearch::free(fuzzysearch* x)
{
  if(x)
  {
    outlet_delete(x->m_outlet);
    x->~fuzzysearch();
  }
}

t_max_err
fuzzysearch::notify(fuzzysearch* x, t_symbol* s, t_symbol* msg, void* sender, void* data)
{
  return 0;
}

void fuzzysearch::assist(fuzzysearch* x, void* b, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
  {
    sprintf(s, "Log messages inlet");
  }
}

#pragma mark -
#pragma mark t_fuzzysearch structure functions

fuzzysearch::fuzzysearch() = default;
fuzzysearch::~fuzzysearch() = default;
