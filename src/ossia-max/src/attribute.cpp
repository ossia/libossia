// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/common/path.hpp>

#include <ossia-max/src/attribute.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

using namespace ossia::max_binding;

extern "C" void ossia_attribute_setup()
{
  auto c = class_new(
      "ossia.attribute", (method)attribute::create, (method)attribute::destroy,
      (long)sizeof(attribute), 0L, A_GIMME, 0);

  parameter_base::class_setup(c);

  class_addmethod(c, (method)attribute::assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)parameter_base::notify, "notify", A_CANT, 0);

  class_addmethod(c, (method)parameter_base::get_mess_cb, "get", A_SYM, 0);
  class_addmethod(c, (method)address_mess_cb<attribute>, "address", A_SYM, 0);

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_attribute_class = c;
}

namespace ossia
{
namespace max_binding
{

void attribute::assist(attribute* x, void* b, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
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
      default:;
    }
  }
}

void attribute::on_device_removing(device_base* obj)
{
  auto dev = obj->m_device.get();
  dev->on_parameter_created.disconnect<&attribute::on_parameter_created_callback>(this);
  dev->on_node_renamed.disconnect<&attribute::on_node_renamed_callback>(this);
}

void attribute::on_device_created(device_base* obj)
{
  auto dev = obj->m_device.get();
  // no need to connect to on_node_removing because ossia::max::matcher
  // already connect to it
  dev->on_parameter_created.connect<&attribute::on_parameter_created_callback>(this);
  dev->on_node_renamed.connect<&attribute::on_node_renamed_callback>(this);
}

void wrapper(void* x)
{
  if(ossia_max::instance().config.autoregister)
    static_cast<attribute*>(x)->do_registration();
}

void attribute::on_node_renamed_callback(ossia::net::node_base& node, const std::string&)
{
  // first remove the matcher with old name
  remove_matchers(node);

  schedule(this, (method)wrapper, 0, nullptr, 0, nullptr);
}

void attribute::on_parameter_created_callback(const ossia::net::parameter_base& addr)
{
  schedule(this, (method)wrapper, 0, nullptr, 0, nullptr);
}

void attribute::do_registration()
{
  return;
  if(m_name && std::string_view(m_name->s_name) != "")
  {
    clear_and_init_registration();

    m_registered = true;
  }
}

void attribute::unregister()
{
  auto copy = m_matchers;
  for(auto& m : copy)
  {
    if(m->is_locked())
    {
      m->set_zombie();
    }
    else
    {
      ossia::remove_erase(m_matchers, m);
    }
  }

  m_registered = false;
}

void* attribute::create(t_symbol* name, int argc, t_atom* argv)
{
  auto x = make_ossia<attribute>();

  if(x)
  {
    critical_enter(0);
    ossia_max::get_patcher_descriptor(x->m_patcher).attributes.push_back(x);

    device_base::on_device_created.connect<&attribute::on_device_created>(x);
    device_base::on_device_removing.connect<&attribute::on_device_removing>(x);

    std::vector<ossia::net::generic_device*> devs = get_all_devices();
    for(auto dev : devs)
    {
      dev->on_parameter_created.connect<&attribute::on_parameter_created_callback>(x);
      dev->on_node_renamed.connect<&attribute::on_node_renamed_callback>(x);
    }

    x->m_otype = object_class::attribute;
    x->m_dumpout = outlet_new(x, NULL);

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if(attrstart && argv)
    {
      if(atom_gettype(argv) == A_SYM)
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

    defer_low(x, (method)object_base::loadbang, nullptr, 0, nullptr);

    critical_exit(0);
  }

  return (x);
}

void attribute::destroy(attribute* x)
{
  critical_enter(0);
  x->m_dead = true;
  x->unregister();

  for(auto dev : get_all_devices())
  {
    dev->on_parameter_created.disconnect<&attribute::on_parameter_created_callback>(x);
    dev->on_node_renamed.disconnect<&attribute::on_node_renamed_callback>(x);
  }

  device_base::on_device_created.disconnect<&attribute::on_device_created>(x);
  device_base::on_device_removing.disconnect<&attribute::on_device_removing>(x);

  outlet_delete(x->m_dumpout);
  x->~attribute();
  critical_exit(0);
}

} // pd namespace
} // ossia namespace
