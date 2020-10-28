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
    auto patcher = x->m_patcher;
    ossia_max::instance().patchers[patcher].remotes.push_back(x);

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

    // need to schedule a loadbang because objects only receive a loadbang when patcher loads.
    x->m_reg_clock = clock_new(x, (method) object_base::loadbang);
    clock_set(x->m_reg_clock, 1);
  }

  return (x);
}

void remote::destroy(remote* x)
{
  auto pat_it = ossia_max::instance().patchers.find(x->m_patcher);
  if(pat_it != ossia_max::instance().patchers.end())
  {
    auto& pat_desc = pat_it->second;
    pat_desc.remotes.remove_all(x);
    if(pat_desc.empty())
    {
      ossia_max::instance().patchers.erase(pat_it);
    }
  }

  x->m_dead = true;
  x->unregister();

  ossia_max::instance().remotes.remove_all(x);
  outlet_delete(x->m_dumpout);
  outlet_delete(x->m_set_out);
  outlet_delete(x->m_data_out);
  x->~remote();
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
        x->unregister();
      else
        ossia_register(x);
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
      m_ounit = unit;
    else
    {
      object_error((t_object*)this, "wrong unit: %s", m_unit->s_name);
      m_ounit = std::nullopt;
      m_unit = gensym("");
      return;
    }

    for (auto& m : m_node_selection)
    {
      if ( m->get_node()->get_parameter()->get_value_type()
           != ossia::val_type::IMPULSE)
      {
        auto dst_unit = m->get_node()->get_parameter()->get_unit();
        if (!ossia::check_units_convertible(*m_ounit,dst_unit)){
          auto src = ossia::get_pretty_unit_text(*m_ounit);
          auto dst = ossia::get_pretty_unit_text(dst_unit);
          object_error((t_object*)this, "sorry I don't know how to convert '%s' into '%s'",
                       src.data(), dst.data() );
          m_ounit = std::nullopt;
          m_unit = gensym("");
          break;
        } else {
          outlet_anything(m->get_parent()->m_dumpout,gensym("address"),1, m->get_atom_addr_ptr());
          t_atom a;
          A_SETSYM(&a, m_unit);
          outlet_anything(m->get_parent()->m_dumpout,gensym("unit"),   1, &a);
          m->output_value(m->get_node()->get_parameter()->value());
        }
      }
    }

  } else {
    m_ounit = std::nullopt;
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

bool remote::register_node(const std::vector<std::shared_ptr<matcher>>& matchers, bool output_value)
{
  if(m_mute) return false;

  update_path();

  bool res = do_registration(matchers, output_value);

  if (res)
  {
    ossia_max::instance().nr_remotes.remove_all(this);
  }
  else
  {
    ossia_max::instance().nr_remotes.push_back(this);
  }

  if (m_matchers.empty() || m_is_pattern)
  {
    bool registered = false;
    if (m_addr_scope == ossia::net::address_scope::global)
    {
      std::size_t pos = std::string(m_name->s_name).find(":/");
      std::string device_name = std::string(m_name->s_name).substr(0,pos);
      if(ossia::traversal::is_pattern(device_name))
      {
        // TODO listen to device (local or remote) creation
        // in the meanwhile, when ossia.device or ossia.client is created / connected,
        // a new registration will be performed, this is suboptimal, but should work
      }
      else
      {
        for(auto& dev : ossia_max::instance().devices.reference())
        {
          if(dev->m_device->get_name() == device_name)
          {
            registered = true;
            m_devices.push_back(dev->m_device.get());
          }
        }

        if(!registered)
        {
          for(auto& dev : ossia_max::instance().devices.reference())
          {
            if(dev->m_device->get_name() == device_name)
            {
              registered = true;
              m_devices.push_back(dev->m_device.get());
            }
          }
        }
      }
    }
    else
    {
      if (m_addr_scope == ossia::net::address_scope::relative)
      {
        object_base* parent{};
        int model_level, view_level;
        // look for parent view/model to get full path in case it's a relative address
        auto model = find_parent_box_alive<ossia::max::model>(this, 0, &model_level);
        auto view = find_parent_box_alive<ossia::max::view>(this, 0, &view_level);

        if(model && view)
        {
          if(model_level < view_level)
          {
            parent = static_cast<object_base*>(model);
          }
          else
          {
            parent = static_cast<object_base*>(view);
          }
        }
        if(parent)
        {
          // build  full path to device/client
        }
      }
    }
  }

  return res;
}

void remote::on_device_deleted(const net::node_base& root)
{
  for(auto dev : m_devices.reference())
  {
    if(&dev->get_root_node() == &root)
    {
      dev->on_parameter_created.disconnect<&remote::on_parameter_created_callback>(this);
      dev->get_root_node().about_to_be_deleted.disconnect<&remote::on_device_deleted>(this);
      m_devices.remove_all(dev);
      break;
    }
  }
}

bool remote::do_registration(const std::vector<std::shared_ptr<matcher>>& matchers, bool output_value)
{
  object_post(&m_object, "register remote");

  m_registered = true;

  std::string name = m_name->s_name;

  for (auto& m : matchers)
  {
    auto node = m->get_node();
    if (m_addr_scope == ossia::net::address_scope::absolute)
    {
      // get root node
      node = &node->get_device().get_root_node();
      // and remove starting '/'
      name = name.substr(1);
    }

    m_parent_node = node;

    std::vector<ossia::net::node_base*> nodes{};

    if (m_addr_scope == ossia::net::address_scope::global)
      nodes = ossia::max::find_global_nodes(name);
    else
      nodes = ossia::net::find_nodes(*node, name);

    m_matchers.reserve(m_matchers.size() + nodes.size());

    for (auto n : nodes){

      bool continue_flag = false;

      // avoid to register the same node twice
      for (auto& m_m : m_matchers)
      {
        if ( m_m->get_node() == n && m_m->get_parent() == this )
        {
          continue_flag = true;
          break;
        }
      }

      if (continue_flag)
        continue;

      if (n->get_parameter()){
        m_matchers.emplace_back(std::make_shared<matcher>(n, this));
      } else {
        // if there is a node without address it might be a model
        // then look if that node have an eponyme child
        n = ossia::net::find_node(*n, fmt::format("{}/{}", name, name));

        if (n && n->get_parameter()){
          m_matchers.emplace_back(std::make_shared<matcher>(n, this));
        } else {
          continue;
        }
      }

      if (output_value && n->get_parameter()->get_value_type()
          != ossia::val_type::IMPULSE)
      {
        for(const auto& m : m_matchers)
        {
          auto v = m->get_node()->get_parameter()->value();
          if(v.valid())
          {
            m->output_value(v);
          }
        }
      }
    }
  }

  fill_selection();

  // do not put it in quarantine if it's a pattern
  // and even if it can't find any matching node
  return (!m_matchers.empty() || m_is_pattern);
}

bool remote::unregister()
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

  m_parent_node = nullptr;

  for(auto dev : m_devices.reference())
  {
    dev->on_parameter_created.disconnect<&remote::on_parameter_created_callback>(this);
    dev->get_root_node().about_to_be_deleted.disconnect<&remote::on_device_deleted>(this);
  }
  m_devices.clear();

  return true;
}

void remote::on_parameter_created_callback(const ossia::net::parameter_base& addr)
{
  auto& node = addr.get_node();

  if ( m_path )
  {
    if ( ossia::traversal::match(*m_path, node) )
    {
      if(m_addr_scope == net::address_scope::relative)
      {
        m_parent_node = node.get_parent();
        std::string name(m_name->s_name);
        size_t pos = name.find('/', 0);
        while(pos != std::string::npos)
        {
          m_parent_node = node.get_parent();
          pos = name.find('/',pos+1);
        }
      }
      m_matchers.emplace_back(std::make_shared<matcher>(&node,this));
      fill_selection();
    }
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

      if (x->m_ounit && !ossia::check_units_convertible(param->get_unit(), *x->m_ounit))
      {
        x->m_ounit = param->get_unit();
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
  if ( s == gensym("mute") )
    remote::get_mute(x);
  else
    parameter_base::get_mess_cb(x,s);
}

} // max namespace
} // ossia namespace
