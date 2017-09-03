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
  std::cout << attribute << std::endl;
  if (node.get_parameter())
  {
    for ( auto param : ossia_pd::instance().params.copy() )
    {
      for ( auto& m : param->m_matchers )
      {
        if ( m.get_node() == &node )
          //t_param::update_attribute((t_param*)m.parent,attribute);
          ;
      }
    }

    for ( auto remote : ossia_pd::instance().remotes.copy() )
    {
      for ( auto& m : remote->m_matchers )
      {
        if ( m.get_node() == &node )
          parameter_base::update_attribute((parameter_base*)m.get_parent(),attribute);
      }
    }
  } else {
    for ( auto model : ossia_pd::instance().models.copy() )
    {
      for ( auto& m : model->m_matchers )
      {
        if ( m.get_node() == &node )
          // t_model::update_attribute((t_model*)m.parent,attribute);
          ;
      }
    }

    for ( auto view : ossia_pd::instance().views.copy() )
    {
      for ( auto& m : view->m_matchers )
      {
        if ( m.get_node() == &node )
          // t_view::update_attribute((t_view*)m.parent,attribute);
          ;
      }
    }
  }
}

} // namespace pd
} // namespace ossia

