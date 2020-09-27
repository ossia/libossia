// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/fuzzysearch.hpp>
#include <ossia-max/src/ossia-max.hpp>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_fuzzysearch class methods

extern "C" void ossia_fuzzysearch_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.fuzzysearch class
  ossia_library.ossia_fuzzysearch_class = class_new(
      "ossia.fuzzysearch", (method)ossia_fuzzysearch_new, (method)fuzzysearch::free,
      (long)sizeof(fuzzysearch), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_fuzzysearch_class;
  class_addmethod(
      c, (method)fuzzysearch::search,
      "search", A_GIMME, 0);
  class_addmethod(
      c, (method)fuzzysearch::assist,
      "assist", A_CANT, 0);
  class_addmethod(
      c, (method)fuzzysearch::notify,
      "notify", A_CANT, 0);

  class_register(CLASS_BOX, ossia_library.ossia_fuzzysearch_class);
}

extern "C" void* ossia_fuzzysearch_new(t_symbol* s, long argc, t_atom* argv)
{
  auto x = make_ossia<fuzzysearch>(argc, argv);
  x->m_root = &ossia_max::get_default_device()->get_root_node();
  x->m_outlet = outlet_new(x, nullptr);
  return x;
}

void fuzzysearch::search(fuzzysearch* x, t_symbol* s, long argc, t_atom* argv)
{
  if(argc == 0)
    return;

  std::vector<std::string> patterns;
  patterns.reserve(argc);
  for(int i = 0; i<argc; i++)
  {
    if(argv[i].a_type == A_SYM)
    {
      patterns.push_back(std::string(argv[i].a_w.w_sym->s_name));
    }
  }
  if(patterns.empty())
    return;

  auto matches = ossia::net::fuzzysearch(x->m_root, patterns);

  for(const auto& m : matches)
  {
    t_atom a[2];
    A_SETFLOAT(a, std::get<0>(m));
    A_SETSYM(a+1, gensym(std::get<1>(m).c_str()));
    outlet_anything(x->m_outlet, gensym("match"), 2, a);
  }
}

void fuzzysearch::free(fuzzysearch* x)
{
  if (x)
  {
    outlet_delete(x->m_outlet);
    x->~fuzzysearch();
  }
}

t_max_err fuzzysearch::notify(
    fuzzysearch* x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
  return 0;
}

void fuzzysearch::assist(fuzzysearch *x, void *b, long m, long a, char *s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "Log messages inlet");
  }
}

#pragma mark -
#pragma mark t_fuzzysearch structure functions

fuzzysearch::fuzzysearch(long argc, t_atom *argv)
{
  object_attach_byptr_register(this, this, CLASS_BOX);
}
