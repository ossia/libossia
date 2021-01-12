#include <ossia-max/src/matcher.hpp>
#include <ossia-max/src/object_base.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>

#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia/network/value/value_conversion.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>

#include <regex>

namespace ossia
{
namespace max
{

matcher::matcher(ossia::net::node_base* n, object_base* p) :
    node{n}, owner{p}, callbackit{std::nullopt}
{
  if (owner)
  {
    switch(owner->m_otype)
    {
      case object_class::remote:
      case object_class::param:
        if (auto param = node->get_parameter())
          callbackit = param->add_callback(
              [=](const ossia::value& v) { output_value(v); });
        break;
      default:
        break;
    }

    node->about_to_be_deleted.connect<&object_base::on_node_removing>(owner);
  }

  set_addr_symbol();

  ossia_max::instance().s_node_matchers_map[node].push_back(this);
}

matcher::matcher(matcher&& other)
{
  node = other.node;
  other.node = nullptr;

  owner = other.owner;
  other.owner = nullptr;

  callbackit = other.callbackit;
  other.callbackit = std::nullopt;

  m_addr = other.m_addr;
  m_dead = other.m_dead;

  if(node && !m_dead)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      if (owner)
      {
        switch(owner->m_otype)
        {
          case object_class::remote:
          case object_class::param:
            callbackit = param->add_callback(
                [=] (const ossia::value& v) { output_value(v); });
            break;
          default:
            break;
        }

        set_addr_symbol();
      }
    }
  }

  ossia_max::instance().s_node_matchers_map[node].remove_all(&other);
  ossia_max::instance().s_node_matchers_map[node].push_back(this);
}

matcher& matcher::operator=(matcher&& other)
{
  node = other.node;
  other.node = nullptr;

  owner = other.owner;
  other.owner = nullptr;

  callbackit = other.callbackit;
  other.callbackit = std::nullopt;

  m_addr = other.m_addr;
  m_dead = other.m_dead;

  if(node && !m_dead)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      if (owner && !owner->m_is_deleted)
      {
        switch(owner->m_otype)
        {
          case object_class::remote:
          case object_class::param:
            callbackit = param->add_callback(
                [=] (const ossia::value& v) { output_value(v); });
            break;
          default:
            break;
        }

        set_addr_symbol();
      }
    }
  }

  ossia_max::instance().s_node_matchers_map[node].remove_all(&other);
  ossia_max::instance().s_node_matchers_map[node].push_back(this);

  return *this;
}

matcher::~matcher()
{
  auto& map = ossia_max::instance().s_node_matchers_map;
  map[node].remove_all(this);
  if(owner)
  {
    // purge selection
    ossia::remove_one(owner->m_node_selection,this);

    if (   owner->m_otype == object_class::param
        || owner->m_otype == object_class::model )
    {
      if (!owner->m_is_deleted)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        node->about_to_be_deleted.disconnect<&object_base::on_node_removing>(owner);

        if(owner->m_otype == object_class::param)
        {
          node->remove_parameter();
        }

        auto _node = node;
        auto parent = _node->get_parent();
        while(parent)
        {
          if(_node->children().size()> 0)
            break;

          parent->remove_child(_node->get_name());

          if(parent->get_parameter())
            break;

          if(map.find(parent) != map.end())
            break;

          _node = parent;
          parent = parent->get_parent();
        }
      }
    } else {
      auto param = node->get_parameter();
      if (param && callbackit) param->remove_callback(*callbackit);
      node->about_to_be_deleted.disconnect<&object_base::on_node_removing>(owner);
    }
    owner = nullptr;
  }
  node = nullptr;
}

void matcher::output_value(ossia::value v)
{
  if(owner->m_otype == object_class::param || owner->m_otype == object_class::remote)
  {
    auto pbase = static_cast<parameter_base*>(owner);
    if(pbase->m_set_flag)
      return;
  }

  auto param = node->get_parameter();
  auto filtered = ossia::net::filter_value(
      param->get_domain(),
      std::move(v),
      param->get_bounding());

  // FIXME filter_value return false but an invalid filtered value
  if(!param->filter_value(filtered) && filtered.valid())
  {
    auto x = (parameter_base*) owner;

    ossia::value val;
    if ( x->m_local_unit == std::nullopt )
    {
      val = std::move(filtered);
    }
    else
    {
      val = ossia::convert(std::move(filtered), param->get_unit(), *x->m_local_unit);
    }

    if(owner->m_dumpout)
      outlet_anything(owner->m_dumpout,gensym("address"),1,&m_addr);

    value_visitor<object_base> vm;
    vm.x = (object_base*)owner;
    val.apply(vm);
  }
}

void matcher::set_addr_symbol()
{
  if (!m_dead && node && owner){
    std::string addr = ossia::net::address_string_from_node(*node);

    if(owner->m_trim_addr > 0)
    {
      switch(owner->m_addr_scope)
      {
        case ossia::net::address_scope::relative:
        {
          auto parent = owner->find_parent_object();
          if(parent)
          {
            for(const auto& m : parent->m_matchers)
            {
              std::string node_addr = ossia::net::address_string_from_node(*m->get_node());

              size_t offset = node_addr.back() == '/' ? 0 : 1;

              if(addr.rfind(node_addr,0) == 0)
              {
                addr = addr.substr(node_addr.size()+offset);
                break;
              }
            }
          }
          else
          {
            auto pos = addr.find(":");
            addr = addr.substr(pos+2);
          }
          break;
        }
        case ossia::net::address_scope::absolute:
        {
          auto pos = addr.find(":");
          addr = addr.substr(pos+1);
          break;
        }
        default:
            ;
      }
    }

    A_SETSYM(&m_addr, gensym(addr.c_str()));
  }
}

} // namespace max
} // namespace ossia
