#include <ossia-max/src/device_base.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia-max/src/ossia-max.hpp>

namespace ossia
{
namespace max
{

void device_base::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();
  std::string addr = ossia::net::address_string_from_node(node);
  t_atom a[2];
  A_SETSYM(a, gensym("create"));
  A_SETSYM(a+1, gensym(addr.c_str()));
  outlet_anything(m_dumpout, gensym("parameter"), 2, a);
}

void device_base::on_unhandled_message_callback(const std::string addr, const ossia::value& val)
{
  std::vector<t_atom> va;
  t_atom a;
  A_SETSYM(&a, gensym(addr.c_str()));
  va.push_back(std::move(a));
  value2atom vm{va};
  val.apply(vm);

  outlet_anything(m_dumpout, gensym("osc"), va.size(), va.data());
}

void device_base::on_parameter_deleted_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();
  std::string addr = ossia::net::address_string_from_node(node);
  t_atom a[2];
  A_SETSYM(a, gensym("delete"));
  A_SETSYM(a+1, gensym(addr.c_str()));
  outlet_anything(m_dumpout, gensym("parameter"), 2, a);
}

void device_base::on_attribute_modified_callback(ossia::net::node_base& node, const std::string& attribute)
{
  if (node.get_parameter())
  {
    for ( auto param : ossia_max::instance().parameters.reference() )
    {
      for ( auto& m : param->m_matchers )
      {
        if ( m->get_node() == &node )
          parameter::update_attribute((ossia::max::parameter*)m->get_parent(),attribute, &node);
      }
    }

    for ( auto remote : ossia_max::instance().remotes.reference() )
    {
      for ( auto& m : remote->m_matchers )
      {
        if ( m->get_node() == &node )
          remote::update_attribute((ossia::max::remote*)m->get_parent(),attribute, &node);
      }
    }

    for ( auto attr : ossia_max::instance().attributes.reference() )
    {
      for ( auto& m : attr->m_matchers )
      {
        if ( m->get_node() == &node )
          attribute::update_attribute((ossia::max::attribute*)m->get_parent(),attribute, &node);
      }
    }
  } else {
    for ( auto model : ossia_max::instance().models.reference() )
    {
      for ( auto& m : model->m_matchers )
      {
        if ( m->get_node() == &node )
          node_base::update_attribute((node_base*)m->get_parent(),attribute, &node);
      }
    }

    for ( auto view : ossia_max::instance().views.reference() )
    {
      for ( auto& m : view->m_matchers )
      {
        if ( m->get_node() == &node )
          node_base::update_attribute((node_base*)m->get_parent(),attribute, &node);
          ;
      }
    }
  }
}

void device_base::on_node_renamed_callback(
    ossia::net::node_base& node,
    const std::string& old_name)
{
  auto name = node.get_name();
  std::string addr = ossia::net::address_string_from_node(node);
  std::string old_addr = addr.substr(0, addr.size()-name.size()) + old_name;
  t_atom a[3];
  A_SETSYM(a, gensym("rename"));
  A_SETSYM(a+1, gensym(old_addr.c_str()));
  A_SETSYM(a+2, gensym(addr.c_str()));
  outlet_anything(m_dumpout, gensym("node"), 3, a);
}

void device_base::on_node_created_callback(ossia::net::node_base& node)
{
  std::string addr = ossia::net::address_string_from_node(node);
  t_atom a[2];
  A_SETSYM(a, gensym("created"));
  A_SETSYM(a+1, gensym(addr.c_str()));
  outlet_anything(m_dumpout, gensym("node"), 2, a);
}

void device_base::on_node_removing_callback(ossia::net::node_base& node)
{
  std::string addr = ossia::net::address_string_from_node(node);
  t_atom a[2];
  A_SETSYM(a, gensym("removed"));
  A_SETSYM(a+1, gensym(addr.c_str()));
  outlet_anything(m_dumpout, gensym("node"), 2, a);
}

void device_base::connect_slots()
{
  if (m_device)
  {
    m_device->on_parameter_created.connect<&device_base::on_parameter_created_callback>(this);
    m_device->on_parameter_removing.connect<&device_base::on_parameter_deleted_callback>(this);
    m_device->on_unhandled_message.connect<&device_base::on_unhandled_message_callback>(this);
    m_device->on_attribute_modified.connect<&device_base::on_attribute_modified_callback>();
    // TODO add callback for node creation request,
    // node deletion request and node_rename request

    m_device->on_node_renamed.connect<&device_base::on_node_renamed_callback>(this);
    m_device->on_node_created.connect<&device_base::on_node_created_callback>(this);
    m_device->on_node_removing.connect<&device_base::on_node_removing_callback>(this);

    m_matchers.emplace_back(std::make_shared<t_matcher>(&m_device->get_root_node(), nullptr));
    // This is to handle [get address( message only
    // so is it really needed ?
    assert(!m_matchers.empty());
    m_node_selection.push_back(m_matchers[0].get());
  }
}


void device_base::disconnect_slots()
{
  if (m_device)
  {
    m_device->on_parameter_created.disconnect<&device_base::on_parameter_created_callback>(this);
    m_device->on_parameter_removing.disconnect<&device_base::on_parameter_deleted_callback>(this);
    m_device->on_unhandled_message.disconnect<&device_base::on_unhandled_message_callback>(this);

    m_device->on_attribute_modified.disconnect<&device_base::on_attribute_modified_callback>();

    m_device->on_node_renamed.disconnect<&device_base::on_node_renamed_callback>(this);
    m_device->on_node_created.disconnect<&device_base::on_node_created_callback>(this);
    m_device->on_node_removing.disconnect<&device_base::on_node_removing_callback>(this);
    // TODO add callback for command request
  }
}

} // namespace max
} // namespace ossia
