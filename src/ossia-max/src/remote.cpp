// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "remote.hpp"
#include "device.hpp"
#include "parameter.hpp"

#include <ossia/network/base/parameter.hpp>
#include <ossia-max/src/utils.hpp>
#include <ossia/network/common/path.hpp>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_remote class methods

extern "C" void ossia_remote_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.remote class
  t_class* c = class_new(
      "ossia.remote", (method)remote::create, (method)remote::destroy,
      (short)sizeof(remote), 0L, A_GIMME, 0);

  if (c)
  {
    parameter_base::class_setup(c);
    class_addmethod(
        c, (method)remote::assist,
        "assist", A_CANT, 0);
    class_addmethod(
        c, (method)remote::notify,
        "notify", A_CANT, 0);
    class_addmethod(c, (method) address_mess_cb<remote>, "address",   A_SYM, 0);

    class_addmethod(c, (method) remote::get_mess_cb, "get",  A_SYM, 0);

    CLASS_ATTR_LONG(c, "deferset", 0, remote, m_defer_set);
    CLASS_ATTR_STYLE(
          c, "deferset", 0, "onoff");
    CLASS_ATTR_LABEL(c, "deferset", 0, "Deferlow  for 'set' output");

  }

  class_register(CLASS_BOX, c);
  ossia_library.ossia_remote_class = c;
}

namespace ossia
{
namespace max
{

void* remote::create(t_symbol* name, long argc, t_atom* argv)
{
  auto x = make_ossia<remote>();

  if (x)
  {
    critical_enter(0);
    ossia_max::get_patcher_descriptor(x->m_patcher).remotes.push_back(x);
    device_base::on_device_created.connect<&remote::on_device_created>(x);
    device_base::on_device_removing.connect<&remote::on_device_removing>(x);

    std::vector<ossia::net::generic_device*> devs = get_all_devices();
    for(auto dev : devs)
    {
      if(!x->m_devices.contains(dev))
      {
        dev->on_parameter_created.connect<&remote::on_parameter_created_callback>(x);
        x->m_devices.push_back(dev);
      }
    }

    x->m_otype = object_class::remote;

    // make outlets:
    // anything outlet to dump remote state
    x->m_dumpout = outlet_new(x, NULL);
    // anything outlet to output data
    x->m_data_out = outlet_new(x, NULL);
    // anything outlet to output data for ui
    x->m_set_out  = outlet_new(x, NULL);

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

void remote::destroy(remote* x)
{
  critical_enter(0);

  x->m_dead = true;
  x->unregister();

  ossia_max::instance().remotes.remove_all(x);
  outlet_delete(x->m_dumpout);
  outlet_delete(x->m_set_out);
  outlet_delete(x->m_data_out);

  device_base::on_device_created.disconnect<&remote::on_device_created>(x);
  device_base::on_device_removing.disconnect<&remote::on_device_removing>(x);

  x->~remote();
  critical_exit(0);
}

void remote::assist(remote* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "Remote input", a);
  }
  else
  {
    switch(a)
    {
    case 0:
      sprintf(s, "Remote parameter deferred value (with set prefix for UI connection)", a);
        break;
      case 1:
        sprintf(s, "Remote parameter value", a);
        break;
      case 2:
        sprintf(s, "Dumpout", a);
        break;
      default:
        break;
    }
  }
}


t_max_err remote::notify(remote *x, t_symbol *s,
                       t_symbol *msg, void *sender, void *data)
{
  t_symbol *attrname;

  if (!x->m_lock && msg == gensym("attr_modified") && sender == x) {
    attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));

    if ( attrname == gensym("unit") )
      x->set_unit();
    else if ( attrname == gensym("mute") )
    {
      if (x->m_mute)
      {
        x->m_node_selection.clear();
        x->m_matchers.clear();
      }
      else
      {
        x->do_registration();
      }
    }
    else
      parameter_base::notify(x, s, msg, sender, data);

  }
  return 0;
}

void remote::set_unit()
{
  if ( m_unit !=  gensym("") )
  {
    ossia::unit_t unit = ossia::parse_pretty_unit(m_unit->s_name);
    if (unit)
      m_local_unit = unit;
    else
    {
      object_error((t_object*)this, "wrong unit: %s", m_unit->s_name);
      m_local_unit = std::nullopt;
      m_unit = gensym("");
      return;
    }

    for (auto& m : m_node_selection)
    {
      if ( m->get_node()->get_parameter()->get_value_type()
           != ossia::val_type::IMPULSE)
      {
        auto dst_unit = m->get_node()->get_parameter()->get_unit();
        if (dst_unit)
        {
          if(ossia::check_units_convertible(*m_local_unit,dst_unit))
          {
            outlet_anything(m->get_owner()->m_dumpout,gensym("address"),1, m->get_atom_addr_ptr());
            t_atom a;
            A_SETSYM(&a, m_unit);
            outlet_anything(m->get_owner()->m_dumpout,gensym("unit"),   1, &a);
            m->output_value(m->get_node()->get_parameter()->value());
          }
          else
          {
            auto src = ossia::get_pretty_unit_text(*m_local_unit);
            auto dst = ossia::get_pretty_unit_text(dst_unit);
            object_error((t_object*)this, "sorry I don't know how to convert '%s' into '%s'",
                         src.data(), dst.data() );
            m_local_unit = std::nullopt;
            break;
          }
        }
        else
        {
          m_local_unit = std::nullopt;
        }
      }
    }

  } else {
    m_local_unit = std::nullopt;
  }
}

void remote::get_unit(remote*x)
{
  t_atom a;
  if (x->m_unit)
  {
    A_SETSYM(&a,x->m_unit);
    outlet_anything(x->m_dumpout, gensym("unit"), 1, &a);
  } else
    outlet_anything(x->m_dumpout, gensym("unit"), 0, NULL);
}

void remote::get_mute(remote*x)
{
  t_atom a;
  A_SETFLOAT(&a,x->m_mute);
  outlet_anything(x->m_dumpout, gensym("mute"), 1, &a);
}

void remote::on_device_removing(device_base* obj)
{
  auto dev = obj->m_device.get();
  if(m_devices.contains(dev))
  {
    dev->on_parameter_created.disconnect<&remote::on_parameter_created_callback>(this);
    m_devices.remove_all(dev);
  }
}

void remote::on_device_created(device_base* obj)
{
  auto dev = obj->m_device.get();
  if(!m_devices.contains(dev))
  {
    // no need to connect to on_node_removing because ossia::max::matcher
    // already connect to it
    dev->on_parameter_created.connect<&remote::on_parameter_created_callback>(this);
    m_devices.push_back(dev);
  }
}

void remote::do_registration()
{
  std::string name = m_name->s_name;

  m_registered = true;

  m_matchers=find_or_create_matchers();

  m_selection_path.reset();
  fill_selection();

  set_unit();
}

void remote::unregister()
{
  auto copy = m_matchers;
  for (auto& m : copy)
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

  ossia_max::instance().nr_remotes.push_back(this);

  for(auto dev : m_devices.reference())
  {
    dev->on_parameter_created.disconnect<&remote::on_parameter_created_callback>(this);
  }
  m_devices.clear();

  m_registered = false;
}

void remote::on_parameter_created_callback(const ossia::net::parameter_base& addr)
{
  auto& node = addr.get_node();
  auto oscaddr = ossia::net::address_string_from_node(node);

  if ( ossia::traversal::match(get_path(), node) )
  {
    m_matchers.emplace_back(std::make_shared<matcher>(&node,this));
    fill_selection();
    set_unit();
  }
}

void remote::update_attribute(remote* x, ossia::string_view attribute, const ossia::net::node_base* node)
{
  // @mute and @unit attributes are specific to each remote
  // it makes no sens to sens to change when an attribute changes

  if ( attribute == ossia::net::text_unit()) {
    // assume all matchers have the same bounding_mode
    assert(!x->m_matchers.empty());

    std::shared_ptr<ossia::max::matcher> good_one{};

    for(auto& m : x->m_matchers)
    {
      if(!m->is_zombie())
      {
        good_one = m;
        break;
      }
    }

    if(good_one)
    {
      ossia::net::node_base* node = good_one->get_node();
      ossia::net::parameter_base* param = node->get_parameter();

      if (x->m_local_unit && !ossia::check_units_convertible(param->get_unit(), *x->m_local_unit))
      {
        x->m_local_unit = param->get_unit();
        std::string_view unit = ossia::get_pretty_unit_text(param->get_unit());
        x->m_unit = gensym(unit.data());
      }
    }

  }  else if ( attribute == ossia::net::text_extended_type() ){
    auto matchers = make_matchers_vector(x,node);
    get_type(x, matchers);
  } else {
    parameter_base::update_attribute(x, attribute, node);
  }
}

void remote::get_mess_cb(remote* x, t_symbol* s)
{
  if ( s == gensym("unit") )
    remote::get_unit(x);
  else if ( s == gensym("mute") )
    remote::get_mute(x);
  else
    parameter_base::get_mess_cb(x,s);
}

} // max namespace
} // ossia namespace
