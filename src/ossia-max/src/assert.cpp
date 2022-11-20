#include <ossia-max/src/assert.hpp>
#include <ossia-max/src/ossia-max.hpp>

using namespace ossia::max_binding;

extern "C" void ossia_assert_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.assert class
  ossia_library.ossia_assert_class = class_new(
      "ossia.assert", (method)ossia_assert_create, (method)oassert::free,
      (long)sizeof(oassert), 0L, A_GIMME, 0);

  auto& c = ossia_library.ossia_assert_class;
  class_addmethod(c, (method)oassert::in_long, "int", A_LONG, 0);
  class_addmethod(c, (method)oassert::reset, "reset", A_NOTHING, 0);
  class_addmethod(c, (method)oassert::assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)oassert::notify, "notify", A_CANT, 0);
  class_addmethod(c, (method)oassert::closebang, "closebang", A_CANT, 0);

  CLASS_ATTR_SYM(c, "name", 0, oassert, m_name);
  CLASS_ATTR_LABEL(c, "name", 0, "Test name");

  class_register(CLASS_BOX, ossia_library.ossia_assert_class);
}

extern "C" void* ossia_assert_create(t_symbol* s, long argc, t_atom* argv)
{
  auto x = make_ossia<oassert>();

  if(x)
  {
    x->m_dumpout = outlet_new(x, nullptr);

    // default attributes
    x->m_name = _sym_nothing;

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

    oassert::reset(x);
  }

  return x;
}

void oassert::reset(oassert* x)
{
  x->m_status = NORUN;
  x->update_color();
}
void oassert::save_color()
{
  t_object* box{};

  // get the object's wrapping box
  t_max_err err = object_obex_lookup(this, gensym("#B"), (t_object**)&box);
  if(err != MAX_ERR_NONE)
    return;

  err = jbox_get_color(box, &m_color);
  if(err != MAX_ERR_NONE)
    return;
}

void oassert::update_color()
{
  t_object* box{};

  // get the object's wrapping box
  t_max_err err = object_obex_lookup(this, gensym("#B"), (t_object**)&box);
  if(err != MAX_ERR_NONE)
    return;

  // invert the color
  t_jrgba new_color{};
  new_color.alpha = 1.0;

  switch(m_status)
  {
    case NORUN:
      new_color = m_color;
      break;
    case FAIL:
      new_color.red = 1.0;
      break;
    case SUCCESS:
      new_color.green = 1.0;
      break;
  }

  jbox_set_color(box, &new_color);
}

void oassert::in_long(oassert* x, long v)
{
  if(v == 0)
  {
    x->m_status = FAIL;
  }
  else
  {
    x->m_status = SUCCESS;
  }
  x->update_color();

  critical_enter(0);
  if(!ossia_max::instance().loggers.empty())
  {
    auto buffer = x->create_report();
    for(auto logger : ossia_max::instance().loggers.reference())
    {
      logger->m_con->socket.send_message(buffer);
    }
  }
  critical_exit(0);
}

void oassert::closebang(oassert* x)
{
  if(x->m_status == NORUN)
  {
    if(!ossia_max::instance().loggers.empty())
    {
      auto buffer = x->create_report();
      for(auto logger : ossia_max::instance().loggers.reference())
      {
        logger->m_con->socket.send_message(buffer);
      }
    }
  }
}

rapidjson::StringBuffer oassert::create_report()
{
  rapidjson::StringBuffer buffer;
  ossia::json_writer writer{buffer};
  writer.StartObject();

  writer.Key("operation");
  writer.String("assert");

  writer.Key("name");
  std::string name = m_name->s_name;
  writer.String(name.data(), name.size());

  writer.Key("status");
  std::string status;
  switch(m_status)
  {
    case NORUN:
      status = "norun";
      break;
    case FAIL:
      status = "fail";
      break;
    case SUCCESS:
      status = "success";
      break;
  }
  writer.String(status);

  writer.EndObject();

  return buffer;
}

void oassert::free(oassert* x)
{
  if(x)
  {
    outlet_delete(x->m_dumpout);
    x->~oassert();
  }
}

t_max_err
oassert::notify(oassert* x, t_symbol* s, t_symbol* msg, void* sender, void* data)
{
  return 0;
}

void oassert::assist(oassert* x, void* b, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
  {
    sprintf(s, "Log messages inlet");
  }
}
