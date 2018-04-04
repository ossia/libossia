#include "device_base.hpp"
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia-pd/src/ossia-pd.hpp>

namespace ossia
{
namespace pd
{

device_base::device_base(t_eclass* c) :
  node_base(c)
{ }

void device_base::on_parameter_created_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();
  std::string addr = ossia::net::address_string_from_node(node);
  t_atom a[2];
  SETSYMBOL(a, gensym("create"));
  SETSYMBOL(a+1, gensym(addr.c_str()));
  outlet_anything(m_dumpout, gensym("parameter"), 2, a);
}

void device_base::on_parameter_deleted_callback(const ossia::net::parameter_base& param)
{
  auto& node = param.get_node();
  std::string addr = ossia::net::address_string_from_node(node);
  t_atom a[2];
  SETSYMBOL(a, gensym("delete"));
  SETSYMBOL(a+1, gensym(addr.c_str()));
  outlet_anything(m_dumpout, gensym("parameter"), 2, a);
}

void device_base::on_attribute_modified_callback(const ossia::net::node_base& node, ossia::string_view attribute)
{
  if (node.get_parameter())
  {
    for ( auto param : ossia_pd::instance().parameters.reference() )
    {
      for ( auto& m : param->m_matchers )
      {
        if ( m->get_node() == &node )
          parameter::update_attribute((ossia::pd::parameter*)m->get_owner(),attribute, &node);
      }
    }

    for ( auto remote : ossia_pd::instance().remotes.reference() )
    {
      for ( auto& m : remote->m_matchers )
      {
        if ( m->get_node() == &node )
          remote::update_attribute((ossia::pd::remote*)m->get_owner(),attribute, &node);
      }
    }

    for ( auto attr : ossia_pd::instance().attributes.reference() )
    {
      for ( auto& m : attr->m_matchers )
      {
        if ( m->get_node() == &node )
          attribute::update_attribute((ossia::pd::attribute*)m->get_owner(),attribute, &node);
      }
    }
  } else {
    for ( auto model : ossia_pd::instance().models.reference() )
    {
      for ( auto& m : model->m_matchers )
      {
        if ( m->get_node() == &node )
          model::update_attribute((ossia::pd::model*)m->get_owner(),attribute, &node);
      }
    }

    for ( auto view : ossia_pd::instance().views.reference() )
    {
      for ( auto& m : view->m_matchers )
      {
        if ( m->get_node() == &node )
          view::update_attribute((ossia::pd::view*)m->get_owner(),attribute, &node);
      }
    }
  }
}

void device_base::connect_slots()
{
  if (m_device)
  {
    m_device->on_parameter_created.connect<device_base, &device_base::on_parameter_created_callback>(this);
    m_device->on_parameter_removing.connect<device_base, &device_base::on_parameter_deleted_callback>(this);
    // TODO add callback for message
    // x->m_device->on_message.connect<t_client, &t_client::on_message_callback>(x);
    m_device->on_attribute_modified.connect<&device_base::on_attribute_modified_callback>();

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
    m_device->on_parameter_created.disconnect<device_base, &device_base::on_parameter_created_callback>(this);
    m_device->on_parameter_removing.disconnect<device_base, &device_base::on_parameter_deleted_callback>(this);
    // TODO add callback for message
    // x->m_device->on_message.disconnect<t_client, &t_client::on_message_callback>(x);
    m_device->on_attribute_modified.disconnect<&device_base::on_attribute_modified_callback>();

    m_node_selection.clear();
    m_matchers.clear();
  }
}

} // namespace pd
} // namespace ossia

