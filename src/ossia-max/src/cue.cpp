#include <ossia-max/src/cue.hpp>
#include <ossia-max/src/ossia-max.hpp>

using namespace ossia::max_binding;
void* ossia_cue_create(t_symbol* s, long argc, t_atom* argv);
extern "C" OSSIA_MAX_EXPORT void ossia_cue_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.cue class
  ossia_library.ossia_cue_class = class_new(
      "ossia.cue", (method)ossia_cue_create, (method)ocue::free,
      (long)sizeof(ocue), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_cue_class;
  class_addmethod(c, (method)+[] (ocue* x, t_symbol*, int argc, t_atom* argv) { x->create(argc, argv); }, "create", A_GIMME, 0);
  class_addmethod(c, (method)+[] (ocue* x, t_symbol*, int argc, t_atom* argv) { x->update(argc, argv); }, "update", A_GIMME, 0);
  class_addmethod(c, (method)+[] (ocue* x, t_symbol*, int argc, t_atom* argv) { x->recall(argc, argv); }, "recall", A_GIMME, 0);
  class_addmethod(c, (method)+[] (ocue* x, t_symbol*, int argc, t_atom* argv) { x->remove(argc, argv); }, "remove", A_GIMME, 0);
  class_addmethod(c, (method)+[] (ocue* x, t_symbol*) { x->clear(); }, "clear", A_NOTHING, 0);
  class_addmethod(c, (method)+[] (ocue* x, t_symbol*, int argc, t_atom* argv) { x->move(argc, argv); }, "move", A_GIMME, 0);
  class_addmethod(c, (method)+[] (ocue* x, t_symbol*, int argc, t_atom* argv) { x->output(argc, argv); }, "output", A_GIMME, 0);

  class_addmethod(c, (method)+[] (ocue* x, t_symbol*, int argc, t_atom* argv) { x->namespace_select(argc, argv); }, "namespace/select", A_GIMME, 0);
  class_addmethod(c, (method)+[] (ocue* x, t_symbol*, int argc, t_atom* argv) { x->namespace_deselect(argc, argv); }, "namespace/deselect", A_GIMME, 0);
  class_addmethod(c, (method)+[] (ocue* x, t_symbol*, int argc, t_atom* argv) { x->namespace_grab(argc, argv); }, "namespace/grab", A_GIMME, 0);
  /*
  class_addmethod(c, (method)ocue::in_long, "int", A_LONG, 0);
  class_addmethod(c, (method)ocue::reset, "reset", A_NOTHING, 0);
  class_addmethod(c, (method)ocue::assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)ocue::notify, "notify", A_CANT, 0);
  class_addmethod(c, (method)ocue::closebang, "closebang", A_CANT, 0);
*/
  CLASS_ATTR_SYM(c, "name", 0, ocue, m_name);
  CLASS_ATTR_LABEL(c, "name", 0, "Test name");

  class_register(CLASS_BOX, ossia_library.ossia_cue_class);
}

void* ossia_cue_create(t_symbol* s, long argc, t_atom* argv)
{
  auto x = make_ossia<ocue>();

  if(x)
  {
      // The cue has to register to a device
      x->m_cues = std::make_shared<ossia::cues>();
    x->m_mainout = outlet_new(x, nullptr);
    x->m_dumpout = outlet_new(x, nullptr);

    // default attributes
    x->m_name = _sym_nothing;

    /*
    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    attr_args_process(x, argc - attrstart, argv + attrstart);

    if(attrstart > 0 && argv)
    {
      if(atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
      }
    }

    object_attach_byptr_register(x, x, CLASS_BOX);

    ocue::reset(x);
    ossia_max::instance().ocues.push_back(x);
    */
  }

  return x;
}

static void invoke_mem_fun(int argc, t_atom* argv, auto f)
{
    if(argc == 0)
    {
        f();
    }
    else if(argc == 1 && argv[0].a_type == A_SYM)
    {
        f(argv[0].a_w.w_sym);
    }
    else if(argc == 1 && argv[0].a_type == A_LONG)
    {
        f(argv[0].a_w.w_long);
    }
    else if(argc == 1 && argv[0].a_type == A_FLOAT)
    {
        f(argv[0].a_w.w_float);
    }
    else if(argc == 2 && argv[0].a_type == A_LONG && argv[1].a_type == A_LONG)
    {
        f(argv[0].a_w.w_long, argv[1].a_w.w_long);
    }
    else if(argc == 2 && argv[0].a_type == A_LONG && argv[1].a_type == A_FLOAT)
    {
        f(argv[0].a_w.w_long, argv[1].a_w.w_float);
    }
    else if(argc == 2 && argv[0].a_type == A_FLOAT && argv[1].a_type == A_LONG)
    {
        f(argv[0].a_w.w_float, argv[1].a_w.w_long);
    }
    else if(argc == 2 && argv[0].a_type == A_FLOAT && argv[1].a_type == A_FLOAT)
    {
        f(argv[0].a_w.w_float, argv[1].a_w.w_float);
    }
    else if(argc == 2 && argv[0].a_type == A_SYM && argv[1].a_type == A_LONG)
    {
        f(argv[0].a_w.w_sym, argv[1].a_w.w_long);
    }
    else if(argc == 2 && argv[0].a_type == A_SYM && argv[1].a_type == A_FLOAT)
    {
        f(argv[0].a_w.w_sym, argv[1].a_w.w_float);
    }
}
void ocue::create(int argc, t_atom* argv)
{
    invoke_mem_fun(argc, argv, [this] (auto&&... args) {
        if constexpr(requires { this->m_cues->create(args...); })
       this->m_cues->create(args...);
    });
}

void ocue::update(int argc, t_atom* argv)
{
    invoke_mem_fun(argc, argv, [this] (auto&&... args) {
        if constexpr(requires { this->m_cues->create(args...); })
       this->m_cues->update(args...);
    });
}
void ocue::recall(int argc, t_atom* argv)
{}
void ocue::remove(int argc, t_atom* argv)
{}
void ocue::clear()
{
    m_cues->clear();
}
void ocue::move(int argc, t_atom* argv)
{}
void ocue::output(int argc, t_atom* argv)
{}

void ocue::namespace_select(int argc, t_atom* argv)
{}
void ocue::namespace_deselect(int argc, t_atom* argv)
{}
void ocue::namespace_grab(int argc, t_atom* argv)
{}
void ocue::reset(ocue* x)
{
}

void ocue::in_long(ocue* x, long v)
{

  critical_enter(0);
  critical_exit(0);
}

void ocue::closebang(ocue* x)
{
}

void ocue::free(ocue* x)
{
  if(x)
  {
    // ossia_max::instance().ocues.remove_all(x);
    outlet_delete(x->m_dumpout);
    x->~ocue();
  }
}

t_max_err
ocue::notify(ocue* x, t_symbol* s, t_symbol* msg, void* sender, void* data)
{
  return 0;
}

void ocue::assist(ocue* x, void* b, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
  {
    sprintf(s, "Log messages inlet");
  }
}
