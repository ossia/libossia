// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/attribute.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <ossia/network/common/path.hpp>

using namespace ossia::max;

extern "C" void ossia_attribute_setup()
{
  auto c = class_new( "ossia.attribute",
      (method)attribute::create,
      (method)attribute::destroy,
      (long)sizeof(attribute), 0L,
      A_GIMME, 0);

  parameter_base::class_setup(c);

  class_addmethod(
        c, (method)attribute::assist,
        "assist", A_CANT, 0);
  class_addmethod(
        c, (method)attribute::notify,
        "notify", A_CANT, 0);

  class_addmethod(c, (method) parameter_base::get_mess_cb, "get",  A_SYM, 0);
  class_addmethod(c, (method) address_mess_cb<attribute>, "address",   A_SYM, 0);


  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_attribute_class = c;
}

namespace ossia
{
namespace max
{


void attribute::assist(attribute* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "Remote parameter attribute messages");
  }
  else
  {
    switch(a)
    {
      case 0:
        sprintf(s, "Remote parameter attribute value");
        break;
      default:
        ;
    }
  }
}

t_max_err attribute::notify(attribute *x, t_symbol *s,
                            t_symbol *msg, void *sender, void *data)
{
  t_symbol *attrname;

  if (!x->m_lock && msg == gensym("attr_modified")) {
    attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));

    if ( attrname == gensym("unit") )
      x->set_unit();
    else if ( attrname == gensym("type") )
      x->set_type();
    else
      parameter_base::notify(x, s, msg, sender, data);
  }
  return 0;
}

void attribute::do_registration()
{
  m_registered = true;

  m_matchers = find_or_create_matchers();

  m_selection_path.reset();
  fill_selection();
}

void attribute::unregister()
{
  m_node_selection.clear();
  m_matchers.clear();

  ossia_max::instance().nr_attributes.push_back(this);

  if(m_dev)
  {
    m_dev->on_parameter_created.disconnect<&attribute::on_parameter_created_callback>(this);
    m_dev->get_root_node().about_to_be_deleted.disconnect<&attribute::on_device_deleted>(this);
  }
  m_dev = nullptr;
}

void attribute::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();

  if( ossia::traversal::match(get_path(), node) )
  {
    m_matchers.emplace_back(std::make_shared<matcher>(&node,this));
    fill_selection();
  }
}

void attribute::on_device_deleted(const net::node_base &)
{
  m_dev = nullptr;
}

void* attribute::create(t_symbol* name, int argc, t_atom* argv)
{
  auto x = make_ossia<attribute>();

  if (x)
  {
    critical_enter(0);
    ossia_max::get_patcher_descriptor(x->m_patcher).attributes.push_back(x);

    x->m_otype = object_class::attribute;
    x->m_dumpout = outlet_new(x, NULL);

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
      }
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    // Register object to istself so it can receive notification when attribute changed
    // This is not documented anywhere, please look at :
    // https://cycling74.com/forums/notify-when-attribute-changes
    object_attach_byptr_register(x, x, CLASS_BOX);

    defer_low(x, (method) object_base::loadbang, nullptr, 0, nullptr);

    critical_exit(0);
  }

  return (x);
}

void attribute::destroy(attribute* x)
{
  critical_enter(0);
  x->m_dead = true;
  x->unregister();

  if(x->m_is_pattern && x->m_dev)
  {
    x->m_dev->on_parameter_created.disconnect<&attribute::on_parameter_created_callback>(x);
    x->m_dev->get_root_node().about_to_be_deleted.disconnect<&attribute::on_device_deleted>(x);
  }

  outlet_delete(x->m_dumpout);
  x->~attribute();
  critical_exit(0);
}

} // pd namespace
} // ossia namespace
