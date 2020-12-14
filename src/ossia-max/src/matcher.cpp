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


void purge_parent(ossia::net::node_base* node)
{
  // remove parent node recursively if they are not used anymore
  if (auto pn = node->get_parent())
  {
    pn->remove_child(*node);
    if (pn->get_parent() && pn->children().size() == 0)
    {
      bool remove_me = true;
      for (auto model : ossia_max::instance().models.copy())
      {
        for (const auto& m : model->m_matchers)
        {
          if (m->get_node() == pn)
          {
            remove_me = false;
            break;
          }
        }
        if(!remove_me)
          break;
      }
      if (remove_me)
        purge_parent(pn);
    }
  }
}

matcher::~matcher()
{
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

        for (auto remote : ossia_max::instance().remotes.copy())
        {
          auto matchers_copy = remote->m_matchers;
          for (auto m : matchers_copy)
          {
            if(m && *m == *this)
            {
              if(m->is_locked())
              {
                m->set_zombie();
              }
              else
              {
                ossia::remove_erase(remote->m_matchers,m);
              }
            }
          }
          // ossia::remove_erase_if(remote->m_matchers, [this] (auto& other) { return *other == *this; });
        }

        for (auto attribute : ossia_max::instance().attributes.copy())
        {
          ossia::remove_erase_if(attribute->m_matchers, [this] (auto& other) { return *other == *this; });
        }

        purge_parent(node);
      }
      // if the vector is empty
      // remote should be quarantinized
      if (owner->m_matchers.size() == 0)
      {
        switch(owner->m_otype)
        {
          case object_class::model:
            ossia_max::instance().nr_models.push_back((model*) owner);
            break;
          case object_class::param:
            ossia_max::instance().nr_parameters.push_back((parameter*) owner);
            break;
          default:
              ;
        }
      }
    } else {

      if (!owner->m_is_deleted && !m_zombie)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        node->about_to_be_deleted.disconnect<&object_base::on_node_removing>(owner);
      }

      // if there vector is empty
      // remote should be quarantinized
      if (owner->m_matchers.size() == 0)
      {
        switch(owner->m_otype)
        {
          case object_class::attribute:
            ossia_max::instance().nr_attributes.push_back((attribute*) owner);
            break;
          case object_class::remote:
            ossia_max::instance().nr_remotes.push_back((remote*) owner);
            break;
          case object_class::view:
            ossia_max::instance().nr_views.push_back((view*) owner);
            break;
          default:
              ;
        }
      }
    }
    owner = nullptr;
  }
  ossia_max::instance().s_node_matchers_map[node].remove_all(this);
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
